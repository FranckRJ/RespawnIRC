#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QUrl>
#include <QtGlobal>
#include <algorithm>

#include "parsingTool.hpp"
#include "styleTool.hpp"
#include "shortcutTool.hpp"
#include "configDependentVar.hpp"
#include "logTool.hpp"
#include "payloadTool.hpp"

namespace
{
    const QRegularExpression expForNormalLink(R"rgx(http(s)?://[^\(\)\]\[ \\<>]*)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxTimestamp(R"rgx(<input type="hidden" name="ajax_timestamp_liste_messages" id="ajax_timestamp_liste_messages" value="([^"]*)" />)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxHash(R"rgx(<input type="hidden" name="ajax_hash_liste_messages" id="ajax_hash_liste_messages" value="([^"]*)" />)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxModTimestamp(R"rgx(<input type="hidden" name="ajax_timestamp_moderation_forum" id="ajax_timestamp_moderation_forum" value="([^"]*)" />)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxModHash(R"rgx(<input type="hidden" name="ajax_hash_moderation_forum" id="ajax_hash_moderation_forum" value="([^"]*)" />)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForDeeplyNestedQuote(R"rgx(^> ?> ?> ?> ?>.*\n)rgx", configDependentVar::regexpBaseOptions | QRegularExpression::MultilineOption);
    const QRegularExpression expForSuccessiveTruncations(R"rgx((\f\[%<citation tronquee>%\]\f)+)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForVersionName(R"rgx("tag_name"[^"]*:[^"]*"([^"]*)")rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForVersionChangelog(R"rgx("body"[^"]*:[^"]*"(.*)")rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForFormTopic(R"rgx((<form role="form" class="form-post-topic[^"]*" method="post" action="".*?>.*?</form>))rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForFormConnect(R"rgx((<form role="form" class="form-connect-jv" method="post" action="".*?>.*?</form>))rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForInput(R"rgx(<input ([^=]*)="([^"]*)" ([^=]*)="([^"]*)" ([^=]*)="([^"]*)"/>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForTopicLocked(R"rgx(<div class="message-lock-topic">|<div class="lockInfo__reason">)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForError(R"rgx(<div class="alert alert-danger[^"]*">.*?<p class="([^"]*)">([^<]*)</p>.*?</div>)rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForErrorInJSON(R"rgx("(?:erreur|error)s?":\[?"([^"]*)")rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForMpJvc(R"rgx(<div class=".*?headerAccount--pm.*?">[^<]*<span[^c]*class="headerAccount__pm[^"]*".*?data-val="([^"]*)")rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForTopicLinkNumber(R"rgx((?<beforeTopicPage>https?://(?<domain>[^/]*)/forums/(?<mode>[^-]*)-(?<forumId>[^-]*)-(?<topicId>[^-]*)-)(?<topicPage>[^-]*)(?<afterTopicPage>-[^-]*-[^-]*-[^-]*-[^\.]*\.htm))rgx", configDependentVar::regexpBaseOptions);
    /* Le <title> a disparu des pages de forum en 2026, le seul endroit où le nom du
     * forum reste écrit en clair est le fil d'Ariane. */
    const QRegularExpression expForForumName(R"rgx(<h1 class="breadcrumb__item">([^<]*)</h1>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForJvfLink(R"rgx(https?://jvforum\.fr/([^/]*)/([^-]*)-([^/]*))rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForSmiley(R"rgx(<img src="http(s)?://image\.jeuxvideo\.com/smileys_img/([^"]*)" alt="[^"]*" data-code="([^"]*)" title="[^"]*" [^>]*>)rgx", configDependentVar::regexpBaseOptions);
    /* Les stickers « historiques » (img-stickers) et ceux de la refonte 2026
     * (message__sticker). Les images noelshack postées seules sont rendues avec
     * message__urlImgSticker et sont traitées comme des images noelshack, pas comme
     * des stickers. */
    const QRegularExpression expForStickers(R"rgx(<img class="(?:img-stickers|message__sticker)" src="([^"]*)".*?/>)rgx", configDependentVar::regexpBaseOptions);
    /* Depuis la refonte, les liens longs sont coupés avec <i class="message__cesure">
     * et <span class="message__middleCesure">, les classes sont donc optionnelles. */
    const QRegularExpression expForLongLink(R"rgx(<span +class="JvCare [^"]*"[^i]*itle="([^"]*)"[^>]*>[^<]*<i *(?:class="message__cesure")?></i><span *(?:class="message__middleCesure")?>[^<]*</span>[^<]*</span>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForShortLink(R"rgx(<span +class="JvCare [^"]*"[^>]*?target="_blank"[^>]*>([^<]*)</span>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForJvcLink(R"rgx(<a +(title="[^"]*" )?href="([^"]*)"( title="[^"]*")? *>.*?</a>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAlloCineLink(R"rgx(<a +target="[^"]*" +title="[^"]*" +href="([^"]*)"[^>]*>[^<]*<i *(?:class="message__cesure")?></i><span *(?:class="message__middleCesure")?>[^<]*</span>[^<]*</a>)rgx", configDependentVar::regexpBaseOptions);
    /* Ancien rendu : <img class="img-shack">, nouveau : <img class="message__urlImg ...">.
     * Dans les deux cas src est la miniature et alt l'URL de l'image en taille réelle.
     * Le `target="_blank""` du nouveau rendu contient un guillemet en trop, d'où le
     * [^>]* permissif avant le <img. */
    const QRegularExpression expForNoelshack(R"rgx(<span class="JvCare[^>]*><img class="(?:img-shack|message__urlImg)[^"]*"[^>]*src="http(s)?://([^"]*)" alt="([^"]*)"[^>]*>[^<]*</span>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForEmbedVideo(R"rgx(<div class="player-contenu"><div class="[^"]*"><iframe.*?src="([^"]*)"[^>]*></iframe></div></div>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForJvcVideo(R"rgx(<div class="player-contenu">.*?<div class="player-jv" id="player-jv-([^-]*)-.*?</div>[^<]*</div>[^<]*</div>[^<]*</div>)rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForSpoilLine(R"rgx(<span class="(?:bloc-spoil-jv en-ligne|message__spoil message__spoil--inline)">.*?<span class="(?:contenu-spoil|message__spoilContent)">(.*?)</span></span>)rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForSpoilBlock(R"rgx(<div class="(?:bloc-spoil-jv|message__spoil)">.*?<div class="(?:contenu-spoil|message__spoilContent)">(.*?)</div></div>)rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForCodeBlock(R"rgx(<pre class="(?:pre-jv|message__pre)"><code class="(?:code-jv|message__code)">([^<]*)</code></pre>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForCodeLine(R"rgx(<code class="(?:code-jv|message__code)">(.*?)</code>)rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForAllJVCare(R"rgx(<span class="JvCare [^"]*">([^<]*)</span>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForUnicodeInText(R"rgx(\\u([a-zA-Z0-9]{4}))rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForOverlyQuote(R"rgx(<(/)?blockquote>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForOverlySpoils(R"rgx((<(span|div) class="(?:bloc-spoil-jv|message__spoil)[^"]*">.*?<(span|div) class="(?:contenu-spoil|message__spoilContent)">|</span></span>|</div></div>))rgx", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    /* Les <p> et <blockquote> du nouveau rendu portent une classe que le reste du
     * traitement ne sait pas gérer, on la retire au pré-parsage. */
    const QRegularExpression expForParagraphWithClass(R"rgx(<p class="message__(?:p|noBlankline)"[^>]*>)rgx", configDependentVar::regexpBaseOptions);
    /* Une image noelshack postée seule est rendue en grand, sans balise <img> : c'est
     * un <span> vide avec l'URL dans data-src-background (suivie d'une accolade
     * parasite laissée par le gabarit de JVC). */
    const QRegularExpression expForNoelshackLarge(R"rgx(<span class="(JvCare [^"]*)"[^>]*><span class="message__urlImg[^"]*"[^>]*data-src-background="([^"]*?)"\}?"?[^>]*></span></span>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForNoelshackResizedPath(R"rgx(/fichiers-(?:xs|sm|md|lg)/)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForUglyImage(R"rgx(issou|risit|jesus|picsart|chancla)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAd(R"rgx(<ins[^>]*></ins>)rgx", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForWebsite(R"rgx(https?://([^/]*)/)rgx", configDependentVar::regexpBaseOptions);
    QString userAgentToUse = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:128.0) Gecko/20100101 Firefox/128.0";

    QString stringModificatorRemoveFirstsAndLastsPAndBr(QString baseMessage)
    {
        baseMessage = baseMessage.trimmed();
        while(baseMessage.startsWith("<p>") == true || baseMessage.startsWith("<br />") == true)
        {
            baseMessage.remove(0, baseMessage.indexOf(">") + 1);
            baseMessage = baseMessage.trimmed();
        }
        while(baseMessage.endsWith("</p>") == true || baseMessage.endsWith("<br />") == true)
        {
            baseMessage.remove(baseMessage.lastIndexOf("<"), baseMessage.size());
            baseMessage = baseMessage.trimmed();
        }
        return baseMessage;
    }

    QString stringModificatorMakeLinkIfPossible(QString baseMessage)
    {
        if((baseMessage.startsWith("http://") || baseMessage.startsWith("https://")) && baseMessage.contains(" ") == false)
        {
            baseMessage = "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"" + baseMessage + "\">" + baseMessage + "</a>";
        }
        return baseMessage;
    }

    QString stringModificatorUrlToStickerId(QString baseMessage)
    {
        if(baseMessage.endsWith("/"))
        {
            baseMessage.remove(baseMessage.size() - 1, 1);
        }
        if(baseMessage.contains("/"))
        {
            baseMessage.remove(0, baseMessage.lastIndexOf("/") + 1);
        }

        return baseMessage;
    }

    QString stringModificatorHideUglyImages(QString baseMessage)
    {
        QRegularExpressionMatch matcherForUglyImage = expForUglyImage.match(baseMessage);
        if(matcherForUglyImage.hasMatch() == true)
        {
            return "";
        }
        return baseMessage;
    }

    /* Ramène les images noelshack affichées en grand à la même forme que les
     * miniatures, pour que la suite du traitement n'ait qu'un seul cas à gérer. */
    void normalizeLargeNoelshackImages(QString& baseMessage)
    {
        QRegularExpressionMatch match = expForNoelshackLarge.match(baseMessage);

        while(match.hasMatch() == true)
        {
            QString linkOfImage = match.captured(2);
            QString linkOfFullSizeImage = QString(linkOfImage).replace(expForNoelshackResizedPath, "/fichiers/");

            QString replacement = "<span class=\"" + match.captured(1) + "\"><img class=\"message__urlImg\" src=\"" +
                                  linkOfImage + "\" alt=\"" + linkOfFullSizeImage + "\" /></span>";

            baseMessage.replace(match.capturedStart(0), match.capturedLength(0), replacement);
            match = expForNoelshackLarge.match(baseMessage, match.capturedStart(0) + replacement.size());
        }
    }

    QString stringModificatorMakeCodeBlockGreatAgain(QString baseMessage)
    {
        while(baseMessage.startsWith("\n") == true)
        {
            baseMessage.remove(0, 1);
        }
        while(baseMessage.endsWith("\n") == true)
        {
            baseMessage.remove(baseMessage.size() - 1, 1);
        }
        return baseMessage.replace("\n", "<br />");
    }
}

bool parsingTool::checkIfTopicAreSame(const QString& firstTopic, const QString& secondTopic)
{
    QRegularExpressionMatch matcherForFirstTopic = expForTopicLinkNumber.match(firstTopic);
    QRegularExpressionMatch matcherForSecondTopic = expForTopicLinkNumber.match(secondTopic);

    if(matcherForFirstTopic.hasMatch() == true && matcherForSecondTopic.hasMatch() == true)
    {
        // Bug apparu en 2024 : les topics pré-Respawn sont affichés avec une URL en mode 42 au lieu de 1 sur la liste des sujets
        if ((matcherForFirstTopic.captured("mode") == "1"  && matcherForSecondTopic.captured("mode") == "42")
         || (matcherForFirstTopic.captured("mode") == "42" && matcherForSecondTopic.captured("mode") == "1"))
        {
            return matcherForFirstTopic.captured("domain") == matcherForSecondTopic.captured("domain") &&
                   matcherForFirstTopic.captured("forumId") == matcherForSecondTopic.captured("forumId");
        }

        return matcherForFirstTopic.captured("domain") == matcherForSecondTopic.captured("domain") &&
               matcherForFirstTopic.captured("forumId") == matcherForSecondTopic.captured("forumId") &&
               matcherForFirstTopic.captured("topicId") == matcherForSecondTopic.captured("topicId");
    }
    else
    {
        return false;
    }
}

ajaxInfoStruct parsingTool::getAjaxInfo(const QString& source)
{
    ajaxInfoStruct newAjaxInfo;

    QString ajaxTimestamp = expForAjaxTimestamp.match(source).captured(1);
    QString ajaxHash = expForAjaxHash.match(source).captured(1);
    QString ajaxModTimestamp = expForAjaxModTimestamp.match(source).captured(1);
    QString ajaxModHash = expForAjaxModHash.match(source).captured(1);

    /* Les champs cachés ont disparu du HTML avec la refonte 2026 : le timestamp et
     * le hash sont maintenant dans le payload. */
    if(ajaxTimestamp.isEmpty() == true || ajaxHash.isEmpty() == true)
    {
        QJsonObject payload = payloadTool::getPayload(source);
        QString tokenFromPayload = payload.value("ajaxToken").toString();
        QString timestampFromPayload = QString::number(payload.value("formSession").toObject().value("fs_timestamp").toVariant().toLongLong());

        if(tokenFromPayload.isEmpty() == false)
        {
            ajaxTimestamp = timestampFromPayload;
            ajaxHash = tokenFromPayload;
        }

        if(ajaxModTimestamp.isEmpty() == true || ajaxModHash.isEmpty() == true)
        {
            QString modTokenFromPayload = payload.value("ajaxModerationToken").toString();

            if(modTokenFromPayload.isEmpty() == false)
            {
                ajaxModTimestamp = timestampFromPayload;
                ajaxModHash = modTokenFromPayload;
            }
        }
    }

    if(ajaxTimestamp.isEmpty() == false && ajaxHash.isEmpty() == false)
    {
        newAjaxInfo.list = "ajax_timestamp=" + ajaxTimestamp + "&ajax_hash=" + ajaxHash;
    }

    if(ajaxModTimestamp.isEmpty() == false && ajaxModHash.isEmpty() == false)
    {
        newAjaxInfo.mod = "ajax_timestamp=" + ajaxModTimestamp + "&ajax_hash=" + ajaxModHash;
    }

    return newAjaxInfo;
}

/* La citation ne passe plus par une requête au site : le texte source de chaque
 * message est déjà dans le payload de la page. On tronque au passage les citations
 * imbriquées trop profondes, sinon citer une citation de citation devient illisible. */
QString parsingTool::quoteThisRawMessage(QString rawMessage)
{
    const QString markerForTruncation = "\f[%<citation tronquee>%]\f";

    rawMessage.replace(expForDeeplyNestedQuote, markerForTruncation);
    rawMessage.replace(expForSuccessiveTruncations, "> > > > ''[citation imbriquée tronquée]''\n");
    rawMessage.replace("\n", "\n>");

    return rawMessage;
}

QString parsingTool::getWebsite(const QString& topicLink)
{
    return expForWebsite.match(topicLink).captured(1);
}

QString parsingTool::getVersionName(const QString& source)
{
    return expForVersionName.match(source).captured(1);
}

QString parsingTool::getVersionChangelog(const QString& source)
{
    QString changelog = expForVersionChangelog.match(source).captured(1).replace("\\n", "<br />").replace("\\r", "").replace("\\\"", "\"").replace(" -", "--").replace("   --", "---").replace("\\\\", "\\");
    replaceWithCapNumber(changelog, expForNormalLink, 0, "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 0, "</a>");
    changelog = changelog.trimmed();
    while(changelog.startsWith("<br />") == true)
    {
        changelog.remove(0, 6);
        changelog = changelog.trimmed();
    }
    while(changelog.endsWith("<br />") == true)
    {
        changelog.remove(changelog.size() - 6, 6);
        changelog = changelog.trimmed();
    }
    return changelog;
}

void parsingTool::getListOfHiddenInputFromThisForm(const QString& source, QString formName, QList<QPair<QString, QString>>& listOfInput)
{
    QString formSource;

    if(formName == "form-post-topic")
    {
        /* Le formulaire de réponse n'existe plus en HTML : ses champs cachés sont
         * la totalité de l'objet `formSession` du payload. */
        QJsonObject formSession = payloadTool::getPayload(source).value("formSession").toObject();

        if(formSession.isEmpty() == false)
        {
            for(auto ite = formSession.constBegin(); ite != formSession.constEnd(); ++ite)
            {
                listOfInput.push_back(QPair<QString, QString>(ite.key(), ite.value().toVariant().toString()));
            }

            /* Le formulaire d'envoi attend aussi le jeton ajax, qui lui n'est pas dans
             * formSession. */
            QString ajaxToken = payloadTool::getPayload(source).value("ajaxToken").toString();

            if(ajaxToken.isEmpty() == false)
            {
                listOfInput.push_back(QPair<QString, QString>("ajax_hash", ajaxToken));
            }

            qDebug(logParsing) << "formSession récupéré du payload," << listOfInput.size() << "champs.";
            return;
        }

        formSource = expForFormTopic.match(source).captured(1);
    }
    else if(formName == "form-connect-jv")
    {
        formSource = expForFormConnect.match(source).captured(1);
    }

    QRegularExpressionMatchIterator matchIteratorForInput = expForInput.globalMatch(formSource);
    while(matchIteratorForInput.hasNext())
    {
        QRegularExpressionMatch matchForInput = matchIteratorForInput.next();
        if(matchForInput.captured(1) == "type")
        {
            if(matchForInput.captured(3) == "name")
            {
                listOfInput.push_back(QPair<QString, QString>(matchForInput.captured(4), matchForInput.captured(6)));
            }
            else
            {
                listOfInput.push_back(QPair<QString, QString>(matchForInput.captured(6), matchForInput.captured(4)));
            }
        }
        else if(matchForInput.captured(3) == "type")
        {
            if(matchForInput.captured(1) == "name")
            {
                listOfInput.push_back(QPair<QString, QString>(matchForInput.captured(2), matchForInput.captured(6)));
            }
            else
            {
                listOfInput.push_back(QPair<QString, QString>(matchForInput.captured(6), matchForInput.captured(2)));
            }
        }
        else
        {
            if(matchForInput.captured(1) == "name")
            {
                listOfInput.push_back(QPair<QString, QString>(matchForInput.captured(2), matchForInput.captured(4)));
            }
            else
            {
                listOfInput.push_back(QPair<QString, QString>(matchForInput.captured(4), matchForInput.captured(2)));
            }
        }
    }
}

bool parsingTool::getTopicLocked(const QString& source)
{
    QJsonObject forumInPayload = payloadTool::getPayload(source).value("forum").toObject();

    if(forumInPayload.isEmpty() == false)
    {
        return forumInPayload.value("isForumReadOnly").toBool(false) == true ||
               forumInPayload.value("lockReason").toArray().isEmpty() == false;
    }

    return expForTopicLocked.match(source).hasMatch();
}

QString parsingTool::getErrorMessage(const QString& source, QString defaultError)
{
    QRegularExpressionMatch match = expForError.match(source);

    if(match.hasMatch() == true)
    {
        return match.captured(2);
    }
    else
    {
        return defaultError;
    }
}

QString parsingTool::getErrorMessageInJSON(const QString& source, bool needToParseAsAjaxMessage, QString defaultError)
{
    QRegularExpressionMatch match = expForErrorInJSON.match(source);

    if(match.hasMatch() == true)
    {
        QString tmpError = match.captured(1);

        if(needToParseAsAjaxMessage == true)
        {
            tmpError = parsingAjaxMessages(tmpError);
        }

        return specialCharToNormalChar(tmpError);
    }
    else
    {
        return defaultError;
    }
}

/* Depuis la refonte 2026, /forums/message/add et /forums/message/edit répondent en
 * JSON. Le succès ne se présente pas toujours de la même façon (objet vide, liste
 * d'erreurs vide, message rendu...), alors que l'échec, lui, est toujours signalé par
 * un champ d'erreur non vide : on considère donc que tout ce qui n'annonce pas
 * d'erreur est passé. C'est ce qui manquait pour ne plus crier à l'échec sur un
 * message correctement posté.
 *
 * Retourne une chaîne vide si l'envoi a réussi, sinon le message d'erreur à afficher. */
QString parsingTool::getErrorOfMessageSending(const QString& source, int httpStatus)
{
    /* Ancien comportement : une redirection signifiait que le message était passé. */
    if(source.isEmpty() == true || source.contains("<meta http-equiv=\"refresh\"") == true)
    {
        return "";
    }

    QJsonParseError errorOfParse;
    QJsonDocument document = QJsonDocument::fromJson(source.toUtf8(), &errorOfParse);

    if(document.isObject() == true)
    {
        QJsonObject answer = document.object();
        QStringList listOfErrors;

        for(const QString& nameOfField : {"erreur", "erreurs", "error", "errors"})
        {
            QJsonValue valueOfField = answer.value(nameOfField);

            if(valueOfField.isArray() == true)
            {
                for(const QJsonValue& thisError : valueOfField.toArray())
                {
                    if(thisError.toString().isEmpty() == false)
                    {
                        listOfErrors.append(thisError.toString());
                    }
                }
            }
            else if(valueOfField.isString() == true && valueOfField.toString().isEmpty() == false)
            {
                listOfErrors.append(valueOfField.toString());
            }
        }

        if(listOfErrors.isEmpty() == false)
        {
            return specialCharToNormalChar(parsingAjaxMessages(listOfErrors.join("\n"))).trimmed();
        }

        if(httpStatus >= 400)
        {
            return "Le message n'a pas été envoyé, le site a répondu " + QString::number(httpStatus) + ".";
        }

        return "";
    }

    /* Réponse en HTML : c'est l'ancien format, on cherche l'encadré d'erreur. */
    QString errorInHtml = getErrorMessage(source, "");

    if(errorInHtml.isEmpty() == false)
    {
        return errorInHtml;
    }

    if(httpStatus >= 400)
    {
        return "Le message n'a pas été envoyé, le site a répondu " + QString::number(httpStatus) + ".";
    }

    return "";
}

QString parsingTool::getNextPageOfTopic(const QString& source, const QString& website)
{
    pagerInfoStruct pagerInfo = getPagerInfo(source, website);

    if(pagerInfo.currentPage <= 0 || pagerInfo.currentPage >= pagerInfo.numberOfPages)
    {
        return "";
    }

    return buildLinkForThisPage(pagerInfo.linkOfAPage, pagerInfo.currentPage + 1);
}

QString parsingTool::getLastPageOfTopic(const QString& source, const QString& website)
{
    pagerInfoStruct pagerInfo = getPagerInfo(source, website);

    if(pagerInfo.currentPage <= 0 || pagerInfo.numberOfPages <= pagerInfo.currentPage)
    {
        return "";
    }

    return buildLinkForThisPage(pagerInfo.linkOfAPage, pagerInfo.numberOfPages);
}

/* La pagination n'est plus rendue en HTML : elle vient de `pagerView`, qui donne la
 * page courante, le nombre total de pages, et une liste *partielle* de liens (les
 * pages proches, puis de dix en dix). Sur un topic de 1300 pages, le lien de la
 * dernière page n'y est donc pas ; on reconstruit l'URL voulue à partir de
 * n'importe lequel des liens fournis. */
pagerInfoStruct parsingTool::getPagerInfo(const QString& source, const QString& website)
{
    pagerInfoStruct pagerInfo;
    QJsonObject pagerView = payloadTool::getPayload(source).value("pagerView").toObject();

    if(pagerView.isEmpty() == true)
    {
        qWarning(logParsing) << "Pas de pagerView dans le payload.";
        return pagerInfo;
    }

    pagerInfo.currentPage = pagerView.value("currentPage").toInt(0);
    pagerInfo.numberOfPages = pagerView.value("pageCount").toInt(0);

    QJsonArray pages = pagerView.value("pages").toArray();

    for(const QJsonValue& thisPage : pages)
    {
        QString urlOfPage = thisPage.toObject().value("url").toString();

        if(urlOfPage.isEmpty() == false)
        {
            if(urlOfPage.startsWith("http") == false)
            {
                urlOfPage = "https://" + website + urlOfPage;
            }

            pagerInfo.linkOfAPage = urlOfPage;
            break;
        }
    }

    return pagerInfo;
}

QString parsingTool::buildLinkForThisPage(const QString& linkOfAnyPage, int wantedPage)
{
    QRegularExpressionMatch matchForLink = expForTopicLinkNumber.match(linkOfAnyPage);

    if(matchForLink.hasMatch() == false)
    {
        return "";
    }

    return matchForLink.captured("beforeTopicPage") + QString::number(wantedPage) + matchForLink.captured("afterTopicPage");
}

QString parsingTool::getFirstPageOfTopic(const QString& topicLink)
{
    QRegularExpressionMatch matchForFirstPage = expForTopicLinkNumber.match(topicLink);

    if(matchForFirstPage.hasMatch() == true)
    {
        return matchForFirstPage.captured("beforeTopicPage") + "1" + matchForFirstPage.captured("afterTopicPage");
    }
    else
    {
        return "";
    }
}

QString parsingTool::getBeforeLastPageOfTopic(const QString& topicLink)
{
    QRegularExpressionMatch matchForBeforeLastPage = expForTopicLinkNumber.match(topicLink);
    QString pageNumber = matchForBeforeLastPage.captured("topicPage");

    if(pageNumber.isEmpty() == false && pageNumber != "1")
    {
        return matchForBeforeLastPage.captured("beforeTopicPage") + QString::number(pageNumber.toInt() - 1) + matchForBeforeLastPage.captured("afterTopicPage");
    }
    else
    {
        return "";
    }
}

QString parsingTool::getNameOfTopic(const QString& source)
{
    /* `topicTitleField` est déjà décodé, contrairement à l'ancien HTML. */
    return specialCharToNormalChar(payloadTool::getPayload(source).value("topicTitleField").toString());
}

QString parsingTool::getNumberOfConnected(const QString& source)
{
    QJsonObject header = payloadTool::getPayload(source).value("forumInfo").toObject().value("header").toObject();

    if(header.contains("btnVal") == false)
    {
        return "";
    }

    return header.value("btnVal").toVariant().toString().trimmed();
}

int parsingTool::getNumberOfMp(const QString& source)
{
    QRegularExpressionMatch matchMpNumber = expForMpJvc.match(source);

    if(matchMpNumber.hasMatch() == true)
    {
        return matchMpNumber.captured(1).trimmed().toInt();
    }
    else
    {
        return 0;
    }
}

QList<messageStruct> parsingTool::getListOfEntireMessagesWithoutMessagePars(const QString& source)
{
    QList<messageStruct> listOfMessages;
    long lastIdOfMessage = 0;
    bool sortNeeded = false;

    QJsonObject payload = payloadTool::getPayload(source);
    QJsonArray listMessage = payload.value("listMessage").toArray();

    if(listMessage.isEmpty() == true)
    {
        if(payload.isEmpty() == true)
        {
            qWarning(logParsing) << "Aucun message : le payload de la page est absent ou illisible.";
            QString pathOfDump = logTool::dumpFailedPage("sans-payload", source);
            if(pathOfDump.isEmpty() == false)
            {
                qWarning(logParsing) << "Page sauvegardée dans" << pathOfDump;
            }
        }
        else if(payload.contains("listMessage") == false)
        {
            /* Une page de forum ou une page d'erreur, pas une page de topic. */
            qWarning(logParsing) << "Aucun message : le payload ne contient pas de listMessage. Clés :" << payload.keys();
        }

        return listOfMessages;
    }

    /* Les URL d'action sont relatives ; le domaine se déduit d'une des URL absolues du
     * payload, pour rester correct sur les autres domaines que jeuxvideo.com. */
    QString websiteOfPayload = getWebsite(payload.value("listMessageUrlUpdate").toString());

    if(websiteOfPayload.isEmpty() == true)
    {
        websiteOfPayload = getWebsite(listMessage.first().toObject().value("publishedAuthorProfileUrl").toString());
    }

    for(const QJsonValue& thisMessageValue : listMessage)
    {
        QJsonObject thisMessage = thisMessageValue.toObject();

        listOfMessages.push_back(messageStruct());
        messageStruct& newMessage = listOfMessages.back();

        newMessage.idOfMessage = static_cast<long>(thisMessage.value("id").toVariant().toLongLong());
        newMessage.avatarLink = removeSchemeOfUrl(thisMessage.value("publishedAuthorAvatar").toString());
        newMessage.pseudoInfo.pseudoName = thisMessage.value("publishedAuthorName").toString();
        newMessage.pseudoInfo.pseudoType = roleToPseudoType(thisMessage.value("publishedAuthorRole").toString());
        newMessage.wholeDate = thisMessage.value("publishedDate").toString();
        newMessage.date = getHourOfDate(newMessage.wholeDate);
        newMessage.message = makeBasicPreParseOfMessage(thisMessage.value("renderedText").toString());
        newMessage.messageRaw = thisMessage.value("text").toString().replace("\r\n", "\n");

        QJsonObject actionsOfMessage = thisMessage.value("actions").toObject();
        newMessage.editUrl = makeAbsoluteUrl(actionsOfMessage.value("edit").toObject().value("url").toString(), websiteOfPayload);
        newMessage.deleteUrl = makeAbsoluteUrl(actionsOfMessage.value("delete").toObject().value("url").toString(), websiteOfPayload);
        newMessage.signature = thisMessage.value("publishedAuthorSignatureRendered").toString();

        /* `updatedDate` est renseigné même quand le message n'a jamais été édité sur
         * certains messages ; c'est `updatedText` qui fait foi. */
        if(thisMessage.value("updatedText").toString().isEmpty() == false)
        {
            newMessage.lastTimeEdit = thisMessage.value("updatedDate").toString();
            newMessage.lastTimeEditHourOnly = getHourOfDate(newMessage.lastTimeEdit);
        }

        if(newMessage.pseudoInfo.pseudoName.isEmpty() == true)
        {
            newMessage.pseudoInfo.pseudoName = "Pseudo supprimé";
        }

        if(lastIdOfMessage > newMessage.idOfMessage)
        {
            sortNeeded = true;
        }
        lastIdOfMessage = newMessage.idOfMessage;
    }

    if(sortNeeded == true)
    {
        std::sort(listOfMessages.begin(), listOfMessages.end());
    }

    qDebug(logParsing) << listOfMessages.size() << "messages extraits du payload.";

    return listOfMessages;
}

QList<topicStruct> parsingTool::getListOfTopic(const QString& source, const QString& website)
{
    QList<topicStruct> listOfTopic;

    QJsonObject payload = payloadTool::getPayload(source);
    QJsonArray listTopics = payload.value("listTopics").toArray();

    if(listTopics.isEmpty() == true)
    {
        if(payload.isEmpty() == true)
        {
            qWarning(logForum) << "Aucun sujet : le payload de la page est absent ou illisible.";
            QString pathOfDump = logTool::dumpFailedPage("forum-sans-payload", source);
            if(pathOfDump.isEmpty() == false)
            {
                qWarning(logForum) << "Page sauvegardée dans" << pathOfDump;
            }
        }
        else
        {
            qWarning(logForum) << "Aucun sujet dans le payload. Clés :" << payload.keys();
        }

        return listOfTopic;
    }

    for(const QJsonValue& thisTopicValue : listTopics)
    {
        QJsonObject thisTopic = thisTopicValue.toObject();
        QString link = thisTopic.value("url").toString();

        if(link.isEmpty() == true)
        {
            continue;
        }

        if(link.startsWith("http") == false)
        {
            link = "https://" + website + link;
        }

        listOfTopic.append(topicStruct());
        listOfTopic.back().name = specialCharToNormalChar(thisTopic.value("title").toString());
        listOfTopic.back().link = link;
        listOfTopic.back().pseudoInfo.pseudoName = thisTopic.value("author").toObject().value("pseudo").toString();
        listOfTopic.back().pseudoInfo.pseudoType = roleToPseudoType(thisTopic.value("author").toObject().value("role").toString());
        listOfTopic.back().topicType = stateOfTopicToTopicType(thisTopic.value("stateTopic").toString(), thisTopic.value("stateIcon").toString());

        /* `responsesCount` est un nombre, sauf pour les modérateurs qui voient
         * « visibles / total ». */
        QString numberOfMessage = thisTopic.value("responsesCount").toVariant().toString();

        if(numberOfMessage.contains("/") == true)
        {
            numberOfMessage = numberOfMessage.section("/", -1).trimmed();
        }

        listOfTopic.back().numberOfMessage = numberOfMessage;
    }

    qDebug(logForum) << listOfTopic.size() << "sujets extraits du payload.";

    return listOfTopic;
}

/* JVC renvoie un rôle vide pour les utilisateurs normaux, et des noms longs pour les
 * autres, alors que le reste du programme raisonne en user/modo/admin/staff. */
QString parsingTool::roleToPseudoType(const QString& role)
{
    if(role == "moderator" || role == "modo")
    {
        return "modo";
    }
    else if(role == "administrator" || role == "admin")
    {
        return "admin";
    }
    else if(role == "staff")
    {
        return "staff";
    }

    return "user";
}

QString parsingTool::stateOfTopicToTopicType(const QString& stateTopic, const QString& stateIcon)
{
    if(stateTopic == "msg-deleted")
    {
        return "topic-removed";
    }

    if(stateIcon == "pinned-on")
    {
        return "topic-pin-on";
    }
    else if(stateIcon == "pinned-off")
    {
        return "topic-pin-off";
    }
    else if(stateIcon == "lock" || stateIcon == "locked")
    {
        return "topic-lock";
    }
    else if(stateIcon == "resolved")
    {
        return "topic-resolved";
    }
    else if(stateIcon == "hot")
    {
        return "topic-folder2";
    }

    return "topic-folder";
}

QString parsingTool::makeAbsoluteUrl(const QString& url, const QString& website)
{
    if(url.isEmpty() == true || url.startsWith("http") == true)
    {
        return url;
    }

    if(website.isEmpty() == true)
    {
        return "https://www.jeuxvideo.com" + url;
    }

    return "https://" + website + url;
}

QString parsingTool::removeSchemeOfUrl(const QString& url)
{
    int positionOfScheme = url.indexOf("//");

    if(positionOfScheme == -1)
    {
        return url;
    }

    return url.mid(positionOfScheme + 2);
}

/* Les dates de JVC sont de la forme « 02 juin 2026 à 22:25:06 ». */
QString parsingTool::getHourOfDate(const QString& wholeDate)
{
    int positionOfHour = wholeDate.lastIndexOf(" à ");

    if(positionOfHour == -1)
    {
        return "";
    }

    return wholeDate.mid(positionOfHour + 3).trimmed();
}

QString parsingTool::getForumIdOfThisTopic(const QString& topicLink)
{
    return expForTopicLinkNumber.match(topicLink).captured("forumId");
}

QString parsingTool::getTopicIdOfThisTopic(const QString& topicLink)
{
    return expForTopicLinkNumber.match(topicLink).captured("topicId");
}

/* Depuis la refonte 2026, l'envoi d'un message passe par un formulaire multipart et
 * non plus par un simple corps urlencodé. */
QByteArray parsingTool::buildMultipartFormData(const QList<QPair<QString, QString>>& listOfField, QByteArray& boundaryUsed)
{
    boundaryUsed = "------respawnircformboundary" + QByteArray::number(QRandomGenerator::global()->generate64(), 16);

    QByteArray body;

    for(const QPair<QString, QString>& thisField : listOfField)
    {
        body += "--" + boundaryUsed + "\r\n";
        body += "Content-Disposition: form-data; name=\"" + thisField.first.toUtf8() + "\"\r\n\r\n";
        body += thisField.second.toUtf8() + "\r\n";
    }

    body += "--" + boundaryUsed + "--\r\n";

    return body;
}

QString parsingTool::getForumOfTopic(const QString& topicLink)
{
    QRegularExpressionMatch infosMatcher = expForTopicLinkNumber.match(topicLink);

    if(infosMatcher.hasMatch() == true)
    {
        return "https://" + infosMatcher.captured("domain") + "/forums/0-" + infosMatcher.captured("forumId") + "-0-1-0-1-0-respawn-irc.htm";
    }
    else
    {
        return "";
    }
}

QString parsingTool::getForumName(const QString& source)
{
    /* Le fil d'Ariane est indenté dans la page, d'où le trimmed(). */
    return specialCharToNormalChar(expForForumName.match(source).captured(1).trimmed());
}

QString parsingTool::jvfLinkToJvcLink(const QString& jvfTopicLink)
{
    QRegularExpressionMatch matchForJvfLink = expForJvfLink.match(jvfTopicLink);
    QString forumNumber = matchForJvfLink.captured(1);
    QString topicNumber = matchForJvfLink.captured(2);
    QString nameOfTopic = matchForJvfLink.captured(3);

    if(forumNumber.isEmpty() == false && topicNumber.isEmpty() == false)
    {
        QString tempo = "42";

        if(topicNumber.startsWith("0") == true)
        {
            tempo = "1";
            topicNumber.remove(0, 1);
        }

        return "https://www.jeuxvideo.com/forums/" + tempo + "-" + forumNumber + "-" + topicNumber + "-1-0-1-0-" + nameOfTopic + ".htm";
    }
    else
    {
        return "";
    }
}

QString parsingTool::normalAvatarLinkToHDLink(const QString& avatarLink)
{
    QString newAvatarLink = avatarLink;
    int sizePos = avatarLink.indexOf("-sm");

    if(sizePos != -1)
    {
        newAvatarLink.replace(sizePos, 3, "-md");
    }

    return newAvatarLink;
}

QString parsingTool::parsingMessages(QString thisMessage, infoForMessageParsingStruct infoForParsing, bool reallyDownloadStickers)
{
    QString extraTableStyle;

    if(infoForParsing.betterQuote == true)
    {
        extraTableStyle += "background: " + styleTool::getColorInfo().quoteBackgroundColor + ";color: " + styleTool::getColorInfo().quoteTextColor + ";";
    }

    if(infoForParsing.betterCodeTag == true)
    {
        replaceWithCapNumber(thisMessage, expForCodeBlock, 1, "<table border=\"0\" cellspacing=\"0\" cellpadding=\"5\" style=\"margin-top: 5px;margin-bottom: 5px;background-color: " + styleTool::getColorInfo().codeTagBackgroundColor + ";\"><tr><td><code style=\"white-space: pre-wrap\">", "</code></td></tr></table>", -1, "", std::bind(stringModificatorMakeCodeBlockGreatAgain, std::placeholders::_1));
        replaceWithCapNumber(thisMessage, expForCodeLine, 1, "<span style=\"background-color: " + styleTool::getColorInfo().codeTagBackgroundColor + ";\">" + QString(QChar::Nbsp) + "<code style=\"white-space: pre-wrap\">", "</code>" + QString(QChar::Nbsp) + "</span>", -1, "");
    }
    else
    {
        replaceWithCapNumber(thisMessage, expForCodeBlock, 1, "<p><code style=\"white-space: pre-wrap\">", "</code></p>", -1, "", std::bind(stringModificatorMakeCodeBlockGreatAgain, std::placeholders::_1));
        replaceWithCapNumber(thisMessage, expForCodeLine, 1, " <code style=\"white-space: pre-wrap\">", "</code> ", -1, "");
    }

    thisMessage.replace("\n", "");

    shortcutTool::transformMessage(&thisMessage, "noLangageSticker");
    if(infoForParsing.stickerToSmiley == true)
    {
        shortcutTool::transformMessage(&thisMessage, "stickerToSmiley");
    }

    if(infoForParsing.showStickers == false)
    {
        replaceWithCapNumber(thisMessage, expForStickers, 1, "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    }
    else
    {
        if(infoForParsing.listOfStickersUsed != nullptr && reallyDownloadStickers == true)
        {
            infoForParsing.listOfStickersUsed->append(getListOfThisCapNumber(thisMessage, expForStickers, 1, false));
        }

        replaceWithCapNumber(thisMessage, expForStickers, 1, "<img width=" + QString::number(infoForParsing.stickersSize) + " height=" + QString::number(infoForParsing.stickersSize) + " src=\"resources/stickers/", ".png\" />", -1, "", std::bind(stringModificatorUrlToStickerId, std::placeholders::_1));
    }

    if(infoForParsing.smileyToText == true)
    {
        replaceWithCapNumber(thisMessage, expForSmiley, 3);
    }
    else
    {
        replaceWithCapNumber(thisMessage, expForSmiley, 2, "<img src=\"resources/smileys/", "\" />");
    }

    replaceWithCapNumber(thisMessage, expForEmbedVideo, 1, "<p><a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a></p>");
    replaceWithCapNumber(thisMessage, expForJvcVideo, 1, "<p><a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"https://www.jeuxvideo.com/videos/iframe/", "\">https://www.jeuxvideo.com/videos/iframe/", 1, "</a></p>");
    replaceWithCapNumber(thisMessage, expForJvcLink, 2, "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 2, "</a>");
    replaceWithCapNumber(thisMessage, expForAlloCineLink, 1, "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForShortLink, 1, "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForLongLink, 1, "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");

    if(infoForParsing.hideUglyImages == true)
    {
        replaceWithCapNumber(thisMessage, expForNoelshack, 0, "", "", -1, "", std::bind(stringModificatorHideUglyImages, std::placeholders::_1));
    }

    if(infoForParsing.listOfNoelshackImageUsed != nullptr)
    {
        infoForParsing.listOfNoelshackImageUsed->append(getListOfThisCapNumber(thisMessage, expForNoelshack, 2, false));

        for(QString& thisNoelshackImage : *infoForParsing.listOfNoelshackImageUsed)
        {
            if(thisNoelshackImage.startsWith("http://") == false && thisNoelshackImage.startsWith("https://") == false)
            {
                thisNoelshackImage = "https://" + thisNoelshackImage;
            }
        }

        replaceWithCapNumber(thisMessage, expForNoelshack, 3, "<a href=\"", "\"><img width=" + QString::number(infoForParsing.noelshackImageWidth) + " height=" + QString::number(infoForParsing.noelshackImageHeight) + " src=\"img/", 2, "\" /></a>");
    }
    else
    {
        replaceWithCapNumber(thisMessage, expForNoelshack, 3, "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"", "\">", 3, "</a>");
    }

    replaceWithCapNumber(thisMessage, expForSpoilLine, 1, "<span style=\"color: " + styleTool::getColorInfo().spoilColor + "; background-color: " + styleTool::getColorInfo().spoilColor + ";\">", "</span>", -1, "", std::bind(stringModificatorRemoveFirstsAndLastsPAndBr, std::placeholders::_1));
    replaceWithCapNumber(thisMessage, expForSpoilBlock, 1, "<p><span style=\"color: " + styleTool::getColorInfo().spoilColor + "; background-color: " + styleTool::getColorInfo().spoilColor + ";\">", "</span></p>", -1, "", std::bind(stringModificatorRemoveFirstsAndLastsPAndBr, std::placeholders::_1));
    replaceWithCapNumber(thisMessage, expForAllJVCare, 1, "", "", -1, "", std::bind(stringModificatorMakeLinkIfPossible, std::placeholders::_1));

    removeAllOverlyQuote(thisMessage, infoForParsing.nbMaxQuote);

    thisMessage.replace(QRegularExpression(R"rgx((<br /> *){0,2}</p> *<p>( *<br />){0,2})rgx"), "<br /><br />");
    thisMessage.replace(QRegularExpression(R"rgx(<br /> *<(/)?p> *<br />)rgx"), "<br /><br />");
    thisMessage.replace(QRegularExpression(R"rgx((<br /> *){1,2}<(/)?p>)rgx"), "<br /><br />");
    thisMessage.replace(QRegularExpression(R"rgx(<(/)?p>(<br /> *){1,2})rgx"), "<br /><br />");
    thisMessage.replace(QRegularExpression(R"rgx(<(/)?p>)rgx"), "<br /><br />");
    thisMessage.replace(QRegularExpression(R"rgx((<br /> *)*(<(/)?blockquote>)( *<br />)*)rgx"), "\\2");

    if(infoForParsing.betterCodeTag == true)
    {
        thisMessage.replace(QRegularExpression(R"rgx((<br /> *)*(<(/)?table[^>]*>)( *<br />)*)rgx"), "\\2");
    }

    thisMessage.replace("<blockquote>", "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\" style=\"margin-top: 5px;margin-bottom: 5px;border-color: " + styleTool::getColorInfo().quoteBorderColor + ";" + extraTableStyle + "\"><tr><td>");
    thisMessage.replace("</blockquote>", "</td></tr></table>");

    thisMessage.remove("</div>");
    thisMessage = thisMessage.trimmed();
    while(thisMessage.startsWith("<br />") == true)
    {
        thisMessage.remove(0, 6);
        thisMessage = thisMessage.trimmed();
    }
    while(thisMessage.endsWith("<br />") == true)
    {
        thisMessage.remove(thisMessage.size() - 6, 6);
        thisMessage = thisMessage.trimmed();
    }

    return thisMessage;
}

QString parsingTool::makeBasicPreParseOfMessage(QString thisMessage)
{
    thisMessage.remove(expForAd);
    thisMessage.replace("\r", "");

    removeAllOverlySpoils(thisMessage);
    normalizeLargeNoelshackImages(thisMessage);
    thisMessage.replace("<blockquote class=\"blockquote-jv\">", "<blockquote>");
    thisMessage.replace("<blockquote class=\"message__blockquote\">", "<blockquote>");
    /* Le reste du traitement ne reconnaît que des <p> nus. */
    thisMessage.replace(expForParagraphWithClass, "<p>");

    if(thisMessage.contains("<li>") == true)
    {
        thisMessage.replace(QRegularExpression(R"rgx(<(ul|ol)[^>]*>)rgx"), "<p>");
        thisMessage.replace("</ul>", "</p>");
        thisMessage.replace("</ol>", "</p>");
        thisMessage.replace("<li><p><li>", "<li><li>");
        thisMessage.replace("<li><p><li>", "<li><li>");
        thisMessage.replace("<li>", " • ");
        thisMessage.replace("</li></p></li>", "</li>");
        thisMessage.replace("</li></p></li>", "</li>");
        thisMessage.replace("</li>", "<br />");
    }

    return thisMessage;
}

QString parsingTool::parsingAjaxMessages(QString thisMessage)
{
    thisMessage.remove("\n");
    thisMessage.remove("\r");
    thisMessage.replace(QRegularExpression(R"rgx((?<!\\)\\r)rgx"), "");
    thisMessage.replace(QRegularExpression(R"rgx((?<!\\)\\")rgx"), "\"");
    thisMessage.replace(QRegularExpression(R"rgx((?<!\\)\\/)rgx"), "/");
    thisMessage.replace(QRegularExpression(R"rgx((?<!\\)\\n)rgx"), "\n");
    thisMessage.replace(QRegularExpression(R"rgx((?<!\\)\\t)rgx"), "\t");
    thisMessage.replace("\\\\", "\\");

    QRegularExpressionMatchIterator matchIterator = expForUnicodeInText.globalMatch(thisMessage);
    int lenghtChanged = 0;
    while(matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();

        thisMessage.replace(match.capturedStart(0) + lenghtChanged, match.capturedLength(0), QChar(match.captured(1).toUpper().toUInt(nullptr, 16)));
        lenghtChanged -= match.capturedLength(0);
        lenghtChanged += 1;
    }

    return thisMessage;
}

QString parsingTool::specialCharToNormalChar(QString thisMessage)
{
    return thisMessage.replace("&amp;", "&").replace("&quot;", "\"").replace("&#039;", "\'").replace("&lt;", "<").replace("&gt;", ">");
}

QNetworkRequest parsingTool::buildRequestWithThisUrl(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // HTTP/2 apaise les challenges Cloudflare
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);

    request.setRawHeader("User-Agent", userAgentToUse.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Cookie", "");
    request.setRawHeader("Connection", "Keep-Alive");

    return request;
}

QList<QString> parsingTool::getListOfThisCapNumber(const QString& source, const QRegularExpression& exp, int capNumber, bool globalMatch)
{
    QList<QString> listOfString;
    if(globalMatch == true)
    {
        QRegularExpressionMatchIterator matchIterator = exp.globalMatch(source);
        while(matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            listOfString.push_back(match.captured(capNumber));
        }
    }
    else
    {
        QRegularExpressionMatch match = exp.match(source);
        while(match.hasMatch() == true)
        {
            listOfString.push_back(match.captured(capNumber));
            match = exp.match(source, match.capturedEnd(capNumber));
        }
    }

    return listOfString;
}

void parsingTool::removeAllOverlyQuote(QString& source, int maxNumberQuote)
{
    QRegularExpressionMatch match = expForOverlyQuote.match(source);
    ++maxNumberQuote;
    while(match.hasMatch() == true)
    {
        if(match.captured() == "<blockquote>")
        {
            --maxNumberQuote;
        }
        else if(match.captured() == "</blockquote>")
        {
            ++maxNumberQuote;
        }

        if(maxNumberQuote <= 0)
        {
            int tmpNumberQuote = 0;
            QRegularExpressionMatch secMatch = expForOverlyQuote.match(source, match.capturedEnd());
            while(secMatch.hasMatch() == true)
            {
                if(secMatch.captured() == "<blockquote>")
                {
                    ++tmpNumberQuote;
                }
                else if(secMatch.captured() == "</blockquote>")
                {
                    --tmpNumberQuote;
                }

                if(tmpNumberQuote < 0)
                {
                    break;
                }

                secMatch = expForOverlyQuote.match(source, secMatch.capturedEnd());
            }

            if(secMatch.capturedStart() != -1)
            {
                source.replace(match.capturedEnd(), (secMatch.capturedStart() - match.capturedEnd()), "[...]");
            }
        }

        match = expForOverlyQuote.match(source, match.capturedEnd());
    }
}

void parsingTool::removeAllOverlySpoils(QString& source)
{
    QRegularExpressionMatch spoilOverlyMatcher = expForOverlySpoils.match(source);
    int currentSpoilTagDeepness = 0;
    int lastOffsetOfTag = 0;

    while(spoilOverlyMatcher.hasMatch() == true)
    {
        bool itsEndingTag = spoilOverlyMatcher.captured().startsWith("</");

        if(itsEndingTag == false)
        {
            ++currentSpoilTagDeepness;
        }

        if(currentSpoilTagDeepness > 1)
        {
            lastOffsetOfTag = spoilOverlyMatcher.capturedStart();
            source.remove(spoilOverlyMatcher.capturedStart(), spoilOverlyMatcher.capturedLength());
        }
        else
        {
            lastOffsetOfTag = spoilOverlyMatcher.capturedEnd();
        }

        if(itsEndingTag == true)
        {
            --currentSpoilTagDeepness;

            if(currentSpoilTagDeepness < 0)
            {
                currentSpoilTagDeepness = 0;
            }
        }

        spoilOverlyMatcher = expForOverlySpoils.match(source, lastOffsetOfTag);
    }
}

void parsingTool::replaceWithCapNumber(QString& source, const QRegularExpression& exp, int capNumber, QString stringBefore, QString stringAfter, int secondCapNumber,
                                            QString stringAfterAfter, std::function<QString(QString)> capModificator)
{
    QRegularExpressionMatch match = exp.match(source);
    QString newString;

    while(match.hasMatch())
    {
        newString = stringBefore;
        if(capModificator != nullptr)
        {
            newString += capModificator(match.captured(capNumber));
        }
        else
        {
            newString += match.captured(capNumber);
        }
        newString += stringAfter;

        if(secondCapNumber != -1)
        {
            if(capModificator != nullptr)
            {
                newString += capModificator(match.captured(secondCapNumber));
            }
            else
            {
                newString += match.captured(secondCapNumber);
            }
            newString += stringAfterAfter;
        }

        source.replace(match.capturedStart(0), match.capturedLength(0), newString);
        match = exp.match(source, match.capturedStart(0) + newString.size());
    }
}
