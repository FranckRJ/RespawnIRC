#include <QMessageBox>
#include <QNetworkCookieJar>
#include <QUrl>
#include <QList>
#include <QPair>

#include "messageActions.hpp"
#include "utilityTool.hpp"

messageActionsClass::messageActionsClass(QWidget* newParent) : QObject(newParent)
{
    parent = newParent;
    networkManager = new QNetworkAccessManager(this);
    timeoutForDeleteInfo = new autoTimeoutReplyClass(0, this);
}

void messageActionsClass::updateSettingInfo()
{
    timeoutForDeleteInfo->updateTimeoutTime();
}

void messageActionsClass::setNewTopic(QString newTopicLink)
{
    websiteOfTopic = parsingTool::getWebsite(newTopicLink);
}

void messageActionsClass::setNewAjaxInfo(ajaxInfoStruct newAjaxInfo)
{
    ajaxInfo = newAjaxInfo;
}

void messageActionsClass::setNewCookie(QNetworkCookie newConnectCookie, QString newWebsiteOfCookie)
{
    currentConnectCookie = newConnectCookie;
    websiteOfCookie = newWebsiteOfCookie;

    if(networkManager != nullptr)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(utilityTool::cookieToCookieList(newConnectCookie), QUrl("https://" + websiteOfCookie));
    }
}

const QNetworkCookie& messageActionsClass::getConnectCookie() const
{
    return currentConnectCookie;
}

void messageActionsClass::deleteMessage(QString idOfMessageDeleted)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookie(currentConnectCookie, websiteOfCookie);
    }

    if(ajaxInfo.mod.isEmpty() == false && replyForDeleteInfo == nullptr)
    {
        /* modal_del_message.php a disparu avec la refonte 2026, remplacé par
         * /forums/message/delete. */
        QNetworkRequest requestForDeleteInfo = parsingTool::buildRequestWithThisUrl("https://" + websiteOfTopic + "/forums/message/delete?ids=" + idOfMessageDeleted + "&type=delete&" + ajaxInfo.mod);
        replyForDeleteInfo = timeoutForDeleteInfo->resetReply(networkManager->get(requestForDeleteInfo));

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

void messageActionsClass::analyzeDeleteInfo()
{
    QString source;

    timeoutForDeleteInfo->resetReply();

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
