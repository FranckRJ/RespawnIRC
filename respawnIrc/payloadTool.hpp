#ifndef PAYLOADTOOL_HPP
#define PAYLOADTOOL_HPP

#include <QByteArray>
#include <QJsonObject>
#include <QString>

/* Depuis 2026, jeuxvideo.com ne rend plus les messages ni la liste des sujets en
 * HTML dans la page : tout est fourni dans un objet JSON encodé dans
 * `window.jvc.forumsAppPayload`, en base64 d'un gzip (l'équivalent PHP côté JVC
 * étant `base64_encode(gzencode(json_encode(...), 6))`).
 *
 * Ce module s'occupe d'extraire et de décoder cet objet. Le résultat est mis en
 * cache : les différentes fonctions de parsingTool reçoivent chacune la source de
 * la page, sans quoi elles la décoderaient toutes séparément. */
namespace payloadTool
{
    /* Retourne le payload de cette page, ou un objet vide si la page n'en contient
     * pas ou qu'il est illisible. */
    QJsonObject getPayload(const QString& source);

    /* Vrai si la page contient bien la balise du payload, qu'il ait pu être décodé
     * ou non. Sert à distinguer « page d'erreur / challenge Cloudflare » de
     * « page de forum dont le décodage a raté ». */
    bool sourceContainsPayload(const QString& source);

    /* Décompresse un flux gzip. Retourne un tableau vide en cas d'échec. Exposé pour
     * les tests. */
    QByteArray gzipUncompress(const QByteArray& compressedData);

    /* Vide le cache. Utile pour les tests et pour ne pas garder de grosses pages en
     * mémoire quand on change de topic. */
    void clearCache();
}

#endif
