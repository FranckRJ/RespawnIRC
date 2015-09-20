#include "parsingTool.hpp"
#include "styleTool.hpp"

QRegularExpression parsingToolClass::expForNormalLink("https://[^\\(\\)\\]\\[ ]*", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForAjaxTimestamp("<input type=\"hidden\" name=\"ajax_timestamp_liste_messages\" id=\"ajax_timestamp_liste_messages\" value=\"([^\"]*)\" />", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForAjaxHash("<input type=\"hidden\" name=\"ajax_hash_liste_messages\" id=\"ajax_hash_liste_messages\" value=\"([^\"]*)\" />", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForMessageEdit("<textarea tabindex=\"3\" class=\"area-editor\" name=\"text_commentaire\" id=\"text_commentaire\" placeholder=\"[^\"]*\">([^<]*)</textarea>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForMessageQuote("\"txt\":\"(.*)\"}", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
QRegularExpression parsingToolClass::expForVersionName("\"tag_name\"[^\"]*:[^\"]*\"([^\"]*)\"", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForVersionChangelog("\"body\"[^\"]*:[^\"]*\"(.*)\"", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForFormTopic("(<form role=\"form\" class=\"form-post-topic[^\"]*\" method=\"post\" action=\"\">.*?</form>)", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
QRegularExpression parsingToolClass::expForFormConnect("(<form role=\"form\" class=\"form-connect-jv\" method=\"post\" action=\"\">.*?</form>)", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
QRegularExpression parsingToolClass::expForInput("<input ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\"/>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForCaptcha("<img src=\"([^\"]*)\" alt=[^>]*>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForCurrentPage("<span class=\"page-active\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForPageLink("<span><a href=\"([^\"]*)\" class=\"lien-jv\">([^<]*)</a></span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForBeforeLastPage("(http://www.jeuxvideo.com/forums/[^-]*-[^-]*-[^-]*-)([^-]*)(-[^-]*-[^-]*-[^-]*-[^.]*.htm)", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForNameOfTopic("<span id=\"bloc-title-forum\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForNumberOfConnected("<span class=\"nb-connect-fofo\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForEntireMessage("(<div class=\"bloc-message-forum \".*?)(<div class=\"bloc-message-forum \"|<div class=\"bloc-pagi-default\">)", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
QRegularExpression parsingToolClass::expForListOfTopic("<div class=\"titre-topic\">[^<]*<a href=\"([^\"]*\" title=\"[^\"]*)\">", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForMessageID("<div class=\"bloc-message-forum \" id=\"post_[^\"]*\" data-id=\"([^\"]*)\">", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForPseudo("<span class=\"JvCare [^ ]* bloc-pseudo-msg text-[^\"]*\" target=\"_blank\">[^a-zA-Z0-9_\\[\\]-]*([a-zA-Z0-9_\\[\\]-]*)[^<]*</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForDate("<div class=\"bloc-date-msg\">[^<]*<span class=\"JvCare [^ ]* lien-jv\" target=\"_blank\">[^ ]* [^ ]* [^ ]* [^ ]* ([0-9:]*)[^<]*</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForMessage("<div class=\"bloc-contenu\"><div class=\"txt-msg  text-[^-]*-forum \">(.*?)</div>", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
QRegularExpression parsingToolClass::expForEdit("<div class=\"info-edition-msg\">Message édité le ([^ ]* [^ ]* [^ ]* [^ ]* [0-9:]*) par <span", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForForum("http://www.jeuxvideo.com/forums/[^-]*-([^-]*)-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^.]*.htm", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForJvfLink("http://jvforum.fr/([^/]*)/([^-]*)-([^/]*)", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForSmiley("<img src=\"//image.jeuxvideo.com/smileys_img/([^\"]*)\" alt=\"[^\"]*\" data-def=\"SMILEYS\" data-code=\"([^\"]*)\" title=\"[^\"]*\" />", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForStickers("<img class=\"img-stickers\" src=\"([^\"]*)\"/>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForLongLink("<span class=\"JvCare [^\"]*\"[^i]*itle=\"([^\"]*)\">[^<]*<i></i><span>[^<]*</span>[^<]*</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForShortLink("<span class=\"JvCare [^\"]*\" rel=\"nofollow\" target=\"_blank\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForJvcLink("<a href=\"([^\"]*)\"( title=\"[^\"]*\")?>.*?</a>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForNoelshack("<a href=\"([^\"]*)\" data-def=\"NOELSHACK\" target=\"_blank\"><img class=\"img-shack\" [^>]*></a>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForSpoilLine("<span class=\"bloc-spoil-jv en-ligne\"><span class=\"contenu-spoil\">(.*?)</span></span>", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
QRegularExpression parsingToolClass::expForSpoilBlock("<span class=\"bloc-spoil-jv\"><span class=\"contenu-spoil\">(.*?)</span></span>", QRegularExpression::OptimizeOnFirstUsageOption | QRegularExpression::DotMatchesEverythingOption);
QRegularExpression parsingToolClass::expForAllJVCare("<span class=\"JvCare [^\"]*\">([^<]*)</span>", QRegularExpression::OptimizeOnFirstUsageOption);
QRegularExpression parsingToolClass::expForUnicodeInText("\\\\u([a-zA-Z0-9]{4})", QRegularExpression::OptimizeOnFirstUsageOption);

QString parsingToolClass::getAjaxInfo(const QString& source)
{
    QString ajaxTimestamp = expForAjaxTimestamp.match(source).captured(1);
    QString ajaxHash = expForAjaxHash.match(source).captured(1);

    if(ajaxTimestamp.isEmpty() == false && ajaxHash.isEmpty() == false)
    {
        return ("ajax_timestamp=" + ajaxTimestamp + "&ajax_hash=" + ajaxHash);
    }
    else
    {
        return "";
    }
}

QString parsingToolClass::getMessageEdit(QString& source)
{
    source.remove("\n");
    source.replace("\\\"", "\"");
    source.replace("\\/", "/");

    return parsingAjaxMessages(expForMessageEdit.match(source).captured(1));
}

QString parsingToolClass::getMessageQuote(QString &source)
{
    QString message = parsingAjaxMessages(expForMessageQuote.match(source).captured(1));
    message.replace("\n", "\n>");

    return message;
}

QString parsingToolClass::getVersionName(const QString &source)
{
    return expForVersionName.match(source).captured(1);
}

QString parsingToolClass::getVersionChangelog(const QString &source)
{
    QString changelog = expForVersionChangelog.match(source).captured(1).replace("\\r\\n", "<br />").replace("\\\"", "\"").replace(" -", "--").replace("   --", "---");
    replaceWithCapNumber(changelog, expForNormalLink, 0, "<a href=\"", "\">", 0, "</a>");
    return changelog;
}

void parsingToolClass::getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput)
{
    if(formName == "form-post-topic")
    {
        source = expForFormTopic.match(source).captured(1);
    }
    else if(formName == "form-connect-jv")
    {
        source = expForFormConnect.match(source).captured(1);
    }

    QRegularExpressionMatchIterator matchIteratorForInput = expForInput.globalMatch(source);
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

QString parsingToolClass::getCaptchaLink(const QString& source)
{
    return expForCaptcha.match(source).captured(1);
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

QString parsingToolClass::getBeforeLastPageOfTopic(const QString &source)
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

QString parsingToolClass::getNumberOfConnected(const QString &source)
{
    return expForNumberOfConnected.match(source).captured(1);
}

QList<messageStruct> parsingToolClass::getListOfEntireMessages(const QString &source)
{
    QList<QString> listOfEntireMessage;
    QList<messageStruct> listOfMessages;

    listOfEntireMessage = getListOfThisCapNumber(source, expForEntireMessage, 1, false);

    for(int i = 0; i < listOfEntireMessage.size(); ++i)
    {
        listOfMessages.push_back(messageStruct());
        listOfMessages.back().idOfMessage = expForMessageID.match(listOfEntireMessage.at(i)).captured(1).toInt();
        listOfMessages.back().pseudo = expForPseudo.match(listOfEntireMessage.at(i)).captured(1);
        listOfMessages.back().date = expForDate.match(listOfEntireMessage.at(i)).captured(1);
        listOfMessages.back().message = parsingMessages(expForMessage.match(listOfEntireMessage.at(i)).captured(1));
        listOfMessages.back().lastTimeEdit = expForEdit.match(listOfEntireMessage.at(i)).captured(1);
    }

    return listOfMessages;
}

QList<topicStruct> parsingToolClass::getListOfTopic(const QString &source)
{
    QList<topicStruct> listOfTopic;
    QList<QString> listOfTopicInfo;

    listOfTopicInfo = getListOfThisCapNumber(source, expForListOfTopic, 1, false);

    for(int i = 0; i < listOfTopicInfo.size(); ++i)
    {
        QString topicInfo = listOfTopicInfo.at(i);
        QString link = "http://www.jeuxvideo.com" + topicInfo.left(topicInfo.indexOf("\""));
        QString name = topicInfo.right(topicInfo.size() - topicInfo.indexOf("title=\"") - 7);
        name.replace("&amp;", "&").replace("&quot;", "\"").replace("&#039;", "\'").replace("&lt;", "<").replace("&gt;", ">");
        listOfTopic.append(topicStruct());
        listOfTopic.back().name = name;
        listOfTopic.back().link = link;
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

QString parsingToolClass::jvfLinkToJvcLink(const QString &source)
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

QString parsingToolClass::parsingMessages(QString thisMessage)
{
    thisMessage.replace("\n", "");

    replaceWithCapNumber(thisMessage, expForSmiley, 1, "<img src=\"smileys/", "\" />");
    replaceWithCapNumber(thisMessage, expForStickers, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForLongLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForShortLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForJvcLink, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForNoelshack, 1, "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"", "\">", 1, "</a>");
    replaceWithCapNumber(thisMessage, expForSpoilLine, 1, "<span style=\"color: " + styleToolClass::getColorInfo().spoilColor + "; background-color: " + styleToolClass::getColorInfo().spoilColor + ";\">", "</span>");
    replaceWithCapNumber(thisMessage, expForSpoilBlock, 1, "<br /><br /><span style=\"color: " + styleToolClass::getColorInfo().spoilColor + "; background-color: " + styleToolClass::getColorInfo().spoilColor + ";\">", "</span>");
    replaceWithCapNumber(thisMessage, expForAllJVCare, 1);

    thisMessage.replace("<blockquote class=\"blockquote-jv\">", "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\" style=\"margin-bottom: 5px;margin-top: 5px;border-color: " + styleToolClass::getColorInfo().tableBorderColor + ";\"><tr><td>");
    thisMessage.replace("</blockquote>", "</td></tr></table>");

    thisMessage.replace(QRegularExpression("</p> *<p>"), "<br /><br />");
    thisMessage.replace("<p>", "");
    thisMessage.replace("</p>", "");

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

QList<QString> parsingToolClass::getListOfThisCapNumber(const QString& source, QRegularExpression& exp, int capNumber, bool globalMatch)
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

void parsingToolClass::replaceWithCapNumber(QString& source, QRegularExpression& exp, int capNumber, QString stringBefore, QString stringAfter, int secondCapNumber, QString stringAfterAfter)
{
    QRegularExpressionMatchIterator matchIterator = exp.globalMatch(source);
    int lenghtChanged = 0;
    QString newString;

    while(matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        newString = stringBefore;
        newString += match.captured(capNumber);
        newString += stringAfter;

        if(secondCapNumber != -1)
        {
            newString += match.captured(secondCapNumber);
            newString += stringAfterAfter;
        }

        source.replace(match.capturedStart(0) + lenghtChanged, match.capturedLength(0), newString);
        lenghtChanged -= match.capturedLength(0);
        lenghtChanged += newString.size();
    }
}
