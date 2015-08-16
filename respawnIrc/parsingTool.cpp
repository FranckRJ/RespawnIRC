#include "parsingTool.hpp"

void parsingToolClass::getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput)
{
    QRegExp expForForm("(<form [^>]*" + formName + ".*</form>)");
    expForForm.setMinimal(true);
    source.contains(expForForm);
    source = expForForm.cap(1);

    QRegExp expForInput("<input ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\" ([^=]*)=\"([^\"]*)\"/>");
    int posForExpForInput = 0;
    while((posForExpForInput = expForInput.indexIn(source, posForExpForInput)) != -1)
    {
        if(expForInput.cap(1) == "type")
        {
            if(expForInput.cap(3) == "name")
            {
                listOfInput.push_back(QPair<QString, QString>(expForInput.cap(4), expForInput.cap(6)));
            }
            else
            {
                listOfInput.push_back(QPair<QString, QString>(expForInput.cap(6), expForInput.cap(4)));
            }
        }
        else if(expForInput.cap(3) == "type")
        {
            if(expForInput.cap(1) == "name")
            {
                listOfInput.push_back(QPair<QString, QString>(expForInput.cap(2), expForInput.cap(6)));
            }
            else
            {
                listOfInput.push_back(QPair<QString, QString>(expForInput.cap(6), expForInput.cap(2)));
            }
        }
        else
        {
            if(expForInput.cap(1) == "name")
            {
                listOfInput.push_back(QPair<QString, QString>(expForInput.cap(2), expForInput.cap(4)));
            }
            else
            {
                listOfInput.push_back(QPair<QString, QString>(expForInput.cap(4), expForInput.cap(2)));
            }
        }
        posForExpForInput += expForInput.matchedLength();
    }
}

QString parsingToolClass::getCaptchaLink(const QString& source)
{
    QRegExp expForCaptcha("<img src=\"([^\"]*)\" alt=[^>]*>");
    expForCaptcha.setMinimal(true);

    return searchThisCapNumber(source, expForCaptcha, 1);
}

QString parsingToolClass::getLastPageOfTopic(const QString& source)
{
    int currentPage = 0;
    QString lastPage;
    QRegExp expForCurrentPage("<span class=\"page-active\">([^<]*)</span>");
    expForCurrentPage.setMinimal(true);
    source.contains(expForCurrentPage);
    currentPage = expForCurrentPage.cap(1).toInt();

    QRegExp expForPageLink("<span><a href=\"([^\"]*)\" class=\"lien-jv\">([^<]*)</a></span>");
    int posForExpForPageLink = 0;
    while((posForExpForPageLink = expForPageLink.indexIn(source, posForExpForPageLink)) != -1)
    {
        if(expForPageLink.cap(2).toInt() > currentPage)
        {
            currentPage = expForPageLink.cap(2).toInt();
            lastPage = "http://www.jeuxvideo.com" + expForPageLink.cap(1);
        }

        posForExpForPageLink += expForPageLink.matchedLength();
    }

    return lastPage;
}

QString parsingToolClass::getNameOfTopic(const QString& source)
{
    QRegExp expForNameOfTopic("<span id=\"bloc-title-forum\">([^<]*)</span>");
    expForNameOfTopic.setMinimal(true);

    return searchThisCapNumber(source, expForNameOfTopic, 1);
}

QString parsingToolClass::getNumberOfConnected(const QString &source)
{
    QRegExp expForNumberOfConnected("<span class=\"nb-connect-fofo\">([^<]*)</span>");
    expForNumberOfConnected.setMinimal(true);

    return searchThisCapNumber(source, expForNumberOfConnected, 1);
}

QList<messageStruct> parsingToolClass::getListOfEntireMessages(const QString &source)
{
    QList<QString> listOfEntireMessage;
    QList<messageStruct> listOfMessages;
    QRegExp expForEntireMessage("(<div class=\"bloc-message-forum \".*)(<div class=\"bloc-message-forum \"|<div class=\"bloc-pagi-default\">)");
    QRegExp expForMessageID("<div class=\"bloc-message-forum \" id=\"post_[^\"]*\" data-id=\"([^\"]*)\">");
    QRegExp expForPseudo("<span class=\"JvCare [^ ]* bloc-pseudo-msg text-[^\"]*\" target=\"_blank\">[^a-zA-Z0-9_\\[\\]-]*([a-zA-Z0-9_\\[\\]-]*)[^<]*</span>");
    QRegExp expForDate("<div class=\"bloc-date-msg\">[^<]*<span class=\"JvCare [^ ]* lien-jv\" target=\"_blank\">[^ ]* [^ ]* [^ ]* [^ ]* ([0-9:]*)[^<]*</span>");
    QRegExp expForMessage("<div class=\"bloc-contenu\"><div class=\"txt-msg  text-enrichi-forum \">(.*)</div>");
    QRegExp expForEdit("<div class=\"info-edition-msg\">Message édité le ([^ ]* [^ ]* [^ ]* [^ ]* [0-9:]*) par <span");
    expForEntireMessage.setMinimal(true);
    expForMessage.setMinimal(true);

    listOfEntireMessage = getListOfThisCapNumber(source, expForEntireMessage, 1, true);

    for(int i = 0; i < listOfEntireMessage.size(); ++i)
    {
        listOfMessages.push_back(messageStruct());
        listOfMessages.back().idOfMessage = searchThisCapNumber(listOfEntireMessage.at(i), expForMessageID, 1).toInt();
        listOfMessages.back().pseudo = searchThisCapNumber(listOfEntireMessage.at(i), expForPseudo, 1);
        listOfMessages.back().date = searchThisCapNumber(listOfEntireMessage.at(i), expForDate, 1);
        listOfMessages.back().message = parsingMessages(searchThisCapNumber(listOfEntireMessage.at(i), expForMessage, 1));
        listOfMessages.back().lastTimeEdit = searchThisCapNumber(listOfEntireMessage.at(i), expForEdit, 1);
    }

    return listOfMessages;
}

QString parsingToolClass::getForumOfTopic(const QString& source)
{
    QRegExp expForForum("http://www.jeuxvideo.com/forums/[^-]*-([^-]*)-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^.]*.htm");

    return searchThisCapNumber(source, expForForum, 1, "http://www.jeuxvideo.com/forums/0-", "-0-1-0-1-0-respawn-irc.htm");
}

QString parsingToolClass::parsingMessages(QString thisMessage)
{
    QRegExp expForSmiley("<img src=\"//image.jeuxvideo.com/smileys_img/([^\"]*)\" alt=\"[^\"]*\" data-def=\"SMILEYS\" data-code=\"([^\"]*)\" title=\"[^\"]*\" />");
    QRegExp expForStickers("<img class=\"img-stickers\" src=\"([^\"]*)\"/>");
    QRegExp expForLongLink("<span class=\"JvCare [^\"]*\".*title=\"([^\"]*)\">[^<]*<i></i><span>[^<]*</span>[^<]*</span>");
    QRegExp expForShortLink("<span class=\"JvCare [^\"]*\" rel=\"nofollow\" target=\"_blank\">([^<]*)</span>");
    QRegExp expForNoelshack("<a href=\"([^\"]*)\" data-def=\"NOELSHACK\" target=\"_blank\"><img class=\"img-shack\" [^>]*></a>");
    QRegExp expForSpoilLine("<span class=\"bloc-spoil-jv en-ligne\"><span class=\"contenu-spoil\">([^<]*)</span></span>");
    QRegExp expForSpoilBlock("<span class=\"bloc-spoil-jv\"><span class=\"contenu-spoil\">([^<]*)</span></span>");
    QRegExp expForAllJVCare("<span class=\"JvCare [^\"]*\">([^<]*)</span>");

    thisMessage.replace("\n", "");

    replaceWithCapNumber(thisMessage, expForSmiley, 1, false, "<img src=\"smileys/", "\" />");
    replaceWithCapNumber(thisMessage, expForStickers, 1, true);
    replaceWithCapNumber(thisMessage, expForLongLink, 1, true);
    replaceWithCapNumber(thisMessage, expForShortLink, 1, true);
    replaceWithCapNumber(thisMessage, expForNoelshack, 1, true);
    replaceWithCapNumber(thisMessage, expForSpoilLine, 1, false, "<span style=\"color: black; background-color: black;\">", "</span>");
    replaceWithCapNumber(thisMessage, expForSpoilBlock, 1, false, "<br /><br /><span style=\"color: black; background-color: black;\">", "</span>");
    replaceWithCapNumber(thisMessage, expForAllJVCare, 1);

    thisMessage.replace("<blockquote class=\"blockquote-jv\">", "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\" style=\"margin-bottom: 5px;margin-top: 5px;\"><tr><td>");
    thisMessage.replace("</blockquote>", "</td></tr></table>");

    thisMessage.replace(QRegExp("</p> *<p>"), "<br /><br />");
    thisMessage.replace("<p>", "");
    thisMessage.replace("</p>", "");

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

QList<QString> parsingToolClass::getListOfThisCapNumber(const QString& source, QRegExp exp, int capNumber, bool onlyUseCapString)
{
    QList<QString> listOfString;
    int posForExp = 0;
    while((posForExp = exp.indexIn(source, posForExp)) != -1)
    {
        listOfString.push_back(exp.cap(capNumber));
        if(onlyUseCapString == false)
        {
            posForExp += exp.matchedLength();
        }
        else
        {
            posForExp += exp.cap(capNumber).size();
        }
    }

    return listOfString;
}

void parsingToolClass::replaceWithCapNumber(QString& source, QRegExp exp, int capNumber, bool createLink, QString stringBefore, QString stringAfter, int secondCapNumber, QString stringAfterAfter)
{
    int posForExp = 0;
    QString newString;

    while((posForExp = exp.indexIn(source, posForExp)) != -1)
    {
        newString = stringBefore;

        if(createLink == false)
        {
            newString += exp.cap(capNumber);
        }
        else
        {
            newString += "<a href=\"" + exp.cap(capNumber) +"\">" + exp.cap(capNumber) + "</a>";
        }

        newString += stringAfter;

        if(secondCapNumber != 0)
        {
            newString += exp.cap(secondCapNumber);
            newString += stringAfterAfter;
        }

        source.replace(posForExp, exp.cap(0).length(), newString);
        posForExp += newString.length();
    }
}

QString parsingToolClass::searchThisCapNumber(const QString& source, QRegExp exp, int capNumber, QString stringBefore, QString stringAfter)
{
    if(source.contains(exp) == true)
    {
        return stringBefore + exp.cap(capNumber) + stringAfter;
    }
    else
    {
        return "";
    }
}
