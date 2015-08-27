#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

#include "showTopicMessages.hpp"
#include "colorPseudoListWindow.hpp"
#include "accountListWindow.hpp"
#include "multiTypeTextBox.hpp"
#include "checkUpdate.hpp"

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
    void selectThisTab(int number);
    void setButtonInButtonLayoutVisible(bool visible);
    static const QString currentVersionName;
public slots:
    void showConnect();
    void showAccountListWindow();
    void showSelectTopic();
    void showIgnoreListWindow();
    void showColorPseudoListWindow();
    void showUpdateTopicTimeWindow();
    void showNumberOfMessageShowedFirstTimeWindow();
    void showAbout();
    void addNewTab();
    void removeTab(int index);
    void checkForUpdate();
    void updateTopic();
    void reloadTopic();
    void goToCurrentTopic();
    void goToCurrentForum();
    void setUpdateTopicTime(int newTime);
    void setNumberOfMessageShowedFirstTime(int newNumber);
    void setShowTextDecorationButton(bool newVal);
    void setMultilineEdit(bool newVal);
    void setLoadTwoLastPage(bool newVal);
    void setIgnoreNetworkError(bool newVal);
    void setSearchForUpdateAtLaunch(bool newVal);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
    void setNewCookiesForCurrentTopic(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool savePseudo);
    void setNewTopic(QString newTopic);
    void setCodeForCaptcha(QString code);
    void setNewMessageStatus();
    void setNewNumberOfConnectedAndPseudoUsed();
    void setNewTopicName(QString topicName);
    void saveListOfAccount();
    void saveListOfIgnoredPseudo();
    void saveListOfColorPseudo();
    void warnUserForNewMessages();
    void currentTabChanged(int newIndex);
    void postMessage();
    void deleteReplyForSendMessage();
    void setEditLastMessage();
    void setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString infoToSend, QString captchaLink);
    void clipboardChanged();
protected:
    void focusInEvent(QFocusEvent * event);
private:
    QGridLayout* mainLayout;
    QHBoxLayout* buttonLayout;
    QTabWidget tabList;
    QList<QNetworkCookie> currentCookieList;
    QList<QNetworkCookie> cookieListForPostMsg;
    QList<showTopicMessagesClass*> listOfShowTopicMessages;
    QList<QString> listOfTopicLink;
    QList<QString> listOfIgnoredPseudo;
    QList<pseudoWithColorStruct> listOfColorPseudo;
    QList<accountStruct> listOfAccount;
    QList<QString> listOfPseudoForTopic;
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
    checkUpdateClass checkUpdate;
    QString dataForEditLastMessage;
    QString captchaLinkForEditLastMessage;
    bool inSending;
    bool isInEdit;
};

#endif
