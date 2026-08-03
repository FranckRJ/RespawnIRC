#include <QJsonDocument>
#include <QJsonParseError>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QRegularExpression>

#include <cstring>
#include <zlib.h>

#include "payloadTool.hpp"
#include "logTool.hpp"
#include "configDependentVar.hpp"

namespace
{
    /* JVC sérialise le payload dans un `<script>` en fin de page. L'attribut est
     * parfois précédé du garde `window.jvc=window.jvc||{};`, on ne s'appuie que sur
     * le nom de la variable pour être robuste aux variations de mise en forme. */
    const QRegularExpression expForPayload(R"rgx(window\.jvc\.forumsAppPayload\s*=\s*"([^"]*)")rgx",
                                           configDependentVar::regexpBaseOptions);

    struct cacheEntryStruct
    {
        uint hashOfSource = 0;
        int sizeOfSource = 0;
        QJsonObject payload;
    };

    QMutex mutexForCache;
    QList<cacheEntryStruct> cacheOfPayloads;
    const int maxNumberOfCachedPayloads = 4;

    bool findInCache(uint hashOfSource, int sizeOfSource, QJsonObject& payloadFound)
    {
        QMutexLocker locker(&mutexForCache);

        for(int i = 0; i < cacheOfPayloads.size(); ++i)
        {
            if(cacheOfPayloads[i].hashOfSource == hashOfSource && cacheOfPayloads[i].sizeOfSource == sizeOfSource)
            {
                /* Remis en tête pour que le cache reste utile quand on alterne entre
                 * deux pages (le mode « deux dernières pages » fait exactement ça). */
                cacheEntryStruct entry = cacheOfPayloads.takeAt(i);
                payloadFound = entry.payload;
                cacheOfPayloads.push_front(entry);
                return true;
            }
        }

        return false;
    }

    void addToCache(uint hashOfSource, int sizeOfSource, const QJsonObject& payload)
    {
        QMutexLocker locker(&mutexForCache);

        cacheEntryStruct entry;
        entry.hashOfSource = hashOfSource;
        entry.sizeOfSource = sizeOfSource;
        entry.payload = payload;
        cacheOfPayloads.push_front(entry);

        while(cacheOfPayloads.size() > maxNumberOfCachedPayloads)
        {
            cacheOfPayloads.pop_back();
        }
    }
}

QByteArray payloadTool::gzipUncompress(const QByteArray& compressedData)
{
    if(compressedData.isEmpty() == true)
    {
        return QByteArray();
    }

    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    /* 15 pour la taille de fenêtre maximale, + 32 pour accepter aussi bien un
     * en-tête gzip qu'un en-tête zlib. */
    if(inflateInit2(&stream, 15 + 32) != Z_OK)
    {
        qWarning(logParsing) << "gzipUncompress : inflateInit2 a échoué.";
        return QByteArray();
    }

    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));
    stream.avail_in = static_cast<uInt>(compressedData.size());

    QByteArray uncompressedData;
    const int sizeOfChunk = 64 * 1024;
    QByteArray chunk(sizeOfChunk, Qt::Uninitialized);
    int resultOfInflate = Z_OK;

    do
    {
        stream.next_out = reinterpret_cast<Bytef*>(chunk.data());
        stream.avail_out = sizeOfChunk;

        resultOfInflate = inflate(&stream, Z_NO_FLUSH);

        if(resultOfInflate != Z_OK && resultOfInflate != Z_STREAM_END && resultOfInflate != Z_BUF_ERROR)
        {
            qWarning(logParsing) << "gzipUncompress : inflate a échoué, code" << resultOfInflate;
            inflateEnd(&stream);
            return QByteArray();
        }

        uncompressedData.append(chunk.constData(), sizeOfChunk - static_cast<int>(stream.avail_out));

        /* Z_BUF_ERROR sans progrès possible signifie qu'il n'y a plus rien à lire. */
        if(resultOfInflate == Z_BUF_ERROR && stream.avail_in == 0)
        {
            break;
        }
    } while(resultOfInflate != Z_STREAM_END);

    inflateEnd(&stream);

    if(resultOfInflate != Z_STREAM_END && uncompressedData.isEmpty() == true)
    {
        return QByteArray();
    }

    return uncompressedData;
}

bool payloadTool::sourceContainsPayload(const QString& source)
{
    return source.contains("forumsAppPayload");
}

QJsonObject payloadTool::getPayload(const QString& source)
{
    if(source.isEmpty() == true)
    {
        return QJsonObject();
    }

    uint hashOfSource = qHash(source);
    int sizeOfSource = source.size();
    QJsonObject cachedPayload;

    if(findInCache(hashOfSource, sizeOfSource, cachedPayload) == true)
    {
        return cachedPayload;
    }

    QJsonObject payload;
    QRegularExpressionMatch matchForPayload = expForPayload.match(source);

    if(matchForPayload.hasMatch() == false)
    {
        qWarning(logParsing) << "Payload introuvable dans la page (" << sizeOfSource << "caractères ).";
    }
    else
    {
        QByteArray uncompressedData = gzipUncompress(QByteArray::fromBase64(matchForPayload.captured(1).toLatin1()));

        if(uncompressedData.isEmpty() == true)
        {
            qWarning(logParsing) << "Décompression du payload impossible.";
        }
        else
        {
            QJsonParseError errorOfParse;
            QJsonDocument document = QJsonDocument::fromJson(uncompressedData, &errorOfParse);

            if(document.isObject() == false)
            {
                qWarning(logParsing) << "Payload décompressé illisible :" << errorOfParse.errorString();
            }
            else
            {
                payload = document.object();
                qDebug(logParsing) << "Payload décodé, clés :" << payload.keys();
            }
        }
    }

    addToCache(hashOfSource, sizeOfSource, payload);

    return payload;
}

void payloadTool::clearCache()
{
    QMutexLocker locker(&mutexForCache);
    cacheOfPayloads.clear();
}
