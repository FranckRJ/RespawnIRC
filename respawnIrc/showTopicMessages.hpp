#ifndef SHOWTOPICMESSAGES_HPP
#define SHOWTOPICMESSAGES_HPP

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

class showTopicMessagesClass : public QWidget
{
    Q_OBJECT
public:
    showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QWidget* parent = 0);
    void startGetMessage();
    const QList<QPair<QString, QString> >& getListOfInput();
    QString getTopicLink();
    QString getTopicName();
    QString getCaptchaLink();
    QString getMessagesStatus();
    QString getNumberOfConnected();
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser);
    void setNewTopic(QString newTopic);
    void setMessageStatus(QString newStatus);
    void setNumberOfConnected(QString newNumber, bool forceSet = false);
    void setTopicToErrorMode();
public slots:
    void getMessages();
    void analyzeMessages();
signals:
    void newMessageStatus();
    void newNumberOfConnected();
    void newMessagesAvailable();
    void newNameForTopic(QString newName);
private:
    QTextBrowser messagesBox;
    QNetworkReply* reply;
    QNetworkAccessManager networkManager;
    QTimer timerForGetMessage;
    QList<QPair<QString, QString> > listOfInput;
    QList<QString>* listOfIgnoredPseudo;
    QMap<int, QString> listOfEdit;
    QString messagesStatus;
    QString numberOfConnected;
    QString topicLink;
    QString topicName;
    QString pseudoOfUser;
    QString captchaLink;
    bool firstTimeGetMessages;
    bool retrievesMessage;
    bool linkHasChanged;
    bool errorMode;
    int idOfLastMessage;
};

#endif
