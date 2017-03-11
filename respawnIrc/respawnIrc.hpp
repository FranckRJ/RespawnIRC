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
    void selectThisTab(int number);
    void useThisFavorite(int index);
    QString addThisFavorite(int index);
    void delThisFavorite(int index);
    multiTypeTextBoxClass* getMessageLine();
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
    void checkForUpdate();
    void updateTopic();
    void reloadTopic();
    void reloadAllTopic();
    void goToCurrentTopic();
    void goToCurrentForum();
    void disconnectFromAllTabs();
    void disconnectFromCurrentTab();
    void setNewTheme(QString newThemeName);
    void reloadTheme();
    void messageHaveToBePosted();
    void editLastMessage();
    void clipboardChanged();
private:
    void loadSettings();
    containerForTopicsInfosClass* getCurrentWidget();
    void addButtonToButtonLayout();
    void setShowTextDecorationButton(bool newVal);
    void updateSettingInfoForList();
    void focusInEvent(QFocusEvent* event) override;
    void addNewTabWithPseudo(QString useThisPseudo);
private slots:
    void addNewTabWithTopic(QString newTopicLink);
    void removeTab(int index);
    void tabHasMoved(int indexFrom, int indexTo);
    void disconnectFromThisPseudo(QString thisPseudo);
    void addThisPeudoToBlacklist(QString pseudoToAdd);
    void setTheseOptions(QMap<QString, bool> newBoolOptions, QMap<QString, int> newIntOptions);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
    void setNewCookiesForCurrentTopic(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool savePseudo);
    void setNewCookiesForPseudo();
    void setNewTopic(QString newTopic);
    void setNewMessageStatus();
    void setNewNumberOfConnectedAndPseudoUsed();
    void setNewTopicName(QString topicName);
    void saveListOfAccount();
    void saveListOfIgnoredPseudo();
    void saveListOfColorPseudo();
    void warnUserForNewMessages();
    void warnUserForNewMP(int newNumber, QString withThisPseudo);
    void currentTabChanged(int newIndex);
    void setEditMessage(long idOfMessageToEdit = 0, bool useMessageEdit = true);
    void downloadStickersIfNeeded(QStringList listOfStickersNeedToBeCheck);
    void downloadNoelshackImagesIfNeeded(QStringList listOfNoelshackImagesNeedToBeCheck);
    void downloadAvatarsIfNeeded(QStringList listOfAvatarsNeedToBeCheck);
    void updateImagesIfNeeded();
signals:
    void themeChanged(QString newThemeName);
public:
    static const QString currentVersionName;
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
    QMap<QString, int> numberOfMPPerPseudos;
    QLabel messagesStatus;
    QLabel numberOfConnectedAndPseudoUsed;
    QString pseudoOfUser;
    QPixmap alertImage;
    checkUpdateClass checkUpdate;
    imageDownloadToolClass imageDownloadTool;
    QString currentThemeName;
    QString lastClipboardDataChanged;
    bool beepWhenWarn;
    bool beepForNewMP;
    bool warnUser;
    int typeOfImageRefresh;
};

#endif
