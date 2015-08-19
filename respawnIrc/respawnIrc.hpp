#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

#include "showTopicMessages.hpp"
#include "accountlistwindow.hpp"
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
    void showAccountListWindow();
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
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
    void setNewTopic(QString newTopic);
    void setCodeForCaptcha(QString code);
    void setNewMessageStatus();
    void setNewNumberOfConnectedAndPseudoUsed();
    void setNewTopicName(QString topicName);
    void saveListOfAccount();
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
    QList<QNetworkCookie> currentCookieList;
    QList<showTopicMessagesClass*> listOfShowTopicMessages;
    QList<QString> listOfTopicLink;
    QList<QString> listOfIgnoredPseudo;
    QList<accountStruct> listOfAccount;
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
};

#endif
