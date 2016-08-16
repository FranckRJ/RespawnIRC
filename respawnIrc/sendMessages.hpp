#ifndef SENDMESSAGES_HPP
#define SENDMESSAGES_HPP

#include <QWidget>
#include <QNetworkCookie>
#include <QPushButton>
#include <QList>
#include <QString>
#include <QPair>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "multiTypeTextBox.hpp"

class sendMessagesClass : public QWidget
{
    Q_OBJECT
public:
    explicit sendMessagesClass(QWidget* parent = 0);
    ~sendMessagesClass();
    QString buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput);
    void clearMessageLine();
    void settingsChanged();
    void styleChanged();
    multiTypeTextBoxClass* getMessageLine();
    bool getIsSending();
    bool getIsInEdit();
    int getNbOfMessagesSend();
    void setIsInEdit(bool newVal);
    void setEnableSendButton(bool newVal);
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
    QNetworkReply* replyForSendMessage = nullptr;
    QNetworkAccessManager* networkManager;
    QList<QNetworkCookie> cookieListForPostMsg;
    QString dataForEditLastMessage;
    bool isInEdit = false;
    bool inSending = false;
    bool changeColorOnEdit = false;
    int idOfLastMessageEdit = 0;
    int nbOfMessagesSend = 0;
};

#endif
