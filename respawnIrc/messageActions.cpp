#include <QMessageBox>
#include <QNetworkCookieJar>
#include <QUrl>
#include <QList>
#include <QPair>

#include "messageActions.hpp"
#include "utilityTool.hpp"
#include "logTool.hpp"

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

/* `urlForDeletion` vient directement du payload de la page (actions.delete.url du
 * message) : elle embarque l'identifiant du message et le jeton attendu par le site,
 * qui n'est pas celui utilisé ailleurs. */
void messageActionsClass::deleteMessage(QString urlForDeletion)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookie(currentConnectCookie, websiteOfCookie);
    }

    if(urlForDeletion.isEmpty() == false && replyForDeleteInfo == nullptr)
    {
        QNetworkRequest requestForDeleteInfo = parsingTool::buildRequestWithThisUrl(urlForDeletion);
        requestForDeleteInfo.setRawHeader("Accept", "application/json");
        requestForDeleteInfo.setRawHeader("X-Requested-With", "XMLHttpRequest");
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
    int httpStatus = replyForDeleteInfo->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    timeoutForDeleteInfo->resetReply();

    if(replyForDeleteInfo->isReadable())
    {
        source = replyForDeleteInfo->readAll();
    }
    replyForDeleteInfo->deleteLater();

    replyForDeleteInfo = nullptr;

    qDebug(logNetwork) << "Réponse à la suppression d'un message - code" << httpStatus << "- corps :" << source.left(1000);

    QString error = parsingTool::getErrorOfMessageSending(source, httpStatus);

    if(error.isEmpty() == false)
    {
        qWarning(logNetwork) << "Suppression refusée :" << error;
        QMessageBox::warning(parent, "Erreur", error);
    }
}
