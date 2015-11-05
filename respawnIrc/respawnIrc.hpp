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
    void useThisFavorite(int index);
    QString addThisFavorite(int index);
    void delThisFavorite(int index);
    void updateSettingInfoForList();
    static const QString currentVersionName;
public slots:
    void showConnect();
    void showAccountListWindow();
    void showSelectTopic();
    void showSelectTheme();
    void showIgnoreListWindow();
    void showColorPseudoListWindow();
    void showUpdateTopicTimeWindow();
    void showNumberOfMessageShowedFirstTimeWindow();
    void showStickersSizeWindow();
    void showAbout();
    void addNewTab();
    void addNewTabWithTopic(QString newTopicLink);
    void removeTab(int index);
    void checkForUpdate();
    void updateTopic();
    void reloadTopic();
    void goToCurrentTopic();
    void goToCurrentForum();
    void quoteThisMessage(QString messageToQuote);
    void addThisPeudoToBlacklist(QString pseudoToAdd);
    void setUpdateTopicTime(int newTime);
    void setNumberOfMessageShowedFirstTime(int newNumber);
    void setStickersSize(int newSize);
    void setThisBoolOption(bool newVal);
    void setShowTextDecorationButton(bool newVal);
    void setMultilineEdit(bool newVal);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
    void setNewCookiesForCurrentTopic(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool savePseudo);
    void setNewTopic(QString newTopic);
    void setNewTheme(QString newThemeName);
    void reloadTheme();
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
    void editLastMessage();
    void setEditMessage(int idOfMessageToEdit = 0, bool useMessageEdit = true);
    void setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString infoToSend, QString captchaLink, bool useMessageEdit);
    void clipboardChanged();
signals:
    void themeChanged(QString newThemeName);
protected:
    void focusInEvent(QFocusEvent * event);
private:
    QHBoxLayout* buttonLayout;
    QTabWidget tabList;
    QVector<QString> vectorOfFavoriteLink;
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
    QNetworkAccessManager* networkManager;
    QLabel messagesStatus;
    QLabel numberOfConnectedAndPseudoUsed;
    QPushButton sendButton;
    QString pseudoOfUser;
    QString captchaCode;
    QPixmap alertImage;
    checkUpdateClass checkUpdate;
    QString dataForEditLastMessage;
    QString captchaLinkForEditLastMessage;
    QString currentThemeName;
    bool inSending;
    bool isInEdit;
    bool beepWhenWarn;
    int idOfLastMessageEdit;
};

#endif
