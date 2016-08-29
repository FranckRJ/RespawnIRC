#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QUrl>

#include "parsingTool.hpp"
#include "styleTool.hpp"
#include "shortcutTool.hpp"

namespace
{
    const QRegularExpression expForNormalLink("http(s)?://[^\\(\\)\\]\\[ \\\\<>]*", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForAjaxTimestamp("<input type=\"hidden\" name=\"ajax_timestamp_liste_messages\" id=\"ajax_timestamp_liste_messages\" value=\"([^\"]*)\" />", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForAjaxHash("<input type=\"hidden\" name=\"ajax_hash_liste_messages\" id=\"ajax_hash_liste_messages\" value=\"([^\"]*)\" />", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForAjaxModTimestamp("<input type=\"hidden\" name=\"ajax_timestamp_moderation_forum\" id=\"ajax_timestamp_moderation_forum\" value=\"([^\"]*)\" />", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForAjaxModHash("<input type=\"hidden\" name=\"ajax_hash_moderation_forum\" id=\"ajax_hash_moderation_forum\" value=\"([^\"]*)\" />", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForMessageEdit("<textarea tabindex=\"3\" class=\"area-editor\" name=\"text_commentaire\" id=\"text_commentaire\" placeholder=\"[^\"]*\">([^<]*)</textarea>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForMessageQuote("\"txt\":\"(.*)\"}", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForVersionName("\"tag_name\"[^\"]*:[^\"]*\"([^\"]*)\"", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForVersionChangelog("\"body\"[^\"]*:[^\"]*\"(.*)\"", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForFormTopic("(<form role=\"form\" class=\"form-post-topic[^\"]*\" method=\"post\" action=\"\".*?>.*?</form>)", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForFormConnect("(<form role=\"form\" class=\"form-connect-jv\" method=\"post\" action=\"\".*?>.*?</form>)", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForInput("<input ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\"/>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForTopicLocked("<div class=\"message-lock-topic\">", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForCaptcha("<img src=\"([^\"]*)\" alt=[^>]*>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForError("<div class=\"alert-row\">([^<]*)</div>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForCurrentPage("<span class=\"page-active\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForPageLink("<span><a href=\"([^\"]*)\" class=\"lien-jv\">([^<]*)</a></span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForBeforeLastPage("(http://www.jeuxvideo.com/forums/[^-]*-[^-]*-[^-]*-)([^-]*)(-[^-]*-[^-]*-[^-]*-[^.]*.htm)", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForNameOfTopic("<span id=\"bloc-title-forum\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForNumberOfConnected("<span class=\"nb-connect-fofo\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForMpJvc("<span[^c]*class=\"account-number-mp[^\"]*\".*?data-val=\"([^\"]*)\"", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForEntireMessage("(<div class=\"bloc-message-forum \".*?)(<span id=\"post_[^\"]*\" class=\"bloc-message-forum-anchor\">|<div class=\"bloc-pagi-default\">)", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForEntireTopic("<li class=\"\" data-id=\"[^\"]*\">[^<]*<span class=\"topic-subject\">.*?</li>", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForTopicNameAndLink("<a class=\"lien-jv topic-title\" href=\"([^\"]*\" title=\"[^\"]*)\"[^>]*>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForTopicNumberMessage("<span class=\"topic-count\">[^0-9]*([0-9]*)", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForMessageID("<div class=\"bloc-message-forum \" data-id=\"([^\"]*)\">", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForPseudo("<span class=\"JvCare [^ ]* bloc-pseudo-msg text-([^\"]*)\" target=\"_blank\">[^a-zA-Z0-9_\\[\\]-]*([a-zA-Z0-9_\\[\\]-]*)[^<]*</span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForDate("<div class=\"bloc-date-msg\">([^<]*<span class=\"JvCare [^ ]* lien-jv\" target=\"_blank\">)?[^a-zA-Z0-9]*([^ ]* [^ ]* [^ ]* [^ ]* ([0-9:]*))", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForMessage("<div class=\"bloc-contenu\"><div class=\"txt-msg  text-[^-]*-forum \">((.*?)(?=<div class=\"info-edition-msg\">)|(.*?)(?=<div class=\"signature-msg)|(.*))", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForEdit("<div class=\"info-edition-msg\">Message édité le ([^ ]* [^ ]* [^ ]* [^ ]* [0-9:]*) par <span", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForForum("http://www.jeuxvideo.com/forums/[^-]*-([^-]*)-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^.]*.htm", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForForumName("<title>(.*?)- jeuxvideo.com</title>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForJvfLink("http://jvforum.fr/([^/]*)/([^-]*)-([^/]*)", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForSmiley("<img src=\"//image.jeuxvideo.com/smileys_img/([^\"]*)\" alt=\"[^\"]*\" data-def=\"SMILEYS\" data-code=\"([^\"]*)\" title=\"[^\"]*\" />", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForStickers("<img class=\"img-stickers\" src=\"(http://jv.stkr.fr/p/([^\"]*))\"/>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForLongLink("<span class=\"JvCare [^\"]*\"[^i]*itle=\"([^\"]*)\">[^<]*<i></i><span>[^<]*</span>[^<]*</span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForShortLink("<span class=\"JvCare [^\"]*\" rel=\"nofollow\" target=\"_blank\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForJvcLink("<a href=\"([^\"]*)\"( title=\"[^\"]*\")?>.*?</a>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForNoelshack("<a href=\"([^\"]*)\" data-def=\"NOELSHACK\" target=\"_blank\"><img class=\"img-shack\" .*? src=\"//([^\"]*)\" [^>]*></a>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForYoutubeVideo("<div class=\"player-contenu\"><div class=\"[^\"]*\"><iframe .*? src=\"http(s)?://www.youtube.com/embed/([^\"]*)\"[^>]*></iframe></div></div>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForSpoilLine("<span class=\"bloc-spoil-jv en-ligne\">.*?<span class=\"contenu-spoil\">(.*?)</span></span>", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForSpoilBlock("<span class=\"bloc-spoil-jv\">.*?<span class=\"contenu-spoil\">(.*?)</span></span>", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForCodeLine("<code class=\"code-jv\">([^<]*)</code>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForCodeBlock("<pre class=\"pre-jv\"><code class=\"code-jv\">([^<]*)</code></pre>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForAllJVCare("<span class=\"JvCare [^\"]*\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForUnicodeInText("\\\\u([a-zA-Z0-9]{4})", QRegularExpression::OptimizeOnFirstUsageOption);
    const QRegularExpression expForHtmlTag("<.+?>", QRegularExpression::OptimizeOnFirstUsageOption);
}

ajaxInfoStruct parsingToolClass::getAjaxInfo(const QString& source)
{
    ajaxInfoStruct newAjaxInfo;

    QString ajaxTimestamp = expForAjaxTimestamp.match(source).captured(1);
    QString ajaxHash = expForAjaxHash.match(source).captured(1);
    QString ajaxModTimestamp = expForAjaxModTimestamp.match(source).captured(1);
    QString ajaxModHash = expForAjaxModHash.match(source).captured(1);

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

QString parsingToolClass::getMessageEditAndChangeSource(QString& source)
{
    source.remove("\n");
    source.replace("\\\"", "\"");
    source.replace("\\/", "/");

    return parsingAjaxMessages(expForMessageEdit.match(source).captured(1));
}

QString parsingToolClass::getMessageQuote(const QString& source)
{
    QString message = parsingAjaxMessages(expForMessageQuote.match(source).captured(1));
    message.replace("\n", "\n>");

    return message;
}

QString parsingToolClass::getVersionName(const QString& source)
{
    return expForVersionName.match(source).captured(1);
}

QString parsingToolClass::getVersionChangelog(const QString& source)
{
    QString changelog = expForVersionChangelog.match(source).captured(1).replace("\\r\\n", "<br />").replace("\\\"", "\"").replace(" -", "--").replace("   --", "---").replace("\\\\", "\\");
    replaceWithCapNumber(changelog, expForNormalLink, 0, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 0, "</a>");
    return changelog;
}

void parsingToolClass::getListOfHiddenInputFromThisForm(const QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput)
{
    QString formSource;

    if(formName == "form-post-topic")
    {
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

bool parsingToolClass::getTopicLocked(const QString& source)
{
    return expForTopicLocked.match(source).hasMatch();
}

QString parsingToolClass::getCaptchaLink(const QString& source)
{
    return expForCaptcha.match(source).captured(1);
}

QString parsingToolClass::getErrorMessage(const QString& source)
{
    QRegularExpressionMatch match = expForError.match(source);

    if(match.hasMatch() == true)
    {
        return expForError.match(source).captured(1);
    }
    else
    {
        return "Le message n'a pas été envoyé.";
    }
}

QString parsingToolClass::getLastPageOfTopic(const QString& source)
{
    int currentPage = 0;
    QString lastPage;
    currentPage = expForCurrentPage.match(source).captured(1).toInt();

    QRegularExpressionMatchIterator matchIteratorForPgaeLink = expForPageLink.globalMatch(source);
    while(matchIteratorForPgaeLink.hasNext())
    {
        QRegularExpressionMatch matchForPageLink = matchIteratorForPgaeLink.next();
        if(matchForPageLink.captured(2).toInt() > currentPage)
        {
            currentPage = matchForPageLink.captured(2).toInt();
            lastPage = "http://www.jeuxvideo.com" + matchForPageLink.captured(1);
        }
    }

    return lastPage;
}

QString parsingToolClass::getFirstPageOfTopic(const QString& source)
{
    QRegularExpressionMatch matchForFirstPage = expForBeforeLastPage.match(source);

    if(matchForFirstPage.hasMatch() == true)
    {
        return matchForFirstPage.captured(1) + QString::number(1) + matchForFirstPage.captured(3);
    }
    else
    {
        return "";
    }
}

QString parsingToolClass::getBeforeLastPageOfTopic(const QString& source)
{
    QRegularExpressionMatch matchForBeforeLastPage = expForBeforeLastPage.match(source);
    QString pageNumber = matchForBeforeLastPage.captured(2);

    if(pageNumber.isEmpty() == false && pageNumber != "1")
    {
        return matchForBeforeLastPage.captured(1) + QString::number(pageNumber.toInt() - 1) + matchForBeforeLastPage.captured(3);
    }
    else
    {
        return "";
    }
}

QString parsingToolClass::getNameOfTopic(const QString& source)
{
    return expForNameOfTopic.match(source).captured(1).replace("&amp;", "&").replace("&quot;", "\"").replace("&#039;", "\'").replace("&lt;", "<").replace("&gt;", ">");
}

QString parsingToolClass::getNumberOfConnected(const QString& source)
{
    return expForNumberOfConnected.match(source).captured(1);
}

QString parsingToolClass::getNumberOfMp(const QString& source)
{
    int numberOfMp = 0;
    QRegularExpressionMatch match = expForMpJvc.match(source);

    if(match.hasMatch() == true)
    {
        numberOfMp = match.captured(1).toInt();
    }

    if(numberOfMp == 0)
    {
        return QString::number(numberOfMp) + " MP";
    }
    else
    {
        return "<b>" + QString::number(numberOfMp) + " MP</b>";
    }
}

QList<messageStruct> parsingToolClass::getListOfEntireMessagesWithoutMessagePars(const QString& source)
{
    QList<QString> listOfEntireMessage;
    QList<messageStruct> listOfMessages;
    int lastIdOfMessage = 0;
    bool sortNeeded = false;

    listOfEntireMessage = getListOfThisCapNumber(source, expForEntireMessage, 1, false);

    for(const QString& thisMessage : listOfEntireMessage)
    {
        listOfMessages.push_back(messageStruct());
        listOfMessages.back().idOfMessage = expForMessageID.match(thisMessage).captured(1).toInt();
        listOfMessages.back().pseudoInfo.pseudoName = expForPseudo.match(thisMessage).captured(2);
        listOfMessages.back().pseudoInfo.pseudoType = expForPseudo.match(thisMessage).captured(1);
        listOfMessages.back().date = expForDate.match(thisMessage).captured(3);
        listOfMessages.back().wholeDate = expForDate.match(thisMessage).captured(2);
        listOfMessages.back().message = expForMessage.match(thisMessage).captured(1);
        listOfMessages.back().lastTimeEdit = expForEdit.match(thisMessage).captured(1);

        if(listOfMessages.back().pseudoInfo.pseudoName.isEmpty() == true)
        {
            listOfMessages.back().pseudoInfo.pseudoName = "Pseudo supprimé";
        }

        if(lastIdOfMessage > listOfMessages.back().idOfMessage)
        {
            sortNeeded = true;
        }
        lastIdOfMessage = listOfMessages.back().idOfMessage;
    }

    if(sortNeeded == true)
    {
        qSort(listOfMessages);
    }

    return listOfMessages;
}

QList<topicStruct> parsingToolClass::getListOfTopic(const QString& source)
{
    QList<topicStruct> listOfTopic;
    QList<QString> listOfEntireTopic;

    listOfEntireTopic = getListOfThisCapNumber(source, expForEntireTopic, 0, false);

    for(const QString& thisTopic : listOfEntireTopic)
    {
        QString topicInfo = expForTopicNameAndLink.match(thisTopic).captured(1);
        QString link = "http://www.jeuxvideo.com" + topicInfo.left(topicInfo.indexOf("\""));
        QString name = topicInfo.right(topicInfo.size() - topicInfo.indexOf("title=\"") - 7);
        name.replace("&amp;", "&").replace("&quot;", "\"").replace("&#039;", "\'").replace("&lt;", "<").replace("&gt;", ">");
        listOfTopic.append(topicStruct());
        listOfTopic.back().name = name;
        listOfTopic.back().link = link;
        listOfTopic.back().numberOfMessage = expForTopicNumberMessage.match(thisTopic).captured(1);
    }

    return listOfTopic;
}

QString parsingToolClass::getForumOfTopic(const QString& source)
{
    QString forumNumber = expForForum.match(source).captured(1);

    if(forumNumber.isEmpty() == false)
    {
        return "http://www.jeuxvideo.com/forums/0-" + forumNumber + "-0-1-0-1-0-respawn-irc.htm";
    }
    else
    {
        return "";
    }
}

QString parsingToolClass::getForumName(const QString& source)
{
    QString forumName = expForForumName.match(source).captured(1);

    while(forumName.endsWith(" "))
    {
        forumName.remove(forumName.size() - 1, 1);
    }

    return forumName;
}

QString parsingToolClass::jvfLinkToJvcLink(const QString& source)
{
    QRegularExpressionMatch matchForJvfLink = expForJvfLink.match(source);
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

        return "http://www.jeuxvideo.com/forums/" + tempo + "-" + forumNumber + "-" + topicNumber + "-1-0-1-0-" + nameOfTopic + ".htm";
    }
    else
    {
        return "";
    }
}

QString parsingToolClass::parsingMessages(QString thisMessage, infoForMessageParsingStruct infoForParsing, QStringList* listOfStickersUsed, QStringList* listOfNoelshackImageUsed)
{
    QString extraTableStyle;

    if(infoForParsing.betterQuote == true)
    {
        extraTableStyle += "background: " + styleToolClass::getColorInfo().tableBackgroundColor + ";color: " + styleToolClass::getColorInfo().tableTextColor + ";";
    }
    if(infoForParsing.useNewLayout == false)
    {
        extraTableStyle += "margin-bottom: 5px;";
    }

    replaceWithCapNumber(thisMessage, expForCodeBlock, 1, "<p><code style=\"white-space: pre-wrap\">", "</code></p>", -1, "", true);
    replaceWithCapNumber(thisMessage, expForCodeLine, 1, " <code style=\"white-space: pre-wrap\">", "</code> ", -1, "", true);

    thisMessage.replace("\n", "");

    if(infoForParsing.useNewLayout == false)
    {
        replaceWithCapNumber(thisMessage, expForStickers, 0, "<p>", "</p>");
        while(replaceWithCapNumber(thisMessage, QRegularExpression("(" + expForStickers.pattern() + ")\\s*</p>\\s*<p>\\s*(" + expForStickers.pattern() + ")"), 1, "", "", 4) == true)
        {
            //c'normal que c'est vide
        }
    }

    if(infoForParsing.stickerToSmiley == true)
    {
        shortcutToolClass::transformMessage(&thisMessage, "stickerToSmiley");
    }

    if(infoForParsing.showStickers == false)
    {
        replaceWithCapNumber(thisMessage, expForStickers, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    }
    else
    {
        if(listOfStickersUsed != nullptr)
        {
            listOfStickersUsed->append(getListOfThisCapNumber(thisMessage, expForStickers, 2, false));
        }

        replaceWithCapNumber(thisMessage, expForStickers, 2, "<img width=" + QString::number(infoForParsing.stickersSize) + " height=" + QString::number(infoForParsing.stickersSize) + " src=\"resources/stickers/", ".png\" />");
    }

    replaceWithCapNumber(thisMessage, expForSmiley, 1, "<img src=\"resources/smileys/", "\" />");
    replaceWithCapNumber(thisMessage, expForYoutubeVideo, 2, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"http://youtu.be/", "\">http://youtu.be/", 2, "</a>");
    replaceWithCapNumber(thisMessage, expForJvcLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForShortLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForLongLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");

    if(listOfNoelshackImageUsed != nullptr)
    {
        listOfNoelshackImageUsed->append(getListOfThisCapNumber(thisMessage, expForNoelshack, 2, false));

        for(QString& thisNoelshackImage : *listOfNoelshackImageUsed)
        {
            if(thisNoelshackImage.startsWith("http://") == false)
            {
                thisNoelshackImage = "http://" + thisNoelshackImage;
            }
        }

        replaceWithCapNumber(thisMessage, expForNoelshack, 1, "<a href=\"", "\"><img width=" + QString::number(infoForParsing.noelshackImageWidth) + " height=" + QString::number(infoForParsing.noelshackImageHeight) + " src=\"img/", 2, "\" /></a>");
    }
    else
    {
        replaceWithCapNumber(thisMessage, expForNoelshack, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    }

    replaceWithCapNumber(thisMessage, expForSpoilLine, 1, "<span style=\"color: " + styleToolClass::getColorInfo().spoilColor + "; background-color: " + styleToolClass::getColorInfo().spoilColor + ";\">", "</span>", -1, "", false, false, false, 1);
    replaceWithCapNumber(thisMessage, expForSpoilBlock, 1, "<p><span style=\"color: " + styleToolClass::getColorInfo().spoilColor + "; background-color: " + styleToolClass::getColorInfo().spoilColor + ";\">", "</span></p>", -1, "", false, false, false, 1);
    replaceWithCapNumber(thisMessage, expForAllJVCare, 1, "", "", -1, "", false, true);

    removeAllOverlyQuote(thisMessage, infoForParsing.nbMaxQuote);

    thisMessage.replace("<blockquote class=\"blockquote-jv\">", "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\" style=\"margin-top: 5px;border-color: " + styleToolClass::getColorInfo().tableBorderColor + ";" + extraTableStyle + "\"><tr><td>");
    thisMessage.replace("</blockquote>", "</td></tr></table>");

    if(infoForParsing.useNewLayout == false)
    {
        thisMessage.replace(QRegularExpression("</p> *<p>"), "<br /><br />");
        thisMessage.replace("<p>", "");
        thisMessage.replace("</p>", "");
    }

    thisMessage.remove("</div>");
    while(thisMessage.endsWith(' ') == true)
    {
        thisMessage.remove(thisMessage.size() - 1, 1);
    }

    return thisMessage;
}

QString parsingToolClass::parsingAjaxMessages(QString thisMessage)
{
    thisMessage.remove("\n");
    thisMessage.remove("\\r");
    thisMessage.replace("\\\"", "\"");
    thisMessage.replace("\\/", "/");
    thisMessage.replace("\\\\", "\\");
    thisMessage.replace("\\n", "\n");

    QRegularExpressionMatchIterator matchIterator = expForUnicodeInText.globalMatch(thisMessage);
    int lenghtChanged = 0;
    while(matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();

        thisMessage.replace(match.capturedStart(0) + lenghtChanged, match.capturedLength(0), QChar(match.captured(1).toUpper().toUInt(0, 16)));
        lenghtChanged -= match.capturedLength(0);
        lenghtChanged += 1;
    }

    thisMessage.replace("&amp;", "&").replace("&quot;", "\"").replace("&#039;", "\'").replace("&lt;", "<").replace("&gt;", ">");

    return thisMessage;
}

QNetworkRequest parsingToolClass::buildRequestWithThisUrl(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "RespawnIRC");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("Cookie", "");
    request.setRawHeader("Connection", "Keep-Alive");

    return request;
}

QList<QString> parsingToolClass::getListOfThisCapNumber(const QString& source, const QRegularExpression& exp, int capNumber, bool globalMatch)
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

void parsingToolClass::removeAllOverlyQuote(QString& source, int maxNumberQuote)
{
    QRegularExpressionMatch match = expForHtmlTag.match(source);
    ++maxNumberQuote;
    while(match.hasMatch() == true)
    {
        if(match.captured() == "<blockquote class=\"blockquote-jv\">")
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
            QRegularExpressionMatch secMatch = expForHtmlTag.match(source, match.capturedEnd());
            while(secMatch.hasMatch() == true)
            {
                if(secMatch.captured() == "<blockquote class=\"blockquote-jv\">")
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

                secMatch = expForHtmlTag.match(source, secMatch.capturedEnd());
            }

            if(secMatch.capturedStart() != -1)
            {
                source.replace(match.capturedEnd(), (secMatch.capturedStart() - match.capturedEnd()), "[...]");
            }
        }

        match = expForHtmlTag.match(source, match.capturedEnd());
    }
}

bool parsingToolClass::replaceWithCapNumber(QString& source, const QRegularExpression& exp, int capNumber, QString stringBefore, QString stringAfter, int secondCapNumber,
                                            QString stringAfterAfter, bool replaceReturnByBr, bool makeLinkIfPossible, bool replacePByBr, int additionnalOffset)
{
    QRegularExpressionMatch match = exp.match(source);
    int offsetMatch = 0;
    bool hasMatch = false;
    QString newString;

    while(match.hasMatch())
    {
        newString = stringBefore;
        hasMatch = true;
        if(replacePByBr == true)
        {
            newString += match.captured(capNumber).replace("</p>", "<br /><br />");
        }
        else
        {
            newString += match.captured(capNumber);
        }
        newString += stringAfter;

        if(secondCapNumber != -1)
        {
            if(replacePByBr == true)
            {
                newString += match.captured(secondCapNumber).replace("</p>", "<br /><br />");
            }
            else
            {
                newString += match.captured(secondCapNumber);
            }
            newString += stringAfterAfter;
        }

        if(replaceReturnByBr == true)
        {
            newString.replace("\n", "<br />");
        }

        if(makeLinkIfPossible == true &&
                (newString.startsWith("http://") || newString.startsWith("https://")) && newString.contains(" ") == false)
        {
            newString = "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"" + newString + "\">" + newString + "</a>";
        }

        source.replace(match.capturedStart(0), match.capturedLength(0), newString);
        offsetMatch = match.capturedStart(0) + (additionnalOffset == -1 ? newString.size() : additionnalOffset);
        match = exp.match(source, offsetMatch);
    }

    return hasMatch;
}
