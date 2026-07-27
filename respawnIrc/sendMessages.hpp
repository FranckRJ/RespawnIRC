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
    void setInfoForEditMessage(long idOfMessageEdit, QString messageEdit, QString error,
                               QList<QPair<QString, QString>> listOfFieldForEdit, bool useMessageEdit);
private:
    QList<QPair<QString, QString>> buildListOfFieldsForMessage(const QString& topicLink,
                                                               const QList<QPair<QString, QString>>& listOfInput) const;
private slots:
    void deleteReplyForSendMessage();
signals:
    void needToPostMessage();
    void needToSetEditMessage(long idOfLastMessage, bool useMessageEdit);
    void needToGetMessages();
private:
    multiTypeTextBoxClass* messageLine;
    QPushButton* sendButton;
    QNetworkReply* replyForSendMessage = nullptr;
    QNetworkAccessManager* networkManager;
    QNetworkCookie connectCookieForPostMsg;
    bool isInEdit = false;
    bool inSending = false;
    bool changeColorOnEdit = false;
    QList<QPair<QString, QString>> listOfFieldForEditLastMessage;
    long idOfLastMessageEdit = 0;
    int nbOfMessagesSend = 0;
};

#endif
