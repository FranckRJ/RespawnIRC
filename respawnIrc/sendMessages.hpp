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
    explicit sendMessagesClass(QWidget* parent = nullptr);
    void doStuffBeforeQuit();
    void postMessage(QString pseudoUsed, QString topicLink, const QNetworkCookie& connectCookie, const QList<QPair<QString, QString>>& listOfInput);
    void clearMessageLine();
    void settingsChanged();
    void styleChanged();
    multiTypeTextBoxClass* getMessageLine();
    bool getIsSending() const;
    bool getIsInEdit() const;
    int getNbOfMessagesSend() const;
    void setIsInEdit(bool newVal);
    void setEnableSendButton(bool newVal);
    void setMultilineEdit(bool newVal);
public slots:
    void quoteThisMessage(QString messageToQuote);
    void setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString error, QString infoToSend, bool useMessageEdit);
private:
    QString buildDataWithThisListOfInput(const QList<QPair<QString, QString>>& listOfInput) const;
private slots:
    void deleteReplyForSendMessage();
signals:
    void needToPostMessage();
    void needToSetEditMessage(int idOfLastMessage, bool useMessageEdit);
    void needToGetMessages();
private:
    multiTypeTextBoxClass* messageLine;
    QPushButton* sendButton;
    QNetworkReply* replyForSendMessage = nullptr;
    QNetworkAccessManager* networkManager;
    QNetworkCookie connectCookieForPostMsg;
    QString dataForEditLastMessage;
    bool isInEdit = false;
    bool inSending = false;
    bool changeColorOnEdit = false;
    int idOfLastMessageEdit = 0;
    int nbOfMessagesSend = 0;
};

#endif
