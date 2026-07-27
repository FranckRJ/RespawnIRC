#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QUrl>

#include "sendMessages.hpp"
#include "logTool.hpp"
#include "parsingTool.hpp"
#include "shortcutTool.hpp"
#include "settingTool.hpp"
#include "utilityTool.hpp"

sendMessagesClass::sendMessagesClass(QWidget* parent) : QWidget(parent)
{
    messageLine = new multiTypeTextBoxClass(this);
    sendButton = new QPushButton(this);

    sendButton->setObjectName("sendButton");

    sendButton->setText("Envoyer");
    sendButton->setAutoDefault(true);

    changeColorOnEdit = settingTool::getThisBoolOption("changeColorOnEdit");
    networkManager = new QNetworkAccessManager(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(messageLine, 1);
    layout->addWidget(sendButton);
    layout->setMargin(0);

    setLayout(layout);

    connect(sendButton, &QPushButton::clicked, this, &sendMessagesClass::needToPostMessage);
    connect(messageLine, &multiTypeTextBoxClass::returnPressed, sendButton, &QPushButton::click);
}

void sendMessagesClass::doStuffBeforeQuit()
{
    settingTool::saveThisOption("nbOfMessagesSend",
                                     settingTool::getThisIntOption("nbOfMessagesSend").value + nbOfMessagesSend);
    messageLine->doStuffBeforeQuit();
}

void sendMessagesClass::postMessage(QString pseudoUsed, QString topicLink, const QNetworkCookie& connectCookie, const QList<QPair<QString, QString>>& listOfInput)
{
    QString website = parsingTool::getWebsite(topicLink);

    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
    }

    if(website == "www.forumjv.com")
    {
        QMessageBox::warning(this, "Erreur", "Impossible de poster sur un topic ForumJV pour le moment.");
        return;
    }
    if(pseudoUsed.isEmpty() == true)
    {
        QMessageBox::warning(this, "Erreur", "Impossible de poster le message, vous n'êtes pas connecté.");
        return;
    }
    if(topicLink.isEmpty() == true)
    {
        QMessageBox::warning(this, "Erreur", "Impossible de poster le message, le topic est invalide.");
        return;
    }
    if(listOfInput.isEmpty() == true)
    {
        QMessageBox::warning(this, "Erreur", "Impossible de poster le message, veuillez réessayer plus tard.\n"
                                             "Si le problème persiste, redémarrez RespawnIRC ou supprimez le pseudo de la liste des comptes et ajoutez-le à nouveau.");
        return;
    }
    else
    {
        if(listOfInput.first().first == "locked" && listOfInput.first().second == "true")
        {
            QMessageBox::warning(this, "Erreur", "Impossible de poster le message, le topic est fermé.");
            return;
        }
    }

    if(replyForSendMessage == nullptr && pseudoUsed.isEmpty() == false && topicLink.isEmpty() == false && sendButton->isEnabled() == true)
    {
        QNetworkRequest request;

        connectCookieForPostMsg = connectCookie;
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(utilityTool::cookieToCookieList(connectCookieForPostMsg), QUrl("https://" + website));

        /* Depuis la refonte 2026 de jeuxvideo.com, on ne poste plus sur l'URL du topic
         * avec un corps urlencodé, mais sur /forums/message/add (ou /edit) avec un
         * formulaire multipart. */
        if(isInEdit == true)
        {
            request = parsingTool::buildRequestWithThisUrl("https://" + website + "/forums/message/edit");
        }
        else
        {
            request = parsingTool::buildRequestWithThisUrl("https://" + website + "/forums/message/add");
        }

        sendButton->setEnabled(false);
        inSending = true;

        QList<QPair<QString, QString>> listOfField =
            buildListOfFieldsForMessage(topicLink, (isInEdit == true ? listOfFieldForEditLastMessage : listOfInput));
        QByteArray boundaryUsed;
        QByteArray data = parsingTool::buildMultipartFormData(listOfField, boundaryUsed);

        /* Les valeurs contiennent des jetons de session, on ne journalise que les noms
         * des champs (et la taille du texte) pour pouvoir comparer avec ce qu'attend le
         * site sans recopier de secret dans le fichier de log. */
        QStringList namesOfFields;
        for(const QPair<QString, QString>& thisField : listOfField)
        {
            namesOfFields.append(thisField.first + (thisField.first == "fs_version" || thisField.first == "group" ||
                                                    thisField.first == "messageId" || thisField.first == "topicId" ||
                                                    thisField.first == "forumId"
                                                    ? "=" + thisField.second
                                                    : "(" + QString::number(thisField.second.size()) + " car.)"));
        }
        qDebug(logNetwork) << "Envoi vers" << request.url().toDisplayString() << "- champs :" << namesOfFields.join(", ");

        request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=" + boundaryUsed);
        request.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
        request.setRawHeader("Accept", "application/json");
        request.setRawHeader("Accept-Language", "fr");
        request.setRawHeader("X-Requested-With", "XMLHttpRequest");

        replyForSendMessage = networkManager->post(request, data);

        if(replyForSendMessage->isOpen() == true)
        {
            connect(replyForSendMessage, &QNetworkReply::finished, this, &sendMessagesClass::deleteReplyForSendMessage);
        }
        else
        {
            deleteReplyForSendMessage();
            networkManager->deleteLater();
            networkManager = nullptr;
        }

    }
}

void sendMessagesClass::clearMessageLine()
{
    messageLine->clear();
}

void sendMessagesClass::settingsChanged()
{
    messageLine->settingsChanged();
    changeColorOnEdit = settingTool::getThisBoolOption("changeColorOnEdit");

    if(changeColorOnEdit == true)
    {
        messageLine->setEditMode(isInEdit);
    }
    else
    {
        messageLine->setEditMode(false);
    }
}

void sendMessagesClass::styleChanged()
{
    messageLine->styleChanged();
    if(changeColorOnEdit == true)
    {
        messageLine->setEditMode(isInEdit);
    }
}

multiTypeTextBoxClass* sendMessagesClass::getMessageLine()
{
    return messageLine;
}

bool sendMessagesClass::getIsSending() const
{
    return inSending;
}

bool sendMessagesClass::getIsInEdit() const
{
    return isInEdit;
}

int sendMessagesClass::getNbOfMessagesSend() const
{
    return nbOfMessagesSend;
}

void sendMessagesClass::setIsInEdit(bool newVal)
{
    isInEdit = newVal;

    if(isInEdit == true)
    {
        sendButton->setText("Editer");
    }
    else
    {
        sendButton->setText("Envoyer");
    }

    if(changeColorOnEdit == true)
    {
        messageLine->setEditMode(newVal);
    }
}

void sendMessagesClass::setEnableSendButton(bool newVal)
{
    sendButton->setEnabled(newVal);
}

void sendMessagesClass::setMultilineEdit(bool newVal)
{
    messageLine->setTextEditSelected(newVal);
    messageLine->setFocus();
}

void sendMessagesClass::quoteThisMessage(QString messageToQuote)
{
    if(messageLine->text().isEmpty() == false && messageLine->text().endsWith("\n\n") == false)
    {
        if(messageLine->text().endsWith("\n") == false)
        {
            messageLine->insertText("\n");
        }
        messageLine->insertText("\n");
    }
    messageLine->insertText(messageToQuote);
    messageLine->insertText("\n\n");
    messageLine->setFocus();
}

void sendMessagesClass::setInfoForEditMessage(long idOfMessageEdit, QString messageEdit, QString error,
                                              QList<QPair<QString, QString>> listOfFieldForEdit, bool useMessageEdit)
{
    if(messageEdit.isEmpty() == false)
    {
        if(useMessageEdit == true)
        {
            messageLine->clear();
            messageLine->insertText(messageEdit);
        }
        setIsInEdit(true);
        idOfLastMessageEdit = idOfMessageEdit;
        listOfFieldForEditLastMessage = listOfFieldForEdit;
    }
    else
    {
        QMessageBox::warning(this, "Erreur", error);
        setIsInEdit(false);
    }

    sendButton->setEnabled(true);
}

/* Champs attendus par /forums/message/add et /forums/message/edit : le texte, de quoi
 * situer le message, puis les jetons de session récupérés dans le payload de la page
 * (listOfInput). Pour une édition, `messageId` désigne le message modifié ; pour un
 * nouveau message JVC attend la chaîne « undefined ». */
QList<QPair<QString, QString>> sendMessagesClass::buildListOfFieldsForMessage(const QString& topicLink,
                                                                             const QList<QPair<QString, QString>>& listOfInput) const
{
    QList<QPair<QString, QString>> listOfField;

    listOfField.append(QPair<QString, QString>("text", shortcutTool::transformMessage(messageLine->text(), "shortcut")));
    listOfField.append(QPair<QString, QString>("topicId", parsingTool::getTopicIdOfThisTopic(topicLink)));
    listOfField.append(QPair<QString, QString>("forumId", parsingTool::getForumIdOfThisTopic(topicLink)));
    listOfField.append(QPair<QString, QString>("group", "1"));
    listOfField.append(QPair<QString, QString>("messageId", (isInEdit == true ? QString::number(idOfLastMessageEdit) : QString("undefined"))));

    listOfField.append(listOfInput);

    return listOfField;
}

void sendMessagesClass::deleteReplyForSendMessage()
{
    bool dontEraseEditMessage = false;
    QString source;
    int httpStatus = replyForSendMessage->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool replyIsUsable = (replyForSendMessage->isReadable() == true && replyForSendMessage->rawHeaderList().isEmpty() == false);

    if(replyIsUsable == true)
    {
        source = replyForSendMessage->readAll();
    }

    qDebug(logNetwork) << "Réponse à l'envoi d'un message (édition :" << isInEdit << ") - code" << httpStatus
                       << "- erreur réseau" << replyForSendMessage->errorString() << "- corps :" << source.left(2000);

    replyForSendMessage->deleteLater();
    replyForSendMessage = nullptr;

    /* La réponse est du JSON depuis la refonte 2026 : c'est son contenu qui dit si
     * l'envoi est passé, pas le simple fait qu'elle soit vide comme avant. */
    QString error = (replyIsUsable == true ? parsingTool::getErrorOfMessageSending(source, httpStatus)
                                           : "Le message n'a pas été envoyé, la réponse du site est illisible.");

    if(error.isEmpty() == true)
    {
        messageLine->clear();

        if(isInEdit == false)
        {
            ++nbOfMessagesSend;
        }
    }
    else if(error.contains("captcha") == true)
    {
        QMessageBox::warning(this, "Erreur", "Depuis la mise à jour de JVC les captchas ne sont plus supportés, "
                                           "veuillez attendre quelques secondes avant d'envoyer votre message.");
        dontEraseEditMessage = true;
    }
    else
    {
        qWarning(logNetwork) << "Envoi refusé :" << error;
        QMessageBox::warning(this, "Erreur", error);
        dontEraseEditMessage = true;
    }

    sendButton->setEnabled(true);
    inSending = false;

    if(isInEdit == true)
    {
        setIsInEdit(false);

        if(dontEraseEditMessage == true)
        {
            emit needToSetEditMessage(idOfLastMessageEdit, false);
        }
    }

    emit needToGetMessages();
    messageLine->setFocus();
}
