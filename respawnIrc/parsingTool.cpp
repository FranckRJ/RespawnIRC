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

    if(source.contains(expForCaptcha) == true)
    {
        return expForCaptcha.cap(1);
    }
    else
    {
        return "";
    }
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

    if(source.contains(expForNameOfTopic) == true)
    {
        return expForNameOfTopic.cap(1);
    }
    else
    {
        return "";
    }
}

QList<int> parsingToolClass::getListOfMessageID(const QString& source)
{
    QList<int> listOfMessageIDInNumber;
    QList<QString> listOfMessageIDInString;
    QRegExp expForMessageID("<div class=\"bloc-message-forum \" id=\"post_[^\"]*\" data-id=\"([^\"]*)\">");

    listOfMessageIDInString = getListOfThisCapNumber(source, expForMessageID, 1);

    for(int i = 0; i < listOfMessageIDInString.size(); ++i)
    {
        listOfMessageIDInNumber.push_back(listOfMessageIDInString.at(i).toInt());
    }

    return listOfMessageIDInNumber;
}

QList<QString> parsingToolClass::getListOfPseudo(const QString& source)
{
    QRegExp expForPseudo("<span class=\"JvCare [^ ]* bloc-pseudo-msg text-[^\"]*\" target=\"_blank\">[^a-zA-Z0-9_\\[\\]-]*([a-zA-Z0-9_\\[\\]-]*)[^<]*</span>");

    return getListOfThisCapNumber(source, expForPseudo, 1);
}

QList<QString> parsingToolClass::getListOfDate(const QString& source)
{
    QRegExp expForDate("<div class=\"bloc-date-msg\">[^<]*<span class=\"JvCare [^ ]* lien-jv\" target=\"_blank\">[^ ]* [^ ]* [^ ]* [^ ]* ([0-9:]*)[^<]*</span>");

    return getListOfThisCapNumber(source, expForDate, 1);
}

QList<QString> parsingToolClass::getListOfMessage(const QString& source)
{
    QList<QString> listOfMessage;
    QRegExp expForMessage("<div class=\"bloc-contenu\"><div class=\"txt-msg  text-enrichi-forum \">(.*)</div>");
    expForMessage.setMinimal(true);

    listOfMessage = getListOfThisCapNumber(source, expForMessage, 1);

    for(int i = 0; i < listOfMessage.size(); ++i)
    {
        listOfMessage[i] = parsingMessages(listOfMessage.at(i));
    }

    return listOfMessage;
}

QString parsingToolClass::parsingMessages(QString thisMessage)
{
    QRegExp expForSmiley("<img src=\"//image.jeuxvideo.com/smileys_img/([^\"]*)\" alt=\"[^\"]*\" data-def=\"SMILEYS\" data-code=\"([^\"]*)\" title=\"[^\"]*\" />");
    QRegExp expForStickers("<img class=\"img-stickers\" src=\"([^\"]*)\"/>");
    QRegExp expForLongLink("<span class=\"JvCare [^\"]*\" rel=\"nofollow\" target=\"_blank\" title=\"([^\"]*)\">[^<]*<i></i><span>[^<]*</span>[^<]*</span>");
    QRegExp secExpForLongLink("<span class=\"JvCare [^\"]*\" title=\"([^\"]*)\">[^<]*<i></i><span>[^<]*</span>[^<]*</span>");
    QRegExp expForShortLink("<span class=\"JvCare [^\"]*\" rel=\"nofollow\" target=\"_blank\">([^<]*)</span>");
    QRegExp expForNoelshack("<a href=\"([^\"]*)\" data-def=\"NOELSHACK\" target=\"_blank\"><img class=\"img-shack\" [^>]*></a>");
    QRegExp expForSpoilLine("<span class=\"bloc-spoil-jv en-ligne\"><span class=\"contenu-spoil\">([^<]*)</span></span>");
    QRegExp expForSpoilBlock("<span class=\"bloc-spoil-jv\"><span class=\"contenu-spoil\">([^<]*)</span></span>");
    QRegExp expForAllJVCare("<span class=\"JvCare [^\"]*\">([^<]*)</span>");

    thisMessage.replace("\n", "");

    replaceWithCapNumber(thisMessage, expForSmiley, 1, false, "<img src=\"smileys/", "\" />");
    replaceWithCapNumber(thisMessage, expForStickers, 1, true);
    replaceWithCapNumber(thisMessage, expForLongLink, 1, true); //je sais pas si c'est utile
    replaceWithCapNumber(thisMessage, secExpForLongLink, 1, true);
    replaceWithCapNumber(thisMessage, expForShortLink, 1, true);
    replaceWithCapNumber(thisMessage, expForNoelshack, 1, true);
    replaceWithCapNumber(thisMessage, expForSpoilLine, 1, false, "<span style=\"color: black; background-color: black;\">", "</span>");
    replaceWithCapNumber(thisMessage, expForSpoilBlock, 1, false, "<br /><br /><span style=\"color: black; background-color: black;\">", "</span>");
    replaceWithCapNumber(thisMessage, expForAllJVCare, 1);

    thisMessage.replace("<blockquote class=\"blockquote-jv\">", "<table border=\"1\" cellspacing=\"0\" cellpadding=\"5\"><tr><td>");
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

QList<QString> parsingToolClass::getListOfThisCapNumber(const QString& source, QRegExp exp, int capNumber)
{
    QList<QString> listOfString;
    int posForExp = 0;
    while((posForExp = exp.indexIn(source, posForExp)) != -1)
    {
        listOfString.push_back(exp.cap(capNumber));
        posForExp += exp.matchedLength();
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
