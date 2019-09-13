#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QUrl>

#include "sendMessages.hpp"
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
        QString data;

        connectCookieForPostMsg = connectCookie;
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(utilityTool::cookieToCookieList(connectCookieForPostMsg), QUrl("http://" + website));

        if(isInEdit == true)
        {
            request = parsingTool::buildRequestWithThisUrl("http://" + website + "/forums/ajax_edit_message.php");
        }
        else
        {
            request = parsingTool::buildRequestWithThisUrl(topicLink);
        }

        sendButton->setEnabled(false);
        inSending = true;

        if(isInEdit == false)
        {
            data = buildDataWithThisListOfInput(listOfInput);
        }
        else
        {
            data = "message_topic=" + QUrl::toPercentEncoding(shortcutTool::transformMessage(messageLine->text(), "shortcut"));
            data += "&" + dataForEditLastMessage;
        }

        replyForSendMessage = networkManager->post(request, data.toLatin1());

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

void sendMessagesClass::setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString error, QString infoToSend, bool useMessageEdit)
{
    if(messageEdit.isEmpty() == false)
    {
        if(useMessageEdit == true)
        {
            messageLine->clear();
            messageLine->insertText(messageEdit);
        }
        dataForEditLastMessage = "id_message=" + QString::number(idOfMessageEdit) + "&" + infoToSend;
        setIsInEdit(true);
        idOfLastMessageEdit = idOfMessageEdit;
    }
    else
    {
        QMessageBox::warning(this, "Erreur", error);
        setIsInEdit(false);
    }

    sendButton->setEnabled(true);
}

QString sendMessagesClass::buildDataWithThisListOfInput(const QList<QPair<QString, QString>>& listOfInput) const
{
    QString data;

    for(const QPair<QString, QString>& thisInput : listOfInput)
    {
        data += thisInput.first + "=" + thisInput.second + "&";
    }

    data += "message_topic=" + QUrl::toPercentEncoding(shortcutTool::transformMessage(messageLine->text(), "shortcut"));

    data += "&form_alias_rang=1";

    return data;
}

void sendMessagesClass::deleteReplyForSendMessage()
{
    bool dontEraseEditMessage = false;
    QString source;
    if(replyForSendMessage->isReadable() == true)
    {
        if(replyForSendMessage->rawHeaderList().isEmpty() == false)
        {
            source = replyForSendMessage->readAll();
        }
        else
        {
            source = "weshgrotavu";
        }
    }
    else
    {
        source = "lolmdr";
    }
    replyForSendMessage->deleteLater();
    replyForSendMessage = nullptr;

    if(source.isEmpty() == true || source.contains("<meta http-equiv=\"refresh\"") == true || (isInEdit == true && source.startsWith("{\"erreur\":[]") == true))
    {
        messageLine->clear();

        if(isInEdit == false)
        {
            ++nbOfMessagesSend;
        }
    }
    else if(parsingTool::getErrorMessage(source, "").contains("captcha") == true ||
            (isInEdit == true && parsingTool::getErrorMessageInJSON(source, true, "").contains("captcha") == true))
    {
        QMessageBox::warning(this, "Erreur", "Depuis la mise à jour de JVC les captchas ne sont plus supportés, "
                                           "veuillez attendre quelques secondes avant d'envoyer votre message.");
        dontEraseEditMessage = true;
    }
    else
    {
        if(isInEdit == true)
        {
            QMessageBox::warning(this, "Erreur", parsingTool::getErrorMessageInJSON(source));
        }
        else
        {
            QMessageBox::warning(this, "Erreur", parsingTool::getErrorMessage(source));
        }
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
