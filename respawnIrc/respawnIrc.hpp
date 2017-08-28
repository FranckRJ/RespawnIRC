#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QWidget>
#include <QLabel>
#include <QString>
#include <QMap>
#include <QList>
#include <QNetworkCookie>
#include <QVector>
#include <QStringList>
#include <QFocusEvent>
#include <QHBoxLayout>

#include "sendMessages.hpp"
#include "colorPseudoListWindow.hpp"
#include "accountListWindow.hpp"
#include "multiTypeTextBox.hpp"
#include "checkUpdate.hpp"
#include "tabViewTopicInfos.hpp"

class respawnIrcClass : public QWidget
{
    Q_OBJECT
public:
    explicit respawnIrcClass(QWidget* parent = nullptr);
    void doStuffBeforeQuit();
    void useThisFavorite(int index);
    QString addThisFavorite(int index);
    void delThisFavorite(int index);
    tabViewTopicInfosClass* getTabView();
    multiTypeTextBoxClass* getMessageLine();
public slots:
    void showWebNavigator();
    void showWebNavigatorAtMP();
    void showConnect();
    void showAccountListWindow();
    void showSelectSticker();
    void showSelectTopic();
    void showSelectTheme();
    void showPreferences();
    void showIgnoreListWindow();
    void showColorPseudoListWindow();
    void showManageShortcutWindow();
    void showAbout();
    void checkForUpdate();
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
    void addButtonToButtonLayout();
    void setShowTextDecorationButton(bool newVal);
    void focusInEvent(QFocusEvent* event) override;
private slots:
    void disconnectFromThisPseudo(QString thisPseudo);
    void addThisPeudoToBlacklist(QString pseudoToAdd);
    void setTheseOptions(QMap<QString, bool> newBoolOptions, QMap<QString, int> newIntOptions);
    void setNewCookie(QNetworkCookie newConnectCookie, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
    void setNewCookieForCurrentTopic(QNetworkCookie newConnectCookie, QString newPseudoOfUser, bool savePseudo);
    void setNewCookieForPseudo(QString pseudo, const QNetworkCookie& cookieForPseudo);
    void setNewMessageStatus();
    void setNewNumberOfConnectedAndPseudoUsed();
    void saveListOfAccount();
    void saveListOfIgnoredPseudo();
    void saveListOfColorPseudo();
    void warnUserForNewMessages();
    void warnUserForNewMP(int newNumber, QString withThisPseudo);
    void tabOfTabViewChanged();
    void setEditMessage(long idOfMessageToEdit = 0, bool useMessageEdit = true);
signals:
    void themeChanged(QString newThemeName);
public:
    static const QString currentVersionName;
private:
    QHBoxLayout* buttonLayout;
    sendMessagesClass* sendMessages;
    tabViewTopicInfosClass* tabViewTopicInfos;
    QVector<QString> vectorOfFavoriteLink;
    QNetworkCookie currentConnectCookie;
    QList<QString> listOfIgnoredPseudo;
    QList<pseudoWithColorStruct> listOfColorPseudo;
    QList<accountStruct> listOfAccount;
    QMap<QString, int> numberOfMPPerPseudos;
    QLabel* messagesStatus;
    QLabel* numberOfConnectedAndPseudoUsed;
    QString pseudoOfUser;
    checkUpdateClass* checkUpdate;
    QString currentThemeName;
    QString lastClipboardDataChanged;
    bool beepWhenWarn;
    bool beepForNewMP;
    bool warnUser;
};

#endif
