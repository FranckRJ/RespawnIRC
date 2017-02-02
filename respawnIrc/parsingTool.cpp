#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QUrl>
#include <QtGlobal>

#include "parsingTool.hpp"
#include "styleTool.hpp"
#include "shortcutTool.hpp"
#include "configDependentVar.hpp"

namespace
{
    const QRegularExpression expForNormalLink("http(s)?://[^\\(\\)\\]\\[ \\\\<>]*", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxTimestamp("<input type=\"hidden\" name=\"ajax_timestamp_liste_messages\" id=\"ajax_timestamp_liste_messages\" value=\"([^\"]*)\" />", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxHash("<input type=\"hidden\" name=\"ajax_hash_liste_messages\" id=\"ajax_hash_liste_messages\" value=\"([^\"]*)\" />", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxModTimestamp("<input type=\"hidden\" name=\"ajax_timestamp_moderation_forum\" id=\"ajax_timestamp_moderation_forum\" value=\"([^\"]*)\" />", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAjaxModHash("<input type=\"hidden\" name=\"ajax_hash_moderation_forum\" id=\"ajax_hash_moderation_forum\" value=\"([^\"]*)\" />", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForMessageEdit("<textarea tabindex=\"3\" class=\"area-editor\" name=\"text_commentaire\" id=\"text_commentaire\" placeholder=\"[^\"]*\">([^<]*)</textarea>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForMessageQuote("\"txt\":\"(.*)\"}", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForVersionName("\"tag_name\"[^\"]*:[^\"]*\"([^\"]*)\"", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForVersionChangelog("\"body\"[^\"]*:[^\"]*\"(.*)\"", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForFormTopic("(<form role=\"form\" class=\"form-post-topic[^\"]*\" method=\"post\" action=\"\".*?>.*?</form>)", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForFormConnect("(<form role=\"form\" class=\"form-connect-jv\" method=\"post\" action=\"\".*?>.*?</form>)", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForInput("<input ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\"/>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForTopicLocked("<div class=\"message-lock-topic\">", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForCaptcha("<img src=\"([^\"]*)\" alt=[^>]*>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForError("<div class=\"alert-row\">([^<]*)</div>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForCurrentPage("<span class=\"page-active\">([^<]*)</span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForPageLink("<span><a href=\"([^\"]*)\" class=\"lien-jv\">([^<]*)</a></span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForNameOfTopic("<span id=\"bloc-title-forum\">([^<]*)</span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForNumberOfConnected("<span class=\"nb-connect-fofo\">([^<]*)</span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForMpJvc("<span[^c]*class=\"account-number-mp[^\"]*\".*?data-val=\"([^\"]*)\"", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForEntireMessage("(<div class=\"bloc-message-forum \".*?)(<span id=\"post_[^\"]*\" class=\"bloc-message-forum-anchor\">|<div class=\"bloc-outils-plus-modo bloc-outils-bottom\">|<div class=\"bloc-pagi-default\">)", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForEntireTopic("<li class=\"\" data-id=\"[^\"]*\">[^<]*<span class=\"topic-subject\">.*?</li>", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForTopicNameAndLink("<a class=\"lien-jv topic-title[^\"]*\" href=\"([^\"]*\" title=\"[^\"]*)\"[^>]*>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForTopicNumberMessage("<span class=\"topic-count\">[^0-9]*([0-9]*)", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForTopicPseudoInfo("<span class=\"JvCare [^ ]* text-([^ ]*) topic-author", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForTopicType("<img src=\"/img/forums/topic-(.*?).png\"", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForMessageID("<div class=\"bloc-message-forum \" data-id=\"([^\"]*)\">", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForAvatars("<img src=\"[^\"]*\" data-srcset=\"(http:)?//([^\"]*)\" class=\"user-avatar-msg\"", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForPseudo("<span class=\"JvCare [^ ]* bloc-pseudo-msg text-([^\"]*)\" target=\"_blank\">[^a-zA-Z0-9_\\[\\]-]*([a-zA-Z0-9_\\[\\]-]*)[^<]*</span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForDate("<div class=\"bloc-date-msg\">([^<]*<span class=\"JvCare [^ ]* lien-jv\" target=\"_blank\">)?[^a-zA-Z0-9]*([^ ]* [^ ]* [^ ]* [^ ]* ([0-9:]*))", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForMessage("<div class=\"bloc-contenu\"><div class=\"txt-msg  text-[^-]*-forum \">((.*?)(?=<div class=\"info-edition-msg\">)|(.*?)(?=<div class=\"signature-msg)|(.*))", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForEdit("<div class=\"info-edition-msg\">Message édité le ([^ ]* [^ ]* [^ ]* [^ ]* [0-9:]*) par <span", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForSignature("<div class=\"signature-msg[^\"]*\">(.*?)</div>", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForTopicLinkNumber("(http://([^/]*)/forums/[^-]*-([^-]*)-([^-]*)-)([^-]*)(-[^-]*-[^-]*-[^-]*-[^.]*.htm)", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForForumName("<title>(.*?)- jeuxvideo.com</title>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForJvfLink("http://jvforum.fr/([^/]*)/([^-]*)-([^/]*)", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForSmiley("<img src=\"//image.jeuxvideo.com/smileys_img/([^\"]*)\" alt=\"[^\"]*\" data-def=\"SMILEYS\" data-code=\"([^\"]*)\" title=\"[^\"]*\" />", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForStickers("<img class=\"img-stickers\" src=\"(http://jv.stkr.fr/p/([^\"]*))\"/>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForLongLink("<span class=\"JvCare [^\"]*\"[^i]*itle=\"([^\"]*)\">[^<]*<i></i><span>[^<]*</span>[^<]*</span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForShortLink("<span class=\"JvCare [^\"]*\" rel=\"nofollow[^\"]*\" target=\"_blank\">([^<]*)</span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForJvcLink("<a href=\"([^\"]*)\"( title=\"[^\"]*\")?>.*?</a>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForNoelshack("<a href=\"([^\"]*)\" data-def=\"NOELSHACK\" target=\"_blank\"><img class=\"img-shack\" .*? src=\"//([^\"]*)\" [^>]*></a>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForYoutubeVideo("<div class=\"player-contenu\"><div class=\"[^\"]*\"><iframe .*? src=\"http(s)?://www.youtube.com/embed/([^\"]*)\"[^>]*></iframe></div></div>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForSpoilLine("<span class=\"bloc-spoil-jv en-ligne\">.*?<span class=\"contenu-spoil\">(.*?)</span></span>", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForSpoilBlock("<span class=\"bloc-spoil-jv\">.*?<span class=\"contenu-spoil\">(.*?)</span></span>", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForCodeBlock("<pre class=\"pre-jv\"><code class=\"code-jv\">([^<]*)</code></pre>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForCodeLine("<code class=\"code-jv\">(.*?)</code>", configDependentVar::regexpBaseOptions | QRegularExpression::DotMatchesEverythingOption);
    const QRegularExpression expForAllJVCare("<span class=\"JvCare [^\"]*\">([^<]*)</span>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForUnicodeInText("\\\\u([a-zA-Z0-9]{4})", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForHtmlTag("<.+?>", configDependentVar::regexpBaseOptions);
    const QRegularExpression expForWebsite("http://([^/]*)/", configDependentVar::regexpBaseOptions);
    QString userAgentToUse = "RespatatouilleIRC";
}

void parsingToolClass::generateNewUserAgent() {
    QStringList allStringForGeneration;
    int lastMessageId = qrand() % 3;

    allStringForGeneration.append("RespawnIRC");
    allStringForGeneration.append("jeuxvideo.com");

    if(lastMessageId == 0)
    {
        allStringForGeneration.append("laissez moi fonctionner");
    }
    else if(lastMessageId == 1)
    {
        allStringForGeneration.append("je suis gentil");
    }
    else
    {
        allStringForGeneration.append("fonctionne optimal");
    }

    userAgentToUse.clear();

    while(allStringForGeneration.isEmpty() == false)
    {
        int currentString = qrand() % allStringForGeneration.size();
        int currentChar = qrand() % allStringForGeneration.at(currentString).size();

        userAgentToUse += allStringForGeneration.at(currentString).at(currentChar);
        allStringForGeneration[currentString].remove(currentChar, 1);

        if(allStringForGeneration.at(currentString).isEmpty() == true)
        {
            allStringForGeneration.removeAt(currentString);
        }
    }
}

bool parsingToolClass::checkIfTopicAreSame(const QString& firstTopic, const QString& secondTopic)
{
    QRegularExpressionMatch matcherForFirstTopic = expForTopicLinkNumber.match(firstTopic);
    QRegularExpressionMatch matcherForSecondTopic = expForTopicLinkNumber.match(secondTopic);

    if(matcherForFirstTopic.hasMatch() == true && matcherForSecondTopic.hasMatch() == true)
    {
        return matcherForFirstTopic.captured(2) == matcherForSecondTopic.captured(2) &&
               matcherForFirstTopic.captured(3) == matcherForSecondTopic.captured(3) &&
               matcherForFirstTopic.captured(4) == matcherForSecondTopic.captured(4);
    }
    else
    {
        return false;
    }
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

QString parsingToolClass::getMessageEdit(const QString& source)
{
    return specialCharToNormalChar(expForMessageEdit.match(source).captured(1));
}

QString parsingToolClass::getMessageQuote(const QString& source)
{
    QString message = specialCharToNormalChar(parsingAjaxMessages(expForMessageQuote.match(source).captured(1)));
    message.replace("\n", "\n>");

    return message;
}

QString parsingToolClass::getWebsite(const QString& topicLink)
{
    return expForWebsite.match(topicLink).captured(1);
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

QString parsingToolClass::getNextPageOfTopic(const QString& source, const QString& website)
{
    int currentPage = 0;
    currentPage = expForCurrentPage.match(source).captured(1).toInt();

    QRegularExpressionMatchIterator matchIteratorForPgaeLink = expForPageLink.globalMatch(source);
    while(matchIteratorForPgaeLink.hasNext())
    {
        QRegularExpressionMatch matchForPageLink = matchIteratorForPgaeLink.next();
        if(matchForPageLink.captured(2).toInt() == currentPage + 1)
        {
            return "http://" + website + matchForPageLink.captured(1);
        }
    }

    return "";
}

QString parsingToolClass::getLastPageOfTopic(const QString& source, const QString& website)
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
            lastPage = "http://" + website + matchForPageLink.captured(1);
        }
    }

    return lastPage;
}

QString parsingToolClass::getFirstPageOfTopic(const QString& topicLink)
{
    QRegularExpressionMatch matchForFirstPage = expForTopicLinkNumber.match(topicLink);

    if(matchForFirstPage.hasMatch() == true)
    {
        return matchForFirstPage.captured(1) + "1" + matchForFirstPage.captured(6);
    }
    else
    {
        return "";
    }
}

QString parsingToolClass::getBeforeLastPageOfTopic(const QString& topicLink)
{
    QRegularExpressionMatch matchForBeforeLastPage = expForTopicLinkNumber.match(topicLink);
    QString pageNumber = matchForBeforeLastPage.captured(5);

    if(pageNumber.isEmpty() == false && pageNumber != "1")
    {
        return matchForBeforeLastPage.captured(1) + QString::number(pageNumber.toInt() - 1) + matchForBeforeLastPage.captured(6);
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

int parsingToolClass::getNumberOfMp(const QString& source)
{
    int numberOfMp = 0;
    QRegularExpressionMatch match = expForMpJvc.match(source);

    if(match.hasMatch() == true)
    {
        return numberOfMp = match.captured(1).toInt();
    }
    else
    {
        return 0;
    }
}

QList<messageStruct> parsingToolClass::getListOfEntireMessagesWithoutMessagePars(const QString& source)
{
    QList<QString> listOfEntireMessage;
    QList<messageStruct> listOfMessages;
    long lastIdOfMessage = 0;
    bool sortNeeded = false;

    listOfEntireMessage = getListOfThisCapNumber(source, expForEntireMessage, 1, false);

    for(const QString& thisMessage : listOfEntireMessage)
    {
        listOfMessages.push_back(messageStruct());
        listOfMessages.back().idOfMessage = expForMessageID.match(thisMessage).captured(1).toLong();
        listOfMessages.back().avatarLink = expForAvatars.match(thisMessage).captured(2);
        listOfMessages.back().pseudoInfo.pseudoName = expForPseudo.match(thisMessage).captured(2);
        listOfMessages.back().pseudoInfo.pseudoType = expForPseudo.match(thisMessage).captured(1);
        listOfMessages.back().date = expForDate.match(thisMessage).captured(3);
        listOfMessages.back().wholeDate = expForDate.match(thisMessage).captured(2);
        listOfMessages.back().message = expForMessage.match(thisMessage).captured(1);
        listOfMessages.back().lastTimeEdit = expForEdit.match(thisMessage).captured(1);
        listOfMessages.back().signature = expForSignature.match(thisMessage).captured(1);

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

QList<topicStruct> parsingToolClass::getListOfTopic(const QString& source, const QString& website)
{
    QList<topicStruct> listOfTopic;
    QList<QString> listOfEntireTopic;

    listOfEntireTopic = getListOfThisCapNumber(source, expForEntireTopic, 0, false);

    for(const QString& thisTopic : listOfEntireTopic)
    {
        QString topicInfo = expForTopicNameAndLink.match(thisTopic).captured(1);
        QString link = "http://" + website + topicInfo.left(topicInfo.indexOf("\""));
        QString name = topicInfo.right(topicInfo.size() - topicInfo.indexOf("title=\"") - 7);
        name.replace("&amp;", "&").replace("&quot;", "\"").replace("&#039;", "\'").replace("&lt;", "<").replace("&gt;", ">");
        listOfTopic.append(topicStruct());
        listOfTopic.back().name = name;
        listOfTopic.back().link = link;
        listOfTopic.back().numberOfMessage = expForTopicNumberMessage.match(thisTopic).captured(1);
        listOfTopic.back().pseudoInfo.pseudoType = expForTopicPseudoInfo.match(thisTopic).captured(1);
        listOfTopic.back().topicType = expForTopicType.match(thisTopic).captured(1);
    }

    return listOfTopic;
}

QString parsingToolClass::getForumOfTopic(const QString& topicLink)
{
    QRegularExpressionMatch infosMatcher = expForTopicLinkNumber.match(topicLink);

    if(infosMatcher.hasMatch() == true)
    {
        return "http://" + infosMatcher.captured(2) + "/forums/0-" + infosMatcher.captured(3) + "-0-1-0-1-0-respawn-irc.htm";
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

QString parsingToolClass::jvfLinkToJvcLink(const QString& jvfTopicLink)
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

        return "http://www.jeuxvideo.com/forums/" + tempo + "-" + forumNumber + "-" + topicNumber + "-1-0-1-0-" + nameOfTopic + ".htm";
    }
    else
    {
        return "";
    }
}

QString parsingToolClass::parsingMessages(QString thisMessage, infoForMessageParsingStruct infoForParsing, bool reallyDownloadStickers)
{
    QString extraTableStyle;

    if(infoForParsing.betterQuote == true)
    {
        extraTableStyle += "background: " + styleToolClass::getColorInfo().tableBackgroundColor + ";color: " + styleToolClass::getColorInfo().tableTextColor + ";";
    }

    replaceWithCapNumber(thisMessage, expForCodeBlock, 1, "<p><code style=\"white-space: pre-wrap\">", "</code></p>", -1, "", true);
    replaceWithCapNumber(thisMessage, expForCodeLine, 1, " <code style=\"white-space: pre-wrap\">", "</code> ", -1, "", false);

    thisMessage.replace("\n", "");
    thisMessage.replace("\r", "");

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
        if(infoForParsing.listOfStickersUsed != nullptr && reallyDownloadStickers == true)
        {
            infoForParsing.listOfStickersUsed->append(getListOfThisCapNumber(thisMessage, expForStickers, 2, false));
        }

        replaceWithCapNumber(thisMessage, expForStickers, 2, "<img width=" + QString::number(infoForParsing.stickersSize) + " height=" + QString::number(infoForParsing.stickersSize) + " src=\"resources/stickers/", ".png\" />");
    }

    replaceWithCapNumber(thisMessage, expForSmiley, 1, "<img src=\"resources/smileys/", "\" />");
    replaceWithCapNumber(thisMessage, expForYoutubeVideo, 2, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"http://youtu.be/", "\">http://youtu.be/", 2, "</a>");
    replaceWithCapNumber(thisMessage, expForJvcLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForShortLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForLongLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");

    if(infoForParsing.listOfNoelshackImageUsed != nullptr)
    {
        infoForParsing.listOfNoelshackImageUsed->append(getListOfThisCapNumber(thisMessage, expForNoelshack, 2, false));

        for(QString& thisNoelshackImage : *infoForParsing.listOfNoelshackImageUsed)
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

    replaceWithCapNumber(thisMessage, expForSpoilLine, 1, "<span style=\"color: " + styleToolClass::getColorInfo().spoilColor + "; background-color: " + styleToolClass::getColorInfo().spoilColor + ";\">", "</span>", -1, "", false, false, true, 1);
    replaceWithCapNumber(thisMessage, expForSpoilBlock, 1, "<p><span style=\"color: " + styleToolClass::getColorInfo().spoilColor + "; background-color: " + styleToolClass::getColorInfo().spoilColor + ";\">", "</span></p>", -1, "", false, false, true, 1);
    replaceWithCapNumber(thisMessage, expForAllJVCare, 1, "", "", -1, "", false, true);

    thisMessage.replace("<blockquote class=\"blockquote-jv\">", "<blockquote>");
    removeAllOverlyQuote(thisMessage, infoForParsing.nbMaxQuote);

    thisMessage.replace("<ul class=\"liste-default-jv\">", "<br /><br /><ul>");
    thisMessage.replace("<ol class=\"liste-default-jv\">", "<br /><br /><ol>");
    thisMessage.replace("</ul>", "<ul><br /><br />");
    thisMessage.replace("</ol>", "<ol><br /><br />");

    thisMessage.replace(QRegularExpression("(<br /> *){0,2}</p> *<p>( *<br />){0,2}"), "<br /><br />");
    thisMessage.replace(QRegularExpression("<br /> *<(/)?p> *<br />"), "<br /><br />");
    thisMessage.replace(QRegularExpression("(<br /> *){1,2}<(/)?p>"), "<br /><br />");
    thisMessage.replace(QRegularExpression("<(/)?p>(<br /> *){1,2}"), "<br /><br />");
    thisMessage.replace(QRegularExpression("<(/)?p>"), "<br /><br />");
    thisMessage.replace(QRegularExpression("(<br /> *)*(<(/)?blockquote>)( *<br />)*"), "\\2");

    thisMessage.replace("<br /><br /><ul>", "<ul>");
    thisMessage.replace("<br /><br /><ol>", "<ol>");
    thisMessage.replace("<ul><br /><br />", "</ul>");
    thisMessage.replace("<ol><br /><br />", "</ol>");

    thisMessage.replace("<blockquote>", "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\" style=\"margin-top: 5px;margin-bottom: 5px;border-color: " + styleToolClass::getColorInfo().tableBorderColor + ";" + extraTableStyle + "\"><tr><td>");
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

QString parsingToolClass::parsingAjaxMessages(QString thisMessage)
{
    thisMessage.remove("\n");
    thisMessage.remove("\r");
    thisMessage.replace(QRegularExpression("(?<!\\\\)\\\\r"), "");
    thisMessage.replace(QRegularExpression("(?<!\\\\)\\\\\""), "\"");
    thisMessage.replace(QRegularExpression("(?<!\\\\)\\\\/"), "/");
    thisMessage.replace(QRegularExpression("(?<!\\\\)\\\\n"), "\n");
    thisMessage.replace("\\\\", "\\");

    QRegularExpressionMatchIterator matchIterator = expForUnicodeInText.globalMatch(thisMessage);
    int lenghtChanged = 0;
    while(matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();

        thisMessage.replace(match.capturedStart(0) + lenghtChanged, match.capturedLength(0), QChar(match.captured(1).toUpper().toUInt(0, 16)));
        lenghtChanged -= match.capturedLength(0);
        lenghtChanged += 1;
    }

    return thisMessage;
}

QString parsingToolClass::specialCharToNormalChar(QString thisMessage)
{
    return thisMessage.replace("&amp;", "&").replace("&quot;", "\"").replace("&#039;", "\'").replace("&lt;", "<").replace("&gt;", ">");
}

QNetworkRequest parsingToolClass::buildRequestWithThisUrl(QString url)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", userAgentToUse.toUtf8());
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
            QRegularExpressionMatch secMatch = expForHtmlTag.match(source, match.capturedEnd());
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
                                            QString stringAfterAfter, bool replaceReturnByBr, bool makeLinkIfPossible, bool removeFirstAndLastP, int additionnalOffset)
{
    QRegularExpressionMatch match = exp.match(source);
    int offsetMatch = 0;
    bool hasMatch = false;
    QString newString;

    while(match.hasMatch())
    {
        newString = stringBefore;
        hasMatch = true;
        if(removeFirstAndLastP == true)
        {
            QString tmpString = match.captured(capNumber).trimmed();
            while(tmpString.startsWith("<p>") == true)
            {
                tmpString.remove(0, 3);
                tmpString = tmpString.trimmed();
            }
            while(tmpString.endsWith("</p>") == true)
            {
                tmpString.remove(tmpString.size() - 4, 4);
                tmpString = tmpString.trimmed();
            }
            newString += tmpString;
        }
        else
        {
            newString += match.captured(capNumber);
        }
        newString += stringAfter;

        if(secondCapNumber != -1)
        {
            if(removeFirstAndLastP == true)
            {
                QString tmpString = match.captured(secondCapNumber).trimmed();
                while(tmpString.startsWith("<p>") == true)
                {
                    tmpString.remove(0, 3);
                    tmpString = tmpString.trimmed();
                }
                while(tmpString.endsWith("</p>") == true)
                {
                    tmpString.remove(tmpString.size() - 4, 4);
                    tmpString = tmpString.trimmed();
                }
                newString += tmpString;
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
