#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QMap>
#include <QList>
#include <QTabWidget>
#include <QNetworkCookie>
#include <QVector>
#include <QPixmap>
#include <QStringList>
#include <QFocusEvent>
#include <QHBoxLayout>

#include "containerForTopicsInfos.hpp"
#include "sendMessages.hpp"
#include "colorPseudoListWindow.hpp"
#include "accountListWindow.hpp"
#include "multiTypeTextBox.hpp"
#include "checkUpdate.hpp"
#include "imageDownloadTool.hpp"

class respawnIrcClass : public QWidget
{
    Q_OBJECT
public:
    explicit respawnIrcClass(QWidget* parent = 0);
    void doStuffBeforeQuit();
    void loadSettings();
    containerForTopicsInfosClass* getCurrentWidget();
    multiTypeTextBoxClass* getMessageLine();
    void addButtonToButtonLayout();
    void selectThisTab(int number);
    void setButtonInButtonLayoutVisible(bool visible);
    void useThisFavorite(int index);
    QString addThisFavorite(int index);
    void delThisFavorite(int index);
    void updateSettingInfoForList();
    static const QString currentVersionName;
public slots:
    void showWebNavigator();
    void showWebNavigatorAtPM();
    void showConnect();
    void showAccountListWindow();
    void showSelectSticker();
    void showSelectTopic();
    void showSelectTheme();
    void showPreferences();
    void showIgnoreListWindow();
    void showColorPseudoListWindow();
    void showAbout();
    void addNewTab();
    void addNewTabWithPseudo(QString useThisPseudo);
    void addNewTabWithTopic(QString newTopicLink);
    void removeTab(int index);
    void tabHasMoved(int indexFrom, int indexTo);
    void checkForUpdate();
    void updateTopic();
    void reloadTopic();
    void reloadAllTopic();
    void goToCurrentTopic();
    void goToCurrentForum();
    void disconnectFromAllTabs();
    void disconnectFromCurrentTab();
    void disconnectFromThisPseudo(QString thisPseudo);
    void addThisPeudoToBlacklist(QString pseudoToAdd);
    void setTheseOptions(QMap<QString, bool> newBoolOptions, QMap<QString, int> newIntOptions);
    void setShowTextDecorationButton(bool newVal);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
    void setNewCookiesForCurrentTopic(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool savePseudo);
    void setNewCookiesForPseudo();
    void setNewTopic(QString newTopic);
    void setNewTheme(QString newThemeName);
    void reloadTheme();
    void setNewMessageStatus();
    void setNewNumberOfConnectedAndPseudoUsed();
    void setNewTopicName(QString topicName);
    void saveListOfAccount();
    void saveListOfIgnoredPseudo();
    void saveListOfColorPseudo();
    void warnUserForNewMessages();
    void currentTabChanged(int newIndex);
    void messageHaveToBePosted();
    void editLastMessage();
    void setEditMessage(long idOfMessageToEdit = 0, bool useMessageEdit = true);
    void downloadStickersIfNeeded(QStringList listOfStickersNeedToBeCheck);
    void downloadNoelshackImagesIfNeeded(QStringList listOfNoelshackImagesNeedToBeCheck);
    void downloadAvatarsIfNeeded(QStringList listOfAvatarsNeedToBeCheck);
    void updateImagesIfNeeded();
    void clipboardChanged();
signals:
    void themeChanged(QString newThemeName);
protected:
    void focusInEvent(QFocusEvent* event);
private:
    QHBoxLayout* buttonLayout;
    sendMessagesClass sendMessages;
    QTabWidget tabList;
    QVector<QString> vectorOfFavoriteLink;
    QList<QNetworkCookie> currentCookieList;
    QList<containerForTopicsInfosClass*> listOfContainerForTopicsInfos;
    QList<QString> listOfIgnoredPseudo;
    QList<pseudoWithColorStruct> listOfColorPseudo;
    QList<accountStruct> listOfAccount;
    QLabel messagesStatus;
    QLabel numberOfConnectedAndPseudoUsed;
    QString pseudoOfUser;
    QPixmap alertImage;
    checkUpdateClass checkUpdate;
    imageDownloadToolClass imageDownloadTool;
    QString currentThemeName;
    QString lastClipboardDataChanged;
    bool beepWhenWarn;
    bool warnUser;
    int typeOfImageRefresh;
};

#endif
