#ifndef SENDMESSAGES_HPP
#define SENDMESSAGES_HPP

#include <QtWidgets>
#include <QtNetwork>

#include "multiTypeTextBox.hpp"

class sendMessagesClass : public QWidget
{
    Q_OBJECT
public:
    sendMessagesClass(QWidget* parent = 0);
    QString buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput);
    void clearMessageLine();
    void settingsChanged();
    void styleChanged();
    multiTypeTextBoxClass* getMessageLine();
    bool getIsSending();
    bool getIsInEdit();
    void setIsInEdit(bool newVal);
    void setEnableSendButton(bool newVal);
    void setTextSendButton(QString newText);
public slots:
    void quoteThisMessage(QString messageToQuote);
    void postMessage(QString pseudoUsed, QString topicLink, const QList<QNetworkCookie>& listOfCookies, const QList<QPair<QString, QString> >& listOfInput);
    void deleteReplyForSendMessage();
    void setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString infoToSend, bool useMessageEdit);
    void setMultilineEdit(bool newVal);
signals:
    void needToPostMessage();
    void needToSetEditMessage(int idOfLastMessage, bool useMessageEdit);
    void needToGetMessages();
private:
    multiTypeTextBoxClass messageLine;
    QPushButton sendButton;
    QNetworkReply* replyForSendMessage;
    QNetworkAccessManager* networkManager;
    QList<QNetworkCookie> cookieListForPostMsg;
    QString dataForEditLastMessage;
    bool isInEdit;
    bool inSending;
    int idOfLastMessageEdit;
};

#endif
