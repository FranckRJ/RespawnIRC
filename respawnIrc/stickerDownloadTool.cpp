#include "stickerDownloadTool.hpp"
#include "parsingTool.hpp"

stickerDownloadToolClass::stickerDownloadToolClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

void stickerDownloadToolClass::updateListOfStickers()
{
    QDir stickerDir(QCoreApplication::applicationDirPath() + "/resources/stickers/");
    QStringList listOfFullNameStickers;
    listOfStickers.clear();

    if(stickerDir.exists() == true)
    {
        listOfFullNameStickers = stickerDir.entryList(QDir::Files);
    }

    for(const QString& thisSticker : listOfFullNameStickers)
    {
        if(thisSticker.endsWith(".png") == true)
        {
            listOfStickers.append(thisSticker.left(thisSticker.size() - 4));
        }
    }
}

bool stickerDownloadToolClass::checkIfStickerExist(QString stickerName)
{
    if(listOfStickers.indexOf(stickerName) != -1)
    {
        return true;
    }
    else
    {
        QFileInfo stickerFile(QCoreApplication::applicationDirPath() + "/resources/stickers/" + stickerName + ".png");

        if(stickerFile.exists() == true && stickerFile.isFile() == true)
        {
            listOfStickers.append(stickerName);
            return true;
        }
        else
        {
            return false;
        }
    }
}

void stickerDownloadToolClass::checkAndStartDownloadMissingStickers(QStringList listOfStickersToCheck)
{
    for(QString thisSticker : listOfStickersToCheck)
    {
        if(listOfStickersNeedDownload.indexOf(thisSticker) == -1 && checkIfStickerExist(thisSticker) == false)
        {
            listOfStickersNeedDownload.push_back(thisSticker);
        }
    }

    startDownloadMissingStickers();
}

void stickerDownloadToolClass::startDownloadMissingStickers()
{
    if(listOfStickersNeedDownload.isEmpty() == false)
    {
        if(networkManager == nullptr)
        {
            networkManager = new QNetworkAccessManager(this);
        }

        if(reply == nullptr)
        {
            reply = networkManager->get(parsingToolClass::buildRequestWithThisUrl("http://jv.stkr.fr/p/" + listOfStickersNeedDownload.front()));

            if(reply->isOpen() == true)
            {
                QObject::connect(reply, &QNetworkReply::finished, this, &stickerDownloadToolClass::analyzeLatestStickerDownloaded);
            }
            else
            {
                analyzeLatestStickerDownloaded();
                networkManager->deleteLater();
                networkManager = nullptr;
            }
        }
    }
}

void stickerDownloadToolClass::analyzeLatestStickerDownloaded()
{
    if(reply->isReadable() == true)
    {
        QFile newSticker;
        newSticker.setFileName(QCoreApplication::applicationDirPath() + "/resources/stickers/" + listOfStickersNeedDownload.front() + ".png");
        newSticker.open(QIODevice::WriteOnly);
        newSticker.write(reply->readAll());
        newSticker.close();
        listOfStickers.append(listOfStickersNeedDownload.front());
    }
    reply->deleteLater();

    listOfStickersNeedDownload.pop_front();
    reply = nullptr;

    startDownloadMissingStickers();
}
