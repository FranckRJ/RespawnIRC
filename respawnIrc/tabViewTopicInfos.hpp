#ifndef TABVIEWTOPICINFOS_HPP
#define TABVIEWTOPICINFOS_HPP

#include <QWidget>
#include <QTabWidget>
#include <QString>
#include <QList>
#include <QPair>
#include <QNetworkCookie>
#include <QPixmap>

#include "containerForTopicsInfos.hpp"
#include "colorPseudoListWindow.hpp"
#include "accountListWindow.hpp"
#include "imageDownloadTool.hpp"

class tabViewTopicInfosClass : public QWidget
{
    Q_OBJECT
public:
    tabViewTopicInfosClass(const QList<QString>* newListOfIgnoredPseudo, const QList<pseudoWithColorStruct>* newListOfColorPseudo,
                           const QList<accountStruct>* newListOfAccount, QWidget* parent = 0);
    void doStuffBeforeQuit();
    void updateSettings();
    void updateSettingInfoForList();
    void setNewTheme(QString newThemeName);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave, QString forThisPseudo = "");
    void setNewCookiesForCurrentTab(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave);
    bool getEditInfoForCurrentTab(long idOfMessageToEdit, bool useMessageEdit);
    void selectThisTab(int number);
    void addNewTabWithPseudo(QString useThisPseudo);
    bool getTabListIsEmpty() const;
    QString getTopicLinkFirstPageOfCurrentTab() const;
    QString getTopicLinkLastPageOfCurrentTab() const;
    QString getTopicNameOfCurrentTab() const;
    QString getPseudoUsedOfCurrentTab() const;
    QString getMessageStatusOfCurrentTab() const;
    QString getNumberOfConnectedAndMPOfCurrentTab() const;
    const QList<QNetworkCookie>& getListOfCookiesOfCurrentTab() const;
    const QList<QPair<QString, QString>>& getListOfInputsOfCurrentTab() const;
    void setBufferForTopicLinkFirstPage(QString topicLink, int idOfTab);
public slots:
    void addNewTab();
    void setNewTopicForCurrentTab(QString newTopicLink);
    void updateCurrentTopic();
    void reloadCurrentTopic();
    void reloadAllTopic();
private:
    const containerForTopicsInfosClass* getConstCurrentWidget() const;
    containerForTopicsInfosClass* getCurrentWidget();
private slots:
    void currentTabChanged(int newIndex);
    void addNewTabWithTopic(QString newTopicLink);
    void removeTab(int index);
    void tabHasMoved(int indexFrom, int indexTo);
    void setNewTopicName(QString topicName);
    void setNewCookiesForPseudo();
    void warnUserForNewMessages();
    void downloadStickersIfNeeded(QStringList listOfStickersNeedToBeCheck);
    void downloadNoelshackImagesIfNeeded(QStringList listOfNoelshackImagesNeedToBeCheck);
    void downloadAvatarsIfNeeded(QStringList listOfAvatarsNeedToBeCheck);
    void updateImagesIfNeeded();
signals:
    void currentTabHasChanged();
    void newMessageStatus();
    void newNumberOfConnectedAndMP();
    void newMPAreAvailables(int newNumber, QString withThisPseudo);
    void newMessagesAvailable();
    void setEditInfo(long idOfMessageEdit, QString messageEdit, QString infoToSend, bool useMessageEdit);
    void quoteThisMessage(QString messageToQuote);
    void addToBlacklist(QString pseudoToBlacklist);
    void editThisMessage(long idOfMessageEdit, bool useMessageEdit);
    void newCookiesHaveToBeSet(QString pseudo, const QList<QNetworkCookie>& cookiesForPseudo);
private:
    QTabWidget tabList;
    QList<containerForTopicsInfosClass*> listOfContainerForTopicsInfos;
    const QList<QString>* listOfIgnoredPseudo;
    const QList<pseudoWithColorStruct>* listOfColorPseudo;
    const QList<accountStruct>* listOfAccount;
    QList<QNetworkCookie> generalCookieList;
    QString generalPseudoToUse;
    QPixmap alertImage;
    imageDownloadToolClass imageDownloadTool;
    QString currentThemeName;
    int typeOfImageRefresh;
};

#endif
