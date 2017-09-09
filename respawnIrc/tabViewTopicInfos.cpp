#include <QHBoxLayout>
#include <QTabBar>
#include <QCoreApplication>

#include "tabViewTopicInfos.hpp"
#include "styleTool.hpp"
#include "settingTool.hpp"

tabViewTopicInfosClass::tabViewTopicInfosClass(const QList<QString>* newListOfIgnoredPseudo, const QList<pseudoWithColorStruct>* newListOfColorPseudo,
                                               const QList<accountStruct>* newListOfAccount, QWidget* parent) : QWidget(parent)
{
    tabList = new QTabWidget(this);
    imageDownloadTool = new imageDownloadToolClass(this);

    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    listOfColorPseudo = newListOfColorPseudo;
    listOfAccount = newListOfAccount;

    tabList->setObjectName("tabListForTopics");

    tabList->setTabsClosable(true);
    tabList->setMovable(true);
    alertImage.load(QCoreApplication::applicationDirPath() + "/resources/alert.png");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(tabList);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    connect(tabList, &QTabWidget::currentChanged, this, &tabViewTopicInfosClass::currentTabChanged);
    connect(tabList, &QTabWidget::tabCloseRequested, this, &tabViewTopicInfosClass::removeTab);
    connect(tabList->tabBar(), &QTabBar::tabMoved, this, &tabViewTopicInfosClass::tabHasMoved);
    connect(imageDownloadTool, &imageDownloadToolClass::oneDownloadFinished, this, &tabViewTopicInfosClass::updateImagesIfNeeded);

    updateSettings(true);
    imageDownloadTool->addOrUpdateRule("sticker", "/resources/stickers/", false, true, "http://jv.stkr.fr/p/", ".png", true);
    addOrUpdateAvatarRuleForImageDownloader();
    addOrUpdateNoelshackRuleForImageDownloader();
}

void tabViewTopicInfosClass::doStuffBeforeQuit()
{
    QList<QString> listOfTopicLink;
    QList<QString> listOfPseudoForTopic;

    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        if(thisContainer->getTopicLinkFirstPage().isEmpty() == false)
        {
            listOfTopicLink.push_back(thisContainer->getTopicLinkFirstPage());

            if(thisContainer->getPseudoTypeOfSave() == typeOfSaveForPseudo::REMEMBER)
            {
                if(thisContainer->getShowTopic().getPseudoUsed().isEmpty() == true)
                {
                    listOfPseudoForTopic.push_back(".");
                }
                else
                {
                    listOfPseudoForTopic.push_back(thisContainer->getShowTopic().getPseudoUsed());
                }
            }
            else
            {
                listOfPseudoForTopic.push_back("");
            }
        }
    }

    settingTool::saveListOfTopicLink(listOfTopicLink);
    settingTool::saveListOfPseudoForTopic(listOfPseudoForTopic);
    imageDownloadTool->deleteCache();
}

void tabViewTopicInfosClass::updateSettings(bool firstTimeUpdate)
{
    int oldAvatarSize = avatarSize;
    int oldNoelshackImageWidth = noelshackImageWidth;
    int oldNoelshackImageHeight = noelshackImageHeight;
    bool oldDownloadHighDefAvatar = downloadHighDefAvatar;

    avatarSize = ((settingTool::getThisBoolOption("smartAvatarResizing") == true) ? settingTool::getThisIntOption("avatarSize").value : 0);
    noelshackImageWidth = ((settingTool::getThisBoolOption("smartNoelshackResizing") == true) ? settingTool::getThisIntOption("noelshackImageWidth").value : 0);
    noelshackImageHeight = ((settingTool::getThisBoolOption("smartNoelshackResizing") == true) ? settingTool::getThisIntOption("noelshackImageHeight").value : 0);
    downloadHighDefAvatar = settingTool::getThisBoolOption("downloadHighDefAvatar");
    typeOfImageRefresh = settingTool::getThisIntOption("typeOfImageRefresh").value;

    if(firstTimeUpdate == false && (avatarSize != oldAvatarSize || noelshackImageWidth != oldNoelshackImageWidth ||
                                    noelshackImageHeight != oldNoelshackImageHeight || downloadHighDefAvatar != oldDownloadHighDefAvatar))
    {
        QString themeImgDir = styleTool::getImagePathOfThemeIfExist(currentThemeName);
        imageDownloadTool->resetCache();
        addOrUpdateAvatarRuleForImageDownloader();
        addOrUpdateNoelshackRuleForImageDownloader();

        for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
        {
            thisContainer->getShowTopic().resetSearchPath();
            thisContainer->getShowTopic().addSearchPath(imageDownloadTool->getPathOfTmpDir());
            thisContainer->getShowTopic().addSearchPath(themeImgDir);
        }
    }
}

void tabViewTopicInfosClass::updateSettingInfoForList()
{
    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        thisContainer->updateSettingsForInfo();
    }
}

void tabViewTopicInfosClass::setNewTheme(QString newThemeName)
{
    QString themeImgDir;

    currentThemeName = newThemeName;
    themeImgDir = styleTool::getImagePathOfThemeIfExist(currentThemeName);

    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        thisContainer->getShowTopic().resetSearchPath();
        thisContainer->setNewThemeForInfo(currentThemeName);
        thisContainer->setNewTopicForInfo(thisContainer->getTopicLinkFirstPage());

        thisContainer->getShowTopic().addSearchPath(imageDownloadTool->getPathOfTmpDir());
        thisContainer->getShowTopic().addSearchPath(themeImgDir);
    }
}

void tabViewTopicInfosClass::setNewCookie(QNetworkCookie newConnectCookie, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave, QString forThisPseudo)
{
    generalConnectCookie = newConnectCookie;
    generalPseudoToUse = newPseudoOfUser;

    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        if(forThisPseudo.isEmpty() == true || thisContainer->getShowTopic().getPseudoUsed().toLower() == forThisPseudo.toLower())
        {
            thisContainer->setNewCookieForInfo(newConnectCookie, newPseudoOfUser, newTypeOfSave);
        }
    }
}

void tabViewTopicInfosClass::setNewCookieForCurrentTab(QNetworkCookie newConnectCookie, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave)
{
    getCurrentWidget()->setNewCookieForInfo(newConnectCookie, newPseudoOfUser, newTypeOfSave);
}

bool tabViewTopicInfosClass::getEditInfoForCurrentTab(long idOfMessageToEdit, bool useMessageEdit)
{
    return getCurrentWidget()->getShowTopic().getEditInfo(idOfMessageToEdit, useMessageEdit);
}

void tabViewTopicInfosClass::selectThisTab(int number)
{
    if(listOfContainerForTopicsInfos.size() > number)
    {
        tabList->setCurrentIndex(number);
    }
}

void tabViewTopicInfosClass::addNewTabWithPseudo(QString useThisPseudo)
{
    QString themeImgDir = styleTool::getImagePathOfThemeIfExist(currentThemeName);
    listOfContainerForTopicsInfos.push_back(new containerForTopicsInfosClass(listOfIgnoredPseudo, listOfColorPseudo, currentThemeName, this));

    if(useThisPseudo.isEmpty() == false)
    {
        if(useThisPseudo != ".")
        {
            bool pseudoFound = false;

            for(int j = 0; j < listOfAccount->size(); ++j)
            {
                if(listOfAccount->at(j).pseudo.toLower() == useThisPseudo.toLower())
                {
                    pseudoFound = true;
                    listOfContainerForTopicsInfos.back()->setNewCookieForInfo(listOfAccount->at(j).connectCookie, listOfAccount->at(j).pseudo, typeOfSaveForPseudo::REMEMBER);
                    break;
                }
            }

            if(pseudoFound == false)
            {
                listOfContainerForTopicsInfos.back()->setNewCookieForInfo(generalConnectCookie, generalPseudoToUse, typeOfSaveForPseudo::DEFAULT);
            }
        }
        else
        {
            listOfContainerForTopicsInfos.back()->setNewCookieForInfo(QNetworkCookie(), "", typeOfSaveForPseudo::REMEMBER);
        }
    }
    else if(generalPseudoToUse.isEmpty() == false)
    {
        listOfContainerForTopicsInfos.back()->setNewCookieForInfo(generalConnectCookie, generalPseudoToUse, typeOfSaveForPseudo::DEFAULT);
    }

    listOfContainerForTopicsInfos.back()->getShowTopic().addSearchPath(imageDownloadTool->getPathOfTmpDir());
    listOfContainerForTopicsInfos.back()->getShowTopic().addSearchPath(themeImgDir);

    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::newMessageStatus, this, &tabViewTopicInfosClass::newMessageStatus);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::newNumberOfConnectedAndMP, this, &tabViewTopicInfosClass::newNumberOfConnectedAndMP);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::newMPAreAvailables, this, &tabViewTopicInfosClass::newMPAreAvailables);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::newMessagesAvailable, this, &tabViewTopicInfosClass::warnUserForNewMessages);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::newNameForTopic, this, &tabViewTopicInfosClass::setNewTopicName);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::setEditInfo, this, &tabViewTopicInfosClass::setEditInfo);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::quoteThisMessage, this, &tabViewTopicInfosClass::quoteThisMessage);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::addToBlacklist, this, &tabViewTopicInfosClass::addToBlacklist);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::editThisMessage, this, &tabViewTopicInfosClass::editThisMessage);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::downloadTheseStickersIfNeeded, this, &tabViewTopicInfosClass::downloadStickersIfNeeded);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::downloadTheseNoelshackImagesIfNeeded, this, &tabViewTopicInfosClass::downloadNoelshackImagesIfNeeded);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::downloadTheseAvatarsIfNeeded, this, &tabViewTopicInfosClass::downloadAvatarsIfNeeded);
    connect(listOfContainerForTopicsInfos.back(), &containerForTopicsInfosClass::openThisTopicInNewTab, this, &tabViewTopicInfosClass::addNewTabWithTopic);
    connect(listOfContainerForTopicsInfos.back(), &containerForTopicsInfosClass::topicNeedChanged, this, &tabViewTopicInfosClass::setNewTopicForCurrentTab);
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::newCookieHasToBeSet, this, &tabViewTopicInfosClass::setNewCookieForPseudo);
    tabList->addTab(listOfContainerForTopicsInfos.back(), "Onglet " + QString::number(listOfContainerForTopicsInfos.size()));
    tabList->setCurrentIndex(listOfContainerForTopicsInfos.size() - 1);
}

bool tabViewTopicInfosClass::getTabListIsEmpty() const
{
    return listOfContainerForTopicsInfos.isEmpty();
}

QString tabViewTopicInfosClass::getTopicLinkFirstPageOfCurrentTab() const
{
    return getConstCurrentWidget()->getTopicLinkFirstPage();
}

QString tabViewTopicInfosClass::getTopicLinkLastPageOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getTopicLinkLastPage();
}

QString tabViewTopicInfosClass::getTopicNameOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getTopicName();
}

QString tabViewTopicInfosClass::getPseudoUsedOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getPseudoUsed();
}

QString tabViewTopicInfosClass::getMessageStatusOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getMessagesStatus();
}

QString tabViewTopicInfosClass::getNumberOfConnectedAndMPOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getNumberOfConnectedAndMP();
}

const QNetworkCookie& tabViewTopicInfosClass::getConnectCookieOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getConnectCookie();
}

const QList<QPair<QString, QString>>& tabViewTopicInfosClass::getListOfInputsOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getListOfInput();
}

void tabViewTopicInfosClass::setBufferForTopicLinkFirstPage(QString topicLink, int idOfTab)
{
    if(idOfTab < listOfContainerForTopicsInfos.size())
    {
        listOfContainerForTopicsInfos.at(idOfTab)->setBufferForTopicLinkFirstPage(topicLink);
    }
}

void tabViewTopicInfosClass::addNewTab()
{
    addNewTabWithPseudo("");
}

void tabViewTopicInfosClass::setNewTopicForCurrentTab(QString newTopicLink)
{
    getCurrentWidget()->setNewTopicForInfo(newTopicLink);
}

void tabViewTopicInfosClass::updateCurrentTopic()
{
    getCurrentWidget()->getShowTopic().startGetMessage();
}

void tabViewTopicInfosClass::reloadCurrentTopic()
{
    getCurrentWidget()->setNewTopicForInfo(getCurrentWidget()->getTopicLinkFirstPage());
}

void tabViewTopicInfosClass::reloadAllTopic()
{
    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        thisContainer->setNewTopicForInfo(thisContainer->getTopicLinkFirstPage());
    }
}

const containerForTopicsInfosClass* tabViewTopicInfosClass::getConstCurrentWidget() const
{
    return listOfContainerForTopicsInfos.at(tabList->currentIndex());
}

containerForTopicsInfosClass* tabViewTopicInfosClass::getCurrentWidget()
{
    return listOfContainerForTopicsInfos.at(tabList->currentIndex());
}

void tabViewTopicInfosClass::addOrUpdateAvatarRuleForImageDownloader()
{
    imageDownloadTool->addOrUpdateRule("avatar", "/vtr/", true, false, "http://", "", false, avatarSize, avatarSize, true);
}

void tabViewTopicInfosClass::addOrUpdateNoelshackRuleForImageDownloader()
{
    imageDownloadTool->addOrUpdateRule("noelshack", "/img/", true, false, "", "", false, noelshackImageWidth, noelshackImageHeight, false);
}

void tabViewTopicInfosClass::currentTabChanged(int newIndex)
{
    if(newIndex == -1)
    {
        newIndex = tabList->currentIndex();
    }

    tabList->setTabIcon(newIndex, QIcon());

    emit currentTabHasChanged();
}

void tabViewTopicInfosClass::addNewTabWithTopic(QString newTopicLink)
{
    addNewTab();
    setNewTopicForCurrentTab(newTopicLink);
}

void tabViewTopicInfosClass::removeTab(int index)
{
    if(listOfContainerForTopicsInfos.size() > 1)
    {
        tabList->removeTab(index);
        listOfContainerForTopicsInfos.takeAt(index)->deleteLater();
        emit currentTabChanged(-1);
    }
}

void tabViewTopicInfosClass::tabHasMoved(int indexFrom, int indexTo)
{
    listOfContainerForTopicsInfos.move(indexFrom, indexTo);
    emit currentTabChanged(-1);
}

void tabViewTopicInfosClass::setNewTopicName(QString topicName)
{
    QObject* senderObject = sender();

    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        if(senderObject == &listOfContainerForTopicsInfos.at(i)->getShowTopic())
        {
            tabList->setTabText(i, topicName);
        }
    }
}

void tabViewTopicInfosClass::setNewCookieForPseudo()
{
    QObject* senderObject = sender();

    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        if(senderObject == &thisContainer->getShowTopic())
        {
            emit newCookieHasToBeSet(thisContainer->getShowTopic().getPseudoUsed(), thisContainer->getShowTopic().getConnectCookie());
        }
    }
}

void tabViewTopicInfosClass::warnUserForNewMessages()
{
    QObject* senderObject = sender();

    if(senderObject != &getCurrentWidget()->getShowTopic())
    {
        for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
        {
            if(senderObject == &listOfContainerForTopicsInfos.at(i)->getShowTopic())
            {
                tabList->setTabIcon(i, QIcon(alertImage));
            }
        }
    }

    emit newMessagesAvailable();
}

void tabViewTopicInfosClass::downloadStickersIfNeeded(QStringList listOfStickersNeedToBeCheck)
{
    imageDownloadTool->checkAndStartDownloadMissingImages(listOfStickersNeedToBeCheck, "sticker");
}

void tabViewTopicInfosClass::downloadNoelshackImagesIfNeeded(QStringList listOfNoelshackImagesNeedToBeCheck)
{
    imageDownloadTool->checkAndStartDownloadMissingImages(listOfNoelshackImagesNeedToBeCheck, "noelshack");
}

void tabViewTopicInfosClass::downloadAvatarsIfNeeded(QStringList listOfAvatarsNeedToBeCheck)
{
    imageDownloadTool->checkAndStartDownloadMissingImages(listOfAvatarsNeedToBeCheck, "avatar");
}

void tabViewTopicInfosClass::updateImagesIfNeeded()
{
    if(typeOfImageRefresh == 2)
    {
        getCurrentWidget()->getShowTopic().relayoutDocumentHack();
    }
    else if(typeOfImageRefresh == 1)
    {
        if(imageDownloadTool->getNumberOfDownloadRemaining() == 0)
        {
            getCurrentWidget()->getShowTopic().relayoutDocumentHack();
        }
    }
}
