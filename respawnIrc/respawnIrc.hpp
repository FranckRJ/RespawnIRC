#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

#include "showTopicMessages.hpp"

class respawnIrcClass : public QWidget
{
    Q_OBJECT
public:
    respawnIrcClass(QWidget* parent = 0);
    void loadSettings();
    showTopicMessagesClass* getCurrentWidget();
    QString buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput);
    QList<QVariant> createQVariantListWithThisList(QList<QString> list);
    QList<QString> createListWithThisQVariantList(QList<QVariant> list);
public slots:
    void showConnect();
    void showSelectTopic();
    void addNewTab();
    void removeTab(int index);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo);
    void setNewTopic(QString newTopic);
    void setCodeForCaptcha(QString code);
    void setNewMessageStatus();
    void setNewTopicName(QString topicName);
    void warnUserForNewMessages();
    void currentTabChanged(int newIndex);
    void postMessage();
    void deleteReplyForSendMessage();
    void clipboardChanged();
protected:
    void focusInEvent(QFocusEvent * event);
private:
    QSettings setting;
    QTabWidget tabList;
    QList<showTopicMessagesClass*> listOfShowTopicMessages;
    QList<QString> listOfTopicLink;
    QList<QPair<QString, QString> > oldListOfInput;
    QTextEdit messageLine;
    QNetworkReply* replyForSendMessage;
    QNetworkAccessManager networkManager;
    QLabel messagesStatus;
    QPushButton sendButton;
    QString pseudoOfUser;
    QString captchaCode;
    QPixmap alertImage;
    bool isConnected;
};

#endif
