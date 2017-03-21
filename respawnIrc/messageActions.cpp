#include <QMessageBox>
#include <QNetworkCookieJar>
#include <QUrl>

#include "messageActions.hpp"

messageActionsClass::messageActionsClass(QWidget* newParent) : QObject(newParent)
{
    parent = newParent;
    networkManager = new QNetworkAccessManager(this);
}

void messageActionsClass::updateSettingInfo()
{
    timeoutForEditInfo.updateTimeoutTime();
    timeoutForQuoteInfo.updateTimeoutTime();
    timeoutForDeleteInfo.updateTimeoutTime();
}

void messageActionsClass::setNewTopic(QString newTopicLink)
{
    websiteOfTopic = parsingToolClass::getWebsite(newTopicLink);
    oldIdOfMessageToEdit = 0;
    oldUseMessageEdit = false;
}

void messageActionsClass::setNewAjaxInfo(ajaxInfoStruct newAjaxInfo)
{
    ajaxInfo = newAjaxInfo;
}

void messageActionsClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newWebsiteOfCookies)
{
    currentCookieList = newCookies;
    websiteOfCookies = newWebsiteOfCookies;

    if(networkManager != nullptr)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://" + websiteOfCookies));
    }
}

const QList<QNetworkCookie>& messageActionsClass::getCookieList() const
{
    return currentCookieList;
}

bool messageActionsClass::getEditInfo(long idOfMessageToEdit, bool useMessageEdit)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, websiteOfCookies);
    }

    if(ajaxInfo.list.isEmpty() == false)
    {
        if(replyForEditInfo == nullptr)
        {
            QString urlToGet;
            QNetworkRequest requestForEditInfo;

            oldIdOfMessageToEdit = idOfMessageToEdit;
            urlToGet = "http://" + websiteOfTopic + "/forums/ajax_edit_message.php?id_message=" + QString::number(oldIdOfMessageToEdit) + "&" + ajaxInfo.list + "&action=get";
            requestForEditInfo = parsingToolClass::buildRequestWithThisUrl(urlToGet);
            oldAjaxInfo = ajaxInfo;
            ajaxInfo.list.clear();
            oldUseMessageEdit = useMessageEdit;
            replyForEditInfo = timeoutForEditInfo.resetReply(networkManager->get(requestForEditInfo));

            if(replyForEditInfo->isOpen() == true)
            {
                connect(replyForEditInfo, &QNetworkReply::finished, this, &messageActionsClass::analyzeEditInfo);
            }
            else
            {
                analyzeEditInfo();
                networkManager->deleteLater();
                networkManager = nullptr;
            }

            return true;
        }
    }

    return false;
}

void messageActionsClass::getQuoteInfo(QString idOfMessageQuoted, QString messageQuoted)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, websiteOfCookies);
    }

    if(ajaxInfo.list.isEmpty() == false && replyForQuoteInfo == nullptr)
    {
        QNetworkRequest requestForQuoteInfo = parsingToolClass::buildRequestWithThisUrl("http://" + websiteOfTopic + "/forums/ajax_citation.php");
        QString dataForQuote = "id_message=" + idOfMessageQuoted + "&" + ajaxInfo.list;
        lastMessageQuoted = messageQuoted;
        replyForQuoteInfo = timeoutForQuoteInfo.resetReply(networkManager->post(requestForQuoteInfo, dataForQuote.toLatin1()));

        if(replyForQuoteInfo->isOpen() == true)
        {
            connect(replyForQuoteInfo, &QNetworkReply::finished, this, &messageActionsClass::analyzeQuoteInfo);
        }
        else
        {
            analyzeQuoteInfo();
            networkManager->deleteLater();
            networkManager = nullptr;
        }
    }
    else
    {
        QMessageBox::warning(parent, "Erreur", "Erreur, impossible de citer ce message, réessayez.");
    }
}

void messageActionsClass::deleteMessage(QString idOfMessageDeleted)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, websiteOfCookies);
    }

    if(ajaxInfo.mod.isEmpty() == false && replyForDeleteInfo == nullptr)
    {
        QNetworkRequest requestForDeleteInfo = parsingToolClass::buildRequestWithThisUrl("http://" + websiteOfTopic + "/forums/modal_del_message.php?tab_message[]=" + idOfMessageDeleted + "&type=delete&" + ajaxInfo.mod);
        replyForDeleteInfo = timeoutForDeleteInfo.resetReply(networkManager->get(requestForDeleteInfo));

        if(replyForDeleteInfo->isOpen() == true)
        {
            connect(replyForDeleteInfo, &QNetworkReply::finished, this, &messageActionsClass::analyzeDeleteInfo);
        }
        else
        {
            analyzeDeleteInfo();
            networkManager->deleteLater();
            networkManager = nullptr;
        }
    }
    else
    {
        QMessageBox::warning(parent, "Erreur", "Erreur, impossible de supprimer ce message, réessayez.");
    }
}

void messageActionsClass::analyzeEditInfo()
{
    QString message;
    QString dataToSend = oldAjaxInfo.list + "&action=post";
    QList<QPair<QString, QString>> listOfEditInput;
    QString source;

    timeoutForEditInfo.resetReply();

    if(replyForEditInfo->isReadable())
    {
        source = replyForEditInfo->readAll();
    }
    replyForEditInfo->deleteLater();

    source = parsingToolClass::parsingAjaxMessages(source);
    message = parsingToolClass::getMessageEdit(source);
    parsingToolClass::getListOfHiddenInputFromThisForm(source, "form-post-topic", listOfEditInput);

    for(const QPair<QString, QString>& thisInput : listOfEditInput)
    {
        dataToSend += "&" + thisInput.first + "=" + thisInput.second;
    }

    emit setEditInfo(oldIdOfMessageToEdit, message, dataToSend, oldUseMessageEdit);

    replyForEditInfo = nullptr;
}

void messageActionsClass::analyzeQuoteInfo()
{
    QString messageQuote;
    QString source;

    timeoutForQuoteInfo.resetReply();

    if(replyForQuoteInfo->isReadable())
    {
        source = replyForQuoteInfo->readAll();
    }
    replyForQuoteInfo->deleteLater();

    messageQuote = parsingToolClass::getMessageQuote(source);

    messageQuote = ">" + QUrl::fromPercentEncoding(lastMessageQuoted.toUtf8()) + "\n>" + messageQuote;
    replyForQuoteInfo = nullptr;

    emit quoteThisMessage(messageQuote);
}

void messageActionsClass::analyzeDeleteInfo()
{
    QString source;

    timeoutForDeleteInfo.resetReply();

    if(replyForDeleteInfo->isReadable())
    {
        source = replyForDeleteInfo->readAll();
    }
    replyForDeleteInfo->deleteLater();

    replyForDeleteInfo = nullptr;

    if(source.startsWith("{\"erreur\":[]}") == false)
    {
        source.remove(0, source.indexOf("[") + 2);
        source.remove(source.lastIndexOf("]") - 1, 3);
        QMessageBox::warning(parent, "Erreur", source);
    }
}
