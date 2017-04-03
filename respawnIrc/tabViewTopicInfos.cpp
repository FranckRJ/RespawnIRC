#include <QHBoxLayout>
#include <QTabBar>
#include <QCoreApplication>

#include "tabViewTopicInfos.hpp"
#include "styleTool.hpp"
#include "settingTool.hpp"

tabViewTopicInfosClass::tabViewTopicInfosClass(const QList<QString>* newListOfIgnoredPseudo, const QList<pseudoWithColorStruct>* newListOfColorPseudo,
                                               const QList<accountStruct>* newListOfAccount, QWidget* parent) : QWidget(parent)
{
    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    listOfColorPseudo = newListOfColorPseudo;
    listOfAccount = newListOfAccount;
    tabList.setObjectName("tabListForTopics");

    tabList.setTabsClosable(true);
    tabList.setMovable(true);
    alertImage.load(QCoreApplication::applicationDirPath() + "/resources/alert.png");
    imageDownloadTool.addRule("sticker", "/resources/stickers/", false, true, "http://jv.stkr.fr/p/", ".png", true);
    imageDownloadTool.addRule("noelshack", "/img/", true);
    imageDownloadTool.addRule("avatar", "/vtr/", true, false, "http://");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(&tabList);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    connect(&tabList, &QTabWidget::currentChanged, this, &tabViewTopicInfosClass::currentTabChanged);
    connect(&tabList, &QTabWidget::tabCloseRequested, this, &tabViewTopicInfosClass::removeTab);
    connect(tabList.tabBar(), &QTabBar::tabMoved, this, &tabViewTopicInfosClass::tabHasMoved);
    connect(&imageDownloadTool, &imageDownloadToolClass::oneDownloadFinished, this, &tabViewTopicInfosClass::updateImagesIfNeeded);

    updateSettings();
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

    settingToolClass::saveListOfTopicLink(listOfTopicLink);
    settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
}

void tabViewTopicInfosClass::updateSettings()
{
    typeOfImageRefresh = settingToolClass::getThisIntOption("typeOfImageRefresh").value;
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
    themeImgDir = styleToolClass::getImagePathOfThemeIfExist(currentThemeName);

    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        thisContainer->setNewThemeForInfo(currentThemeName);
        thisContainer->setNewTopicForInfo(thisContainer->getTopicLinkFirstPage());

        if(themeImgDir.isEmpty() == false)
        {
            thisContainer->getShowTopic().addSearchPath(themeImgDir);
        }
    }
}

void tabViewTopicInfosClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave, QString forThisPseudo)
{
    generalCookieList = newCookies;
    generalPseudoToUse = newPseudoOfUser;

    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        if(forThisPseudo.isEmpty() == true || thisContainer->getShowTopic().getPseudoUsed().toLower() == forThisPseudo.toLower())
        {
            thisContainer->setNewCookiesForInfo(newCookies, newPseudoOfUser, newTypeOfSave);
        }
    }
}

void tabViewTopicInfosClass::setNewCookiesForCurrentTab(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave)
{
    getCurrentWidget()->setNewCookiesForInfo(newCookies, newPseudoOfUser, newTypeOfSave);
}

bool tabViewTopicInfosClass::getEditInfoForCurrentTab(long idOfMessageToEdit, bool useMessageEdit)
{
    return getCurrentWidget()->getShowTopic().getEditInfo(idOfMessageToEdit, useMessageEdit);
}

void tabViewTopicInfosClass::selectThisTab(int number)
{
    if(listOfContainerForTopicsInfos.size() > number)
    {
        tabList.setCurrentIndex(number);
    }
}

void tabViewTopicInfosClass::addNewTabWithPseudo(QString useThisPseudo)
{
    QString themeImgDir = styleToolClass::getImagePathOfThemeIfExist(currentThemeName);
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
                    listOfContainerForTopicsInfos.back()->setNewCookiesForInfo(listOfAccount->at(j).listOfCookie, listOfAccount->at(j).pseudo, typeOfSaveForPseudo::REMEMBER);
                    break;
                }
            }

            if(pseudoFound == false)
            {
                listOfContainerForTopicsInfos.back()->setNewCookiesForInfo(generalCookieList, generalPseudoToUse, typeOfSaveForPseudo::DEFAULT);
            }
        }
        else
        {
            listOfContainerForTopicsInfos.back()->setNewCookiesForInfo(QList<QNetworkCookie>(), "", typeOfSaveForPseudo::REMEMBER);
        }
    }
    else if(generalPseudoToUse.isEmpty() == false)
    {
        listOfContainerForTopicsInfos.back()->setNewCookiesForInfo(generalCookieList, generalPseudoToUse, typeOfSaveForPseudo::DEFAULT);
    }

    listOfContainerForTopicsInfos.back()->getShowTopic().addSearchPath(imageDownloadTool.getPathOfTmpDir());
    if(themeImgDir.isEmpty() == false)
    {
        listOfContainerForTopicsInfos.back()->getShowTopic().addSearchPath(themeImgDir);
    }

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
    connect(&listOfContainerForTopicsInfos.back()->getShowTopic(), &showTopicClass::newCookiesHaveToBeSet, this, &tabViewTopicInfosClass::setNewCookiesForPseudo);
    tabList.addTab(listOfContainerForTopicsInfos.back(), "Onglet " + QString::number(listOfContainerForTopicsInfos.size()));
    tabList.setCurrentIndex(listOfContainerForTopicsInfos.size() - 1);
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

const QList<QNetworkCookie>& tabViewTopicInfosClass::getListOfCookiesOfCurrentTab() const
{
    return getConstCurrentWidget()->getConstShowTopic().getListOfCookies();
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
    return listOfContainerForTopicsInfos.at(tabList.currentIndex());
}

containerForTopicsInfosClass* tabViewTopicInfosClass::getCurrentWidget()
{
    return listOfContainerForTopicsInfos.at(tabList.currentIndex());
}

void tabViewTopicInfosClass::currentTabChanged(int newIndex)
{
    if(newIndex == -1)
    {
        newIndex = tabList.currentIndex();
    }

    tabList.setTabIcon(newIndex, QIcon());

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
        tabList.removeTab(index);
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
            tabList.setTabText(i, topicName);
        }
    }
}

void tabViewTopicInfosClass::setNewCookiesForPseudo()
{
    QObject* senderObject = sender();

    for(containerForTopicsInfosClass*& thisContainer : listOfContainerForTopicsInfos)
    {
        if(senderObject == &thisContainer->getShowTopic())
        {
            emit newCookiesHaveToBeSet(thisContainer->getShowTopic().getPseudoUsed(), thisContainer->getShowTopic().getListOfCookies());
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
                tabList.setTabIcon(i, QIcon(alertImage));
            }
        }
    }

    emit newMessagesAvailable();
}

void tabViewTopicInfosClass::downloadStickersIfNeeded(QStringList listOfStickersNeedToBeCheck)
{
    imageDownloadTool.checkAndStartDownloadMissingImages(listOfStickersNeedToBeCheck, "sticker");
}

void tabViewTopicInfosClass::downloadNoelshackImagesIfNeeded(QStringList listOfNoelshackImagesNeedToBeCheck)
{
    imageDownloadTool.checkAndStartDownloadMissingImages(listOfNoelshackImagesNeedToBeCheck, "noelshack");
}

void tabViewTopicInfosClass::downloadAvatarsIfNeeded(QStringList listOfAvatarsNeedToBeCheck)
{
    imageDownloadTool.checkAndStartDownloadMissingImages(listOfAvatarsNeedToBeCheck, "avatar");
}

void tabViewTopicInfosClass::updateImagesIfNeeded()
{
    if(typeOfImageRefresh == 2)
    {
        getCurrentWidget()->getShowTopic().relayoutDocumentHack();
    }
    else if(typeOfImageRefresh == 1)
    {
        if(imageDownloadTool.getNumberOfDownloadRemaining() == 0)
        {
            getCurrentWidget()->getShowTopic().relayoutDocumentHack();
        }
    }
}
