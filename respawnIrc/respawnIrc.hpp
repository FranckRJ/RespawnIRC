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
    void warnUser();
    void loadSettings();
    void startGetMessage();
public slots:
    void showConnect();
    void showSelectTopic();
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo);
    void setNewTopic(QString newTopic);
    void setCodeForCaptcha(QString code);
    void getMessages();
    void analyzeMessages();
    void postMessage();
    void deleteReplyForSendMessage();
    void clipboardChanged();
protected:
    void focusInEvent(QFocusEvent * event);
private:
    QSettings setting;
    QTextBrowser messagesBox;
    QTextEdit messageLine;
    QNetworkReply* reply;
    QNetworkReply* replyForSendMessage;
    QNetworkAccessManager networkManager;
    QTimer timerForGetMessage;
    QList<QPair<QString, QString> > listOfInput;
    QLabel messagesStatus;
    QPushButton sendButton;
    QString topicLink;
    QString pseudoOfUser;
    QString captchaLink;
    QString captchaCode;
    bool isConnected;
    bool firstTimeGetMessages;
    bool retrievesMessage;
    bool linkHasChanged;
    int idOfLastMessage;
};

#endif
