#include "sendMessages.hpp"
#include "parsingTool.hpp"

sendMessagesClass::sendMessagesClass(QWidget* parent) : QWidget(parent)
{
    sendButton.setText("Envoyer");
    sendButton.setAutoDefault(true);
    sendButton.setObjectName("sendButton");

    networkManager = new QNetworkAccessManager(this);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(&messageLine, 1);
    layout->addWidget(&sendButton);
    layout->setMargin(0);

    setLayout(layout);

    QObject::connect(&sendButton, &QPushButton::pressed, this, &sendMessagesClass::needToPostMessage);
    QObject::connect(&messageLine, &multiTypeTextBoxClass::returnPressed, &sendButton, &QPushButton::click);
}

QString sendMessagesClass::buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput)
{
    QString data;

    for(int i = 0; i < listOfInput.size(); ++i)
    {
        data += listOfInput.at(i).first + "=" + listOfInput.at(i).second + "&";
    }

    data += "message_topic=" + QUrl::toPercentEncoding(messageLine.text());

    data += "&form_alias_rang=1";

    return data;
}

void sendMessagesClass::clearMessageLine()
{
    messageLine.clear();
}

void sendMessagesClass::settingsChanged()
{
    messageLine.settingsChanged();
}

void sendMessagesClass::styleChanged()
{
    messageLine.styleChanged();
}

multiTypeTextBoxClass* sendMessagesClass::getMessageLine()
{
    return &messageLine;
}

bool sendMessagesClass::getIsSending()
{
    return inSending;
}

bool sendMessagesClass::getIsInEdit()
{
    return isInEdit;
}

void sendMessagesClass::setIsInEdit(bool newVal)
{
    isInEdit = newVal;
}

void sendMessagesClass::setEnableSendButton(bool newVal)
{
    sendButton.setEnabled(newVal);
}

void sendMessagesClass::setTextSendButton(QString newText)
{
    sendButton.setText(newText);
}

void sendMessagesClass::quoteThisMessage(QString messageToQuote)
{
    if(messageLine.text().isEmpty() == false)
    {
        messageLine.insertText("\n");
    }
    messageLine.insertText(messageToQuote);
    messageLine.insertText("\n\n");
    messageLine.setFocus();
}

void sendMessagesClass::postMessage(QString pseudoUsed, QString topicLink, const QList<QNetworkCookie>& listOfCookies, const QList<QPair<QString, QString> >& listOfInput)
{
    if(networkManager == 0)
    {
        networkManager = new QNetworkAccessManager(this);
    }

    if(replyForSendMessage == 0 && pseudoUsed.isEmpty() == false && topicLink.isEmpty() == false)
    {
        QNetworkRequest request;
        QString data;

        cookieListForPostMsg = listOfCookies;
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(cookieListForPostMsg, QUrl("http://www.jeuxvideo.com"));

        if(isInEdit == true)
        {
            request = parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com/forums/ajax_edit_message.php");
        }
        else
        {
            request = parsingToolClass::buildRequestWithThisUrl(topicLink);
        }

        sendButton.setEnabled(false);
        inSending = true;

        if(isInEdit == false)
        {
            data = buildDataWithThisListOfInput(listOfInput);
        }
        else
        {
            data = "message_topic=" + QUrl::toPercentEncoding(messageLine.text());
            data += "&" + dataForEditLastMessage;
        }

        replyForSendMessage = networkManager->post(request, data.toLatin1());

        if(replyForSendMessage->isOpen() == true)
        {
            QObject::connect(replyForSendMessage, &QNetworkReply::finished, this, &sendMessagesClass::deleteReplyForSendMessage);
        }
        else
        {
            deleteReplyForSendMessage();
            networkManager->deleteLater();
            networkManager = 0;
        }

    }
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
    replyForSendMessage = 0;

    if(source.size() == 0 || (isInEdit == true && source.startsWith("{\"erreur\":[]") == true))
    {
        messageLine.clear();
    }
    else if(source.contains("<div class=\"alert-row\"> Le captcha est invalide. </div>") == true ||
            (isInEdit == true && source.startsWith("{\"erreur\":[\"Le captcha est incorrect.\"]") == true))
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Depuis la mise à jour de JVC les captchas ne sont plus supportés, "
                                           "veuillez attendre quelques secondes avant d'envoyer votre message.");
        dontEraseEditMessage = true;
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", parsingToolClass::getErrorMessage(source));
        dontEraseEditMessage = true;
    }

    sendButton.setEnabled(true);
    inSending = false;

    if(isInEdit == true)
    {
        sendButton.setText("Envoyer");
        isInEdit = false;

        if(dontEraseEditMessage == true)
        {
            emit needToSetEditMessage(idOfLastMessageEdit, false);
        }
    }

    emit needToGetMessages();
    messageLine.setFocus();
}

void sendMessagesClass::setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString infoToSend, bool useMessageEdit)
{
    if(messageEdit.isEmpty() == false)
    {
        if(useMessageEdit == true)
        {
            messageLine.clear();
            messageLine.insertText(messageEdit);
        }
        dataForEditLastMessage = "id_message=" + QString::number(idOfMessageEdit) + "&" + infoToSend;
        isInEdit = true;
        idOfLastMessageEdit = idOfMessageEdit;
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Impossible d'éditer ce message.");
        sendButton.setText("Envoyer");
    }

    sendButton.setEnabled(true);
}

void sendMessagesClass::setMultilineEdit(bool newVal)
{
    messageLine.setTextEditSelected(newVal);
    messageLine.setFocus();
}
