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
    timeoutForEditInfo = new autoTimeoutReplyClass(0, this);
    timeoutForDeleteInfo = new autoTimeoutReplyClass(0, this);
}

void messageActionsClass::updateSettingInfo()
{
    timeoutForEditInfo->updateTimeoutTime();
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

/* `urlForFormValues` est l'URL d'édition donnée par le site pour ce message. On y lit
 * le texte à modifier et la session de formulaire à renvoyer, sans quoi l'envoi de la
 * modification est refusé. */
bool messageActionsClass::getEditInfo(long idOfMessageToEdit, QString urlForFormValues, bool useMessageEdit)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookie(currentConnectCookie, websiteOfCookie);
    }

    if(urlForFormValues.isEmpty() == true || replyForEditInfo != nullptr)
    {
        return false;
    }

    oldIdOfMessageToEdit = idOfMessageToEdit;
    oldUseMessageEdit = useMessageEdit;

    QNetworkRequest requestForEditInfo = parsingTool::buildRequestWithThisUrl(urlForFormValues);
    requestForEditInfo.setRawHeader("Accept", "application/json");
    requestForEditInfo.setRawHeader("X-Requested-With", "XMLHttpRequest");
    replyForEditInfo = timeoutForEditInfo->resetReply(networkManager->get(requestForEditInfo));

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

void messageActionsClass::analyzeEditInfo()
{
    QString source;
    int httpStatus = replyForEditInfo->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    timeoutForEditInfo->resetReply();

    if(replyForEditInfo->isReadable() == true)
    {
        source = replyForEditInfo->readAll();
    }
    replyForEditInfo->deleteLater();
    replyForEditInfo = nullptr;

    qDebug(logNetwork) << "Réponse à la demande d'édition - code" << httpStatus << "- corps :" << source.left(1000);

    editFormValuesStruct formValues = parsingTool::getEditFormValues(source);

    if(formValues.isValid == false)
    {
        QString error = parsingTool::getErrorOfMessageSending(source, httpStatus);

        emit setEditInfo(oldIdOfMessageToEdit, "", (error.isEmpty() == true ? "Impossible d'éditer ce message." : error),
                         QList<QPair<QString, QString>>(), oldUseMessageEdit);
        return;
    }

    if(formValues.needsCaptcha == true)
    {
        emit setEditInfo(oldIdOfMessageToEdit, "", "Jeuxvideo.com demande un captcha pour modifier ce message, ce que RespawnIRC ne sait pas faire.",
                         QList<QPair<QString, QString>>(), oldUseMessageEdit);
        return;
    }

    emit setEditInfo(oldIdOfMessageToEdit, formValues.text, "", formValues.listOfField, oldUseMessageEdit);
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
