#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

#include "showTopicMessages.hpp"
#include "multiTypeTextBox.hpp"

class respawnIrcClass : public QWidget
{
    Q_OBJECT
public:
    respawnIrcClass(QWidget* parent = 0);
    void loadSettings();
    showTopicMessagesClass* getCurrentWidget();
    multiTypeTextBoxClass* getMessageLine();
    QString buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput);
    void addButtonToButtonLayout();
    void setButtonInButtonLayoutVisible(bool visible);
public slots:
    void showConnect();
    void showSelectTopic();
    void showIgnoreListWindow();
    void showUpdateTopicTimeWindow();
    void showNumberOfMessageShowedFirstTimeWindow();
    void addNewTab();
    void removeTab(int index);
    void updateTopic();
    void reloadTopic();
    void goToCurrentTopic();
    void goToCurrentForum();
    void setUpdateTopicTime(int newTime);
    void setNumberOfMessageShowedFirstTime(int newNumber);
    void setShowTextDecorationButton(bool newVal);
    void setMultilineEdit(bool newVal);
    void setLoadTwoLastPage(bool newVal);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo);
    void setNewTopic(QString newTopic);
    void setCodeForCaptcha(QString code);
    void setNewMessageStatus();
    void setNewNumberOfConnectedAndPseudoUsed();
    void setNewTopicName(QString topicName);
    void saveListOfIgnoredPseudo();
    void warnUserForNewMessages();
    void currentTabChanged(int newIndex);
    void postMessage();
    void deleteReplyForSendMessage();
    void clipboardChanged();
protected:
    void focusInEvent(QFocusEvent * event);
private:
    QGridLayout* mainLayout;
    QHBoxLayout* buttonLayout;
    QTabWidget tabList;
    QList<showTopicMessagesClass*> listOfShowTopicMessages;
    QList<QString> listOfTopicLink;
    QList<QString> listOfIgnoredPseudo;
    QList<QPair<QString, QString> > oldListOfInput;
    multiTypeTextBoxClass messageLine;
    QNetworkReply* replyForSendMessage;
    QNetworkAccessManager networkManager;
    QLabel messagesStatus;
    QLabel numberOfConnectedAndPseudoUsed;
    QPushButton sendButton;
    QString pseudoOfUser;
    QString captchaCode;
    QPixmap alertImage;
    bool isConnected;
    int updateTopicTime;
    int numberOfMessageShowedFirstTime;
};

#endif
