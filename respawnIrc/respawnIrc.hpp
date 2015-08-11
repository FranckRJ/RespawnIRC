#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

class respawnIrcClass : public QWidget
{
    Q_OBJECT
public:
    respawnIrcClass(QWidget* parent = 0);
public slots:
    void showConnect();
    void showSelectTopic();
    void setNewCookies(QList<QNetworkCookie> newCookies);
    void setNewTopic(QString newTopic);
    void getMessages();
    void analyzeMessages();
    void postMessage();
    void deleteReplyForSendMessage();
protected:
    void focusInEvent(QFocusEvent * event);
private:
    QTextBrowser messagesBox;
    QLineEdit messageLine;
    QNetworkReply* reply;
    QNetworkReply* replyForSendMessage;
    QNetworkAccessManager networkManager;
    QTimer timerForGetMessage;
    QList<QPair<QString, QString> > listOfInput;
    QLabel messagesStatus;
    QString topicLink;
    bool isConnected;
    bool firstTimeGetMessages;
    int idOfLastMessage;
};

#endif
