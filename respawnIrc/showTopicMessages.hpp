#ifndef SHOWTOPICMESSAGES_HPP
#define SHOWTOPICMESSAGES_HPP

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

class showTopicMessagesClass : public QWidget
{
    Q_OBJECT
public:
    showTopicMessagesClass(QWidget* parent = 0);
    void startGetMessage();
    const QList<QPair<QString, QString> >& getListOfInput();
    QString getTopicLink();
    QString getTopicName();
    QString getCaptchaLink();
    QString getMessagesStatus();
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser);
    void setNewTopic(QString newTopic);
    void setMessageStatus(QString newStatus);
public slots:
    void getMessages();
    void analyzeMessages();
signals:
    void newMessageStatus();
    void newMessagesAvailable();
    void newNameForTopic(QString newName);
private:
    QTextBrowser messagesBox;
    QNetworkReply* reply;
    QNetworkAccessManager networkManager;
    QTimer timerForGetMessage;
    QList<QPair<QString, QString> > listOfInput;
    QString messagesStatus;
    QString topicLink;
    QString topicName;
    QString pseudoOfUser;
    QString captchaLink;
    bool firstTimeGetMessages;
    bool retrievesMessage;
    bool linkHasChanged;
    int idOfLastMessage;
};

#endif
