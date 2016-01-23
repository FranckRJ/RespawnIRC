#include <QSound>

#include "respawnIrc.hpp"
#include "connectWindow.hpp"
#include "selectTopicWindow.hpp"
#include "selectThemeWindow.hpp"
#include "ignoreListWindow.hpp"
#include "chooseNumberWindow.hpp"
#include "captchaWindow.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"
#include "styleTool.hpp"

QRegularExpression respawnIrcClass::expForSmileyToCode("<img src=\"ressources/smileys/(.*?)\\..*?\" />", QRegularExpression::OptimizeOnFirstUsageOption);
const QString respawnIrcClass::currentVersionName("v2.0");

respawnIrcClass::respawnIrcClass(QWidget* parent) : QWidget(parent), checkUpdate(this, currentVersionName)
{
    tabList.setTabsClosable(true);
    alertImage.load(QCoreApplication::applicationDirPath() + "/ressources/alert.png");
    beepWhenWarn = true;
    warnUser = true;

    addButtonToButtonLayout();

    QHBoxLayout* infoLayout = new QHBoxLayout();
    infoLayout->addWidget(&messagesStatus);
    infoLayout->addWidget(&numberOfConnectedAndPseudoUsed, 1, Qt::AlignRight);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&tabList, 0, 0);
    mainLayout->addLayout(buttonLayout, 1, 0, 1, 1, Qt::AlignLeft);
    mainLayout->addWidget(&sendMessages, 2, 0);
    mainLayout->addLayout(infoLayout, 3, 0);

    setLayout(mainLayout);

    QObject::connect(&sendMessages, &sendMessagesClass::needToPostMessage, this, &respawnIrcClass::messageHaveToBePosted);
    QObject::connect(&sendMessages, &sendMessagesClass::needToSetEditMessage, this, &respawnIrcClass::setEditMessage);
    QObject::connect(&sendMessages, &sendMessagesClass::needToGetMessages, this, &respawnIrcClass::updateTopic);
    QObject::connect(&tabList, &QTabWidget::currentChanged, this, &respawnIrcClass::currentTabChanged);
    QObject::connect(&tabList, &QTabWidget::tabCloseRequested, this, &respawnIrcClass::removeTab);

    loadSettings();
}

void respawnIrcClass::loadSettings()
{
    beepWhenWarn = settingToolClass::getThisBoolOption("beepWhenWarn");
    warnUser = settingToolClass::getThisBoolOption("warnUser");

    for(int i = 0; i < 10; ++i)
    {
        vectorOfFavoriteLink.push_back(settingToolClass::getThisStringOption("favoriteLink" + QString::number(i)));
    }

    pseudoOfUser = settingToolClass::getThisStringOption("pseudo");
    listOfAccount = settingToolClass::getListOfAccount();

    if(listOfAccount.isEmpty() == true)
    {
        pseudoOfUser.clear();
    }

    for(int i = 0; i < listOfAccount.size(); ++i)
    {
        if(pseudoOfUser.toLower() == listOfAccount.at(i).pseudo.toLower())
        {
            setNewCookies(listOfAccount.at(i).listOfCookie, pseudoOfUser, false, false);
            break;
        }
        else if(i == listOfAccount.size() - 1)
        {
            pseudoOfUser.clear();
            settingToolClass::saveThisOption("pseudo", pseudoOfUser);
        }
    }

    listOfIgnoredPseudo = settingToolClass::getListOfIgnoredPseudo();
    listOfColorPseudo = settingToolClass::getListOfColorPseudo();
    listOfTopicLink = settingToolClass::getListOfTopicLink();
    listOfPseudoForTopic = settingToolClass::getListOfPseudoForTopic();

    for(int i = 0; i < listOfTopicLink.size(); ++i)
    {
        addNewTab();
        tabList.setCurrentIndex(i);
        setNewTopic(listOfTopicLink.at(i));
        tabList.setCurrentIndex(0);
    }

    if(listOfContainerForTopicsInfos.isEmpty() == true)
    {
        addNewTab();
    }

    messagesStatus.setText(getCurrentWidget()->getShowTopicMessages().getMessagesStatus());
    setNewNumberOfConnectedAndPseudoUsed();

    if(settingToolClass::getThisBoolOption("showTextDecorationButton") == false)
    {
        setShowTextDecorationButton(false);
    }

    sendMessages.setMultilineEdit(settingToolClass::getThisBoolOption("setMultilineEdit"));

    if(settingToolClass::getThisBoolOption("searchForUpdateAtLaunch") == true)
    {
        checkUpdate.startDownloadOfLatestUpdatePage();
    }
}

containerForTopicsInfosClass* respawnIrcClass::getCurrentWidget()
{
    return listOfContainerForTopicsInfos.at(tabList.currentIndex());
}

multiTypeTextBoxClass* respawnIrcClass::getMessageLine()
{
    return sendMessages.getMessageLine();
}

void respawnIrcClass::addButtonToButtonLayout()
{
    QPushButton* buttonBold = new QPushButton("B", this);
    QPushButton* buttonItalic = new QPushButton("I", this);
    QPushButton* buttonUnderline = new QPushButton("U", this);
    QPushButton* buttonStrike = new QPushButton("S", this);
    QPushButton* buttonUList = new QPushButton("*", this);
    QPushButton* buttonOList = new QPushButton("#", this);
    QPushButton* buttonQuote = new QPushButton("\" \"", this);
    QPushButton* buttonCode = new QPushButton("< >", this);
    QPushButton* buttonSpoil = new QPushButton("spoil", this);
    buttonBold->setMaximumWidth(buttonBold->fontMetrics().boundingRect(buttonBold->text()).width() + 10);
    buttonBold->setMaximumHeight(20);
    buttonItalic->setMaximumWidth(buttonItalic->fontMetrics().boundingRect(buttonItalic->text()).width() + 10);
    buttonItalic->setMaximumHeight(20);
    buttonUnderline->setMaximumWidth(buttonUnderline->fontMetrics().boundingRect(buttonUnderline->text()).width() + 10);
    buttonUnderline->setMaximumHeight(20);
    buttonStrike->setMaximumWidth(buttonStrike->fontMetrics().boundingRect(buttonStrike->text()).width() + 10);
    buttonStrike->setMaximumHeight(20);
    buttonUList->setMaximumWidth(buttonUList->fontMetrics().boundingRect(buttonUList->text()).width() + 10);
    buttonUList->setMaximumHeight(20);
    buttonOList->setMaximumWidth(buttonOList->fontMetrics().boundingRect(buttonOList->text()).width() + 10);
    buttonOList->setMaximumHeight(20);
    buttonQuote->setMaximumWidth(buttonQuote->fontMetrics().boundingRect(buttonQuote->text()).width() + 10);
    buttonQuote->setMaximumHeight(20);
    buttonCode->setMaximumWidth(buttonCode->fontMetrics().boundingRect(buttonCode->text()).width() + 10);
    buttonCode->setMaximumHeight(20);
    buttonSpoil->setMaximumWidth(buttonSpoil->fontMetrics().boundingRect(buttonSpoil->text()).width() + 10);
    buttonSpoil->setMaximumHeight(20);

    buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(buttonBold);
    buttonLayout->addWidget(buttonItalic);
    buttonLayout->addWidget(buttonUnderline);
    buttonLayout->addWidget(buttonStrike);
    buttonLayout->addWidget(buttonUList);
    buttonLayout->addWidget(buttonOList);
    buttonLayout->addWidget(buttonQuote);
    buttonLayout->addWidget(buttonCode);
    buttonLayout->addWidget(buttonSpoil);

    QObject::connect(buttonBold, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addBold);
    QObject::connect(buttonItalic, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addItalic);
    QObject::connect(buttonUnderline, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addUnderLine);
    QObject::connect(buttonStrike, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addStrike);
    QObject::connect(buttonUList, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addUList);
    QObject::connect(buttonOList, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addOListe);
    QObject::connect(buttonQuote, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addQuote);
    QObject::connect(buttonCode, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addCode);
    QObject::connect(buttonSpoil, &QPushButton::pressed, getMessageLine(), &multiTypeTextBoxClass::addSpoil);
}

void respawnIrcClass::selectThisTab(int number)
{
    if(listOfContainerForTopicsInfos.size() > number)
    {
        tabList.setCurrentIndex(number);
    }
}

void respawnIrcClass::setButtonInButtonLayoutVisible(bool visible)
{
    for(int i = 0; i < buttonLayout->count(); ++i)
    {
        buttonLayout->itemAt(i)->widget()->setVisible(visible);
    }
}

void respawnIrcClass::useThisFavorite(int index)
{
    if(vectorOfFavoriteLink.at(index).isEmpty() == false)
    {
        getCurrentWidget()->setNewTopicForInfo(vectorOfFavoriteLink.at(index));
        listOfTopicLink[tabList.currentIndex()] = vectorOfFavoriteLink.at(index);

        settingToolClass::saveListOfTopicLink(listOfTopicLink);
    }
}

QString respawnIrcClass::addThisFavorite(int index)
{
    if(getCurrentWidget()->getShowTopicMessages().getTopicLink().isEmpty() == false && getCurrentWidget()->getShowTopicMessages().getTopicName().isEmpty() == false)
    {
        vectorOfFavoriteLink[index] = getCurrentWidget()->getShowTopicMessages().getTopicLink();
        settingToolClass::saveThisOption("favoriteLink" + QString::number(index), getCurrentWidget()->getShowTopicMessages().getTopicLink());
        settingToolClass::saveThisOption("favoriteName" + QString::number(index), getCurrentWidget()->getShowTopicMessages().getTopicName());

        return getCurrentWidget()->getShowTopicMessages().getTopicName();
    }
    else
    {
        return "";
    }
}

void respawnIrcClass::delThisFavorite(int index)
{
    vectorOfFavoriteLink[index].clear();
    settingToolClass::saveThisOption("favoriteLink" + QString::number(index), "");
    settingToolClass::saveThisOption("favoriteName" + QString::number(index), "");
}

void respawnIrcClass::updateSettingInfoForList()
{
    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        listOfContainerForTopicsInfos.at(i)->updateSettingsForInfo();
    }
}

void respawnIrcClass::showConnect()
{
    connectWindowClass* myConnectWindow = new connectWindowClass(this);
    QObject::connect(myConnectWindow, &connectWindowClass::newCookiesAvailable, this, &respawnIrcClass::setNewCookies);
    myConnectWindow->exec();
}

void respawnIrcClass::showAccountListWindow()
{
    accountListWindowClass* myAccountListWindow = new accountListWindowClass(&listOfAccount, this);
    QObject::connect(myAccountListWindow, &accountListWindowClass::listHasChanged, this, &respawnIrcClass::saveListOfAccount);
    QObject::connect(myAccountListWindow, &accountListWindowClass::useThisAccount, this, &respawnIrcClass::setNewCookies);
    QObject::connect(myAccountListWindow, &accountListWindowClass::useThisAccountForOneTopic, this, &respawnIrcClass::setNewCookiesForCurrentTopic);
    QObject::connect(myAccountListWindow, &accountListWindowClass::eraseThisPseudo, this, &respawnIrcClass::disconnectFromThisPseudo);
    myAccountListWindow->exec();
}

void respawnIrcClass::showSelectTopic()
{
    selectTopicWindowClass* mySelectTopicWindow = new selectTopicWindowClass(getCurrentWidget()->getShowTopicMessages().getTopicLink(), this);
    QObject::connect(mySelectTopicWindow, &selectTopicWindowClass::newTopicSelected, this, &respawnIrcClass::setNewTopic);
    mySelectTopicWindow->exec();
}

void respawnIrcClass::showSelectTheme()
{
    selectThemeWindowClass* mySelectThemeWindow = new selectThemeWindowClass(currentThemeName, this);
    QObject::connect(mySelectThemeWindow, &selectThemeWindowClass::newThemeSelected, this, &respawnIrcClass::setNewTheme);
    mySelectThemeWindow->exec();
}

void respawnIrcClass::showIgnoreListWindow()
{
    ignoreListWindowClass* myIgnoreListWindow = new ignoreListWindowClass(&listOfIgnoredPseudo, this);
    QObject::connect(myIgnoreListWindow, &ignoreListWindowClass::listHasChanged, this, &respawnIrcClass::saveListOfIgnoredPseudo);
    myIgnoreListWindow->exec();
}

void respawnIrcClass::showColorPseudoListWindow()
{
    colorPseudoListWindowClass* myColorPseudoListWindow = new colorPseudoListWindowClass(&listOfColorPseudo, this);
    QObject::connect(myColorPseudoListWindow, &colorPseudoListWindowClass::listHasChanged, this, &respawnIrcClass::saveListOfColorPseudo);
    myColorPseudoListWindow->exec();
}

void respawnIrcClass::showUpdateTopicTimeWindow()
{
    chooseNumberWindowClass* myChooseNumberWindow = new chooseNumberWindowClass(500, 10000, settingToolClass::getThisIntOption("updateTopicTime"), this);
    QObject::connect(myChooseNumberWindow, &chooseNumberWindowClass::newNumberSet, this, &respawnIrcClass::setUpdateTopicTime);
    myChooseNumberWindow->exec();
}

void respawnIrcClass::showNumberOfMessageShowedFirstTimeWindow()
{
    chooseNumberWindowClass* myChooseNumberWindow = new chooseNumberWindowClass(1, 40, settingToolClass::getThisIntOption("numberOfMessageShowedFirstTime"), this);
    QObject::connect(myChooseNumberWindow, &chooseNumberWindowClass::newNumberSet, this, &respawnIrcClass::setNumberOfMessageShowedFirstTime);
    myChooseNumberWindow->exec();
}

void respawnIrcClass::showStickersSizeWindow()
{
    chooseNumberWindowClass* myChooseNumberWindow = new chooseNumberWindowClass(0, 1000, settingToolClass::getThisIntOption("stickersSize"), this);
    QObject::connect(myChooseNumberWindow, &chooseNumberWindowClass::newNumberSet, this, &respawnIrcClass::setStickersSize);
    myChooseNumberWindow->exec();
}

void respawnIrcClass::showAbout()
{
    QString versionName = currentVersionName;
    QMessageBox messageBox;
    versionName.remove(0, 1);
    messageBox.information(this, "A propos de RespawnIRC", "<b>RespawnIRC version " + versionName + ".</b><br /><br />" +
                           "Ce logiciel à été developpé à l'aide de Qt 5 ainsi que de Hunspell <a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"http://hunspell.sourceforge.net/\">http://hunspell.sourceforge.net/</a>.<br />" +
                           "Lien du dépôt github : <a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"https://github.com/LEpigeon888/RespawnIRC\">https://github.com/LEpigeon888/RespawnIRC</a><br />" +
                           "Lien du site : <a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"http://lepigeon888.github.io/RespawnIRC/\">http://lepigeon888.github.io/RespawnIRC/</a>");
}

void respawnIrcClass::addNewTab()
{
    listOfContainerForTopicsInfos.push_back(new containerForTopicsInfosClass(&listOfIgnoredPseudo, &listOfColorPseudo, currentThemeName, this));

    if(listOfContainerForTopicsInfos.size() > listOfTopicLink.size())
    {
        listOfTopicLink.push_back(QString());
    }

    if(listOfContainerForTopicsInfos.size() > listOfPseudoForTopic.size())
    {
        listOfPseudoForTopic.push_back(QString());
    }

    if(listOfPseudoForTopic.at(listOfContainerForTopicsInfos.size() - 1).isEmpty() == false)
    {
        for(int j = 0; j < listOfAccount.size(); ++j)
        {
            if(listOfAccount.at(j).pseudo.toLower() == listOfPseudoForTopic.at(listOfContainerForTopicsInfos.size() - 1).toLower())
            {
                listOfContainerForTopicsInfos.back()->setNewCookiesForInfo(listOfAccount.at(j).listOfCookie, listOfAccount.at(j).pseudo);
                break;
            }
            else if(j == listOfAccount.size() - 1)
            {
                listOfPseudoForTopic[listOfContainerForTopicsInfos.size() - 1].clear();
                settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
            }
        }
    }
    else if(pseudoOfUser.isEmpty() == false)
    {
        listOfContainerForTopicsInfos.back()->setNewCookiesForInfo(currentCookieList, pseudoOfUser);
    }

    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::newMessageStatus, this, &respawnIrcClass::setNewMessageStatus);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::newNumberOfConnectedAndMP, this, &respawnIrcClass::setNewNumberOfConnectedAndPseudoUsed);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::newMessagesAvailable, this, &respawnIrcClass::warnUserForNewMessages);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::newNameForTopic, this, &respawnIrcClass::setNewTopicName);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::setEditInfo, &sendMessages, &sendMessagesClass::setInfoForEditMessage);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::quoteThisMessage, &sendMessages, &sendMessagesClass::quoteThisMessage);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::addToBlacklist, this, &respawnIrcClass::addThisPeudoToBlacklist);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::editThisMessage, this, &respawnIrcClass::setEditMessage);
    QObject::connect(listOfContainerForTopicsInfos.back(), &containerForTopicsInfosClass::openThisTopicInNewTab, this, &respawnIrcClass::addNewTabWithTopic);
    QObject::connect(listOfContainerForTopicsInfos.back(), &containerForTopicsInfosClass::topicNeedChanged, this, &respawnIrcClass::setNewTopic);
    QObject::connect(&listOfContainerForTopicsInfos.back()->getShowTopicMessages(), &showTopicMessagesClass::newCookiesHaveToBeSet, this, &respawnIrcClass::setNewCookiesForPseudo);
    tabList.addTab(listOfContainerForTopicsInfos.back(), "Onglet " + QString::number(listOfContainerForTopicsInfos.size()));
    tabList.setCurrentIndex(listOfContainerForTopicsInfos.size() - 1);
}


void respawnIrcClass::addNewTabWithTopic(QString newTopicLink)
{
    addNewTab();
    setNewTopic(newTopicLink);
}

void respawnIrcClass::removeTab(int index)
{
    if(listOfContainerForTopicsInfos.size() > 1)
    {
        tabList.removeTab(index);
        listOfTopicLink.removeAt(index);
        settingToolClass::saveListOfTopicLink(listOfTopicLink);
        listOfPseudoForTopic.removeAt(index);
        settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
        delete listOfContainerForTopicsInfos.takeAt(index);
        currentTabChanged(-1);
    }
}

void respawnIrcClass::checkForUpdate()
{
    checkUpdate.startDownloadOfLatestUpdatePage(true);
}

void respawnIrcClass::updateTopic()
{
    getCurrentWidget()->getShowTopicMessages().startGetMessage();
}

void respawnIrcClass::reloadTopic()
{
    getCurrentWidget()->setNewTopicForInfo(listOfTopicLink[tabList.currentIndex()]);
}

void respawnIrcClass::reloadAllTopic()
{
    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        listOfContainerForTopicsInfos.at(i)->setNewTopicForInfo(listOfTopicLink[i]);
    }
}

void respawnIrcClass::goToCurrentTopic()
{
    if(getCurrentWidget()->getShowTopicMessages().getTopicLink().isEmpty() == false)
    {
        QDesktopServices::openUrl(QUrl(getCurrentWidget()->getShowTopicMessages().getTopicLink()));
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Il n'y a pas de topic.");
    }
}

void respawnIrcClass::goToCurrentForum()
{
    if(parsingToolClass::getForumOfTopic(getCurrentWidget()->getShowTopicMessages().getTopicLink()).isEmpty() == false)
    {
        QDesktopServices::openUrl(QUrl(parsingToolClass::getForumOfTopic(getCurrentWidget()->getShowTopicMessages().getTopicLink())));
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Il n'y a pas de forum.");
    }
}

void respawnIrcClass::disconnectFromAllTabs()
{
    currentCookieList = QList<QNetworkCookie>();
    pseudoOfUser = "";
    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        listOfContainerForTopicsInfos.at(i)->setNewCookiesForInfo(QList<QNetworkCookie>(), "");
        listOfPseudoForTopic[i] = "";
    }
    settingToolClass::saveThisOption("pseudo", pseudoOfUser);
    settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::disconnectFromCurrentTab()
{
    getCurrentWidget()->setNewCookiesForInfo(QList<QNetworkCookie>(), ".");
    listOfPseudoForTopic[tabList.currentIndex()] = ".";
    settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::disconnectFromThisPseudo(QString thisPseudo)
{
    if(pseudoOfUser.toLower() == thisPseudo.toLower())
    {
        currentCookieList = QList<QNetworkCookie>();
        pseudoOfUser = "";
    }

    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        if(listOfContainerForTopicsInfos.at(i)->getShowTopicMessages().getPseudoUsed().toLower() == thisPseudo.toLower())
        {
            listOfContainerForTopicsInfos.at(i)->setNewCookiesForInfo(currentCookieList, pseudoOfUser);
            listOfPseudoForTopic[i] = "";
        }
    }
    settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::addThisPeudoToBlacklist(QString pseudoToAdd)
{
    if(listOfIgnoredPseudo.indexOf(pseudoToAdd) == -1)
    {
        listOfIgnoredPseudo.append(pseudoToAdd);
        saveListOfIgnoredPseudo();
    }
}

void respawnIrcClass::setUpdateTopicTime(int newTime)
{
    settingToolClass::saveThisOption("updateTopicTime", newTime);

    updateSettingInfoForList();
}

void respawnIrcClass::setNumberOfMessageShowedFirstTime(int newNumber)
{
    settingToolClass::saveThisOption("numberOfMessageShowedFirstTime", newNumber);

    updateSettingInfoForList();
}

void respawnIrcClass::setStickersSize(int newSize)
{
    settingToolClass::saveThisOption("stickersSize", newSize);

    updateSettingInfoForList();
}

void respawnIrcClass::setThisBoolOption(bool newVal)
{
    QObject* senderObject = sender();

    if(senderObject != 0)
    {
        settingToolClass::saveThisOption(senderObject->objectName(), newVal);

        if(senderObject->objectName() == "showTextDecorationButton")
        {
            setShowTextDecorationButton(newVal);
        }
        else if(senderObject->objectName() == "setMultilineEdit")
        {
            sendMessages.setMultilineEdit(newVal);
        }
        else if(senderObject->objectName() == "beepWhenWarn")
        {
            beepWhenWarn = newVal;
        }
        else if(senderObject->objectName() == "warnUser")
        {
            warnUser = newVal;
        }
        else if(senderObject->objectName() == "useSpellChecker")
        {
            sendMessages.settingsChanged();
        }
        else
        {
            updateSettingInfoForList();
        }
    }
}

void respawnIrcClass::setShowTextDecorationButton(bool newVal)
{
    setButtonInButtonLayoutVisible(newVal);
}

void respawnIrcClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo)
{
    if(newCookies.isEmpty() == false)
    {
        currentCookieList = newCookies;
        pseudoOfUser = newPseudoOfUser;
        for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
        {
            listOfContainerForTopicsInfos.at(i)->setNewCookiesForInfo(currentCookieList, newPseudoOfUser);
        }
        setNewNumberOfConnectedAndPseudoUsed();

        if(saveAccountList == true)
        {
            accountListWindowClass::addAcountToThisList(currentCookieList, pseudoOfUser, &listOfAccount);
            settingToolClass::saveListOfAccount(listOfAccount);
        }

        if(savePseudo == true)
        {
            settingToolClass::saveThisOption("pseudo", pseudoOfUser);
            for(int i = 0; i < listOfPseudoForTopic.size(); ++i)
            {
                listOfPseudoForTopic[i].clear();
            }
            settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
        }
    }
}

void respawnIrcClass::setNewCookiesForCurrentTopic(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool savePseudo)
{
    getCurrentWidget()->setNewCookiesForInfo(newCookies, newPseudoOfUser);
    setNewNumberOfConnectedAndPseudoUsed();

    if(savePseudo == true)
    {
        listOfPseudoForTopic[tabList.currentIndex()] = newPseudoOfUser;
        settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
    }
}

void respawnIrcClass::setNewCookiesForPseudo()
{
    QObject* senderObject = sender();

    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        if(senderObject == &listOfContainerForTopicsInfos.at(i)->getShowTopicMessages())
        {
            QString pseudoUsed = listOfContainerForTopicsInfos.at(i)->getShowTopicMessages().getPseudoUsed();

            for(int j = 0; j < listOfAccount.size(); ++j)
            {
                if(listOfAccount.at(j).pseudo.toLower() == pseudoUsed.toLower())
                {
                    listOfAccount[j].listOfCookie = listOfContainerForTopicsInfos.at(i)->getShowTopicMessages().getListOfCookies();
                    saveListOfAccount();
                    break;
                }
            }
        }
    }
}

void respawnIrcClass::setNewTopic(QString newTopic)
{
    getCurrentWidget()->setNewTopicForInfo(newTopic);
    listOfTopicLink[tabList.currentIndex()] = newTopic;

    settingToolClass::saveListOfTopicLink(listOfTopicLink);
}

void respawnIrcClass::setNewTheme(QString newThemeName)
{
    currentThemeName = newThemeName;
    emit themeChanged(currentThemeName);

    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        listOfContainerForTopicsInfos.at(i)->setNewThemeForInfo(currentThemeName);
        listOfContainerForTopicsInfos.at(i)->setNewTopicForInfo(listOfTopicLink[i]);
    }

    sendMessages.styleChanged();

    settingToolClass::saveThisOption("themeUsed", currentThemeName);
}

void respawnIrcClass::reloadTheme()
{
    setNewTheme(currentThemeName);
}

void respawnIrcClass::setNewMessageStatus()
{
    messagesStatus.setText(getCurrentWidget()->getShowTopicMessages().getMessagesStatus());
}

void respawnIrcClass::setNewNumberOfConnectedAndPseudoUsed()
{
    QString textToShow;

    if(listOfContainerForTopicsInfos.isEmpty() == false)
    {
        textToShow += getCurrentWidget()->getShowTopicMessages().getNumberOfConnectedAndMP();

        if(getCurrentWidget()->getShowTopicMessages().getPseudoUsed().isEmpty() == false)
        {
            if(textToShow.isEmpty() == false)
            {
                textToShow += " - ";
            }
            textToShow += getCurrentWidget()->getShowTopicMessages().getPseudoUsed();
        }
    }

    numberOfConnectedAndPseudoUsed.setText(textToShow);
}

void respawnIrcClass::setNewTopicName(QString topicName)
{
    QObject* senderObject = sender();

    for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
    {
        if(senderObject == &listOfContainerForTopicsInfos.at(i)->getShowTopicMessages())
        {
            tabList.setTabText(i, topicName);
        }
    }
}

void respawnIrcClass::saveListOfAccount()
{
    settingToolClass::saveListOfAccount(listOfAccount);
}

void respawnIrcClass::saveListOfIgnoredPseudo()
{
    settingToolClass::saveListOfIgnoredPseudo(listOfIgnoredPseudo);
}

void respawnIrcClass::saveListOfColorPseudo()
{
    settingToolClass::saveListOfColorPseudo(listOfColorPseudo);
}

void respawnIrcClass::warnUserForNewMessages()
{
    QObject* senderObject = sender();

    if(warnUser == true)
    {
        QApplication::alert(this);
    }

    if(QApplication::focusWidget() == 0 && beepWhenWarn == true)
    {
        QSound::play(QCoreApplication::applicationDirPath() + "/ressources/beep.wav");
    }

    if(senderObject != &getCurrentWidget()->getShowTopicMessages())
    {
        for(int i = 0; i < listOfContainerForTopicsInfos.size(); ++i)
        {
            if(senderObject == &listOfContainerForTopicsInfos.at(i)->getShowTopicMessages())
            {
                tabList.setTabIcon(i, QIcon(alertImage));
            }
        }
    }
}

void respawnIrcClass::currentTabChanged(int newIndex)
{
    if(sendMessages.getIsSending() == false)
    {
        sendMessages.setTextSendButton("Envoyer");
        sendMessages.setEnableSendButton(true);

        if(sendMessages.getIsInEdit() == true)
        {
            sendMessages.clearMessageLine();
            sendMessages.setIsInEdit(false);
        }
    }

    if(newIndex == -1)
    {
        newIndex = tabList.currentIndex();
    }

    setNewMessageStatus();
    setNewNumberOfConnectedAndPseudoUsed();
    tabList.setTabIcon(newIndex, QIcon());
}

void respawnIrcClass::messageHaveToBePosted()
{
    sendMessages.postMessage(getCurrentWidget()->getShowTopicMessages().getPseudoUsed(), getCurrentWidget()->getShowTopicMessages().getTopicLink(),
                             getCurrentWidget()->getShowTopicMessages().getListOfCookies(), getCurrentWidget()->getShowTopicMessages().getListOfInput());
}

void respawnIrcClass::editLastMessage()
{
    setEditMessage(0, true);
}


void respawnIrcClass::setEditMessage(int idOfMessageToEdit, bool useMessageEdit)
{
    if(sendMessages.getIsSending() == false)
    {
        if(sendMessages.getIsInEdit() == false)
        {
            sendMessages.setEnableSendButton(false);
            sendMessages.setTextSendButton("Editer");
            if(getCurrentWidget()->getShowTopicMessages().getEditInfo(idOfMessageToEdit, useMessageEdit) == false)
            {
                QMessageBox messageBox;
                messageBox.warning(this, "Erreur", "Impossible d'éditer ce message.");
                sendMessages.setTextSendButton("Envoyer");
                sendMessages.setEnableSendButton(true);
            }
        }
        else
        {
            sendMessages.setIsInEdit(false);
            sendMessages.setTextSendButton("Envoyer");
            sendMessages.setEnableSendButton(true);
            sendMessages.clearMessageLine();
        }
    }
}

void respawnIrcClass::clipboardChanged()
{
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* data = clipboard->mimeData();
    QString dataInHtml = data->html();

    if(dataInHtml.contains("<img src=\"ressources/smileys/"))
    {
        QTextDocument doc;
        QMimeData* newData = new QMimeData();
        //a changer, si possible
        dataInHtml.replace("<img src=\"ressources/smileys/1.gif\" />", ":)");
        dataInHtml.replace("<img src=\"ressources/smileys/2.gif\" />", ":question:");
        dataInHtml.replace("<img src=\"ressources/smileys/3.gif\" />", ":g)");
        dataInHtml.replace("<img src=\"ressources/smileys/4.gif\" />", ":d)");
        dataInHtml.replace("<img src=\"ressources/smileys/5.gif\" />", ":cd:");
        dataInHtml.replace("<img src=\"ressources/smileys/6.gif\" />", ":globe:");
        dataInHtml.replace("<img src=\"ressources/smileys/7.gif\" />", ":p)");
        dataInHtml.replace("<img src=\"ressources/smileys/8.gif\" />", ":malade:");
        dataInHtml.replace("<img src=\"ressources/smileys/9.gif\" />", ":pacg:");
        dataInHtml.replace("<img src=\"ressources/smileys/10.gif\" />", ":pacd:");
        dataInHtml.replace("<img src=\"ressources/smileys/11.gif\" />", ":noel:");
        dataInHtml.replace("<img src=\"ressources/smileys/12.gif\" />", ":o))");
        dataInHtml.replace("<img src=\"ressources/smileys/13.gif\" />", ":snif2:");
        dataInHtml.replace("<img src=\"ressources/smileys/14.gif\" />", ":-(");
        dataInHtml.replace("<img src=\"ressources/smileys/15.gif\" />", ":-((");
        dataInHtml.replace("<img src=\"ressources/smileys/16.gif\" />", ":mac:");
        dataInHtml.replace("<img src=\"ressources/smileys/17.gif\" />", ":gba:");
        dataInHtml.replace("<img src=\"ressources/smileys/18.gif\" />", ":hap:");
        dataInHtml.replace("<img src=\"ressources/smileys/19.gif\" />", ":nah:");
        dataInHtml.replace("<img src=\"ressources/smileys/20.gif\" />", ":snif:");
        dataInHtml.replace("<img src=\"ressources/smileys/21.gif\" />", ":mort:");
        dataInHtml.replace("<img src=\"ressources/smileys/22.gif\" />", ":ouch:");
        dataInHtml.replace("<img src=\"ressources/smileys/23.gif\" />", ":-)))");
        dataInHtml.replace("<img src=\"ressources/smileys/24.gif\" />", ":content:");
        dataInHtml.replace("<img src=\"ressources/smileys/25.gif\" />", ":nonnon:");
        dataInHtml.replace("<img src=\"ressources/smileys/26.gif\" />", ":cool:");
        dataInHtml.replace("<img src=\"ressources/smileys/27.gif\" />", ":sleep:");
        dataInHtml.replace("<img src=\"ressources/smileys/28.gif\" />", ":doute:");
        dataInHtml.replace("<img src=\"ressources/smileys/29.gif\" />", ":hello:");
        dataInHtml.replace("<img src=\"ressources/smileys/30.gif\" />", ":honte:");
        dataInHtml.replace("<img src=\"ressources/smileys/31.gif\" />", ":-p");
        dataInHtml.replace("<img src=\"ressources/smileys/32.gif\" />", ":lol:");
        dataInHtml.replace("<img src=\"ressources/smileys/33.gif\" />", ":non2:");
        dataInHtml.replace("<img src=\"ressources/smileys/34.gif\" />", ":monoeil:");
        dataInHtml.replace("<img src=\"ressources/smileys/35.gif\" />", ":non:");
        dataInHtml.replace("<img src=\"ressources/smileys/36.gif\" />", ":ok:");
        dataInHtml.replace("<img src=\"ressources/smileys/37.gif\" />", ":oui:");
        dataInHtml.replace("<img src=\"ressources/smileys/38.gif\" />", ":rechercher:");
        dataInHtml.replace("<img src=\"ressources/smileys/39.gif\" />", ":rire:");
        dataInHtml.replace("<img src=\"ressources/smileys/40.gif\" />", ":-D");
        dataInHtml.replace("<img src=\"ressources/smileys/41.gif\" />", ":rire2:");
        dataInHtml.replace("<img src=\"ressources/smileys/42.gif\" />", ":salut:");
        dataInHtml.replace("<img src=\"ressources/smileys/43.gif\" />", ":sarcastic:");
        dataInHtml.replace("<img src=\"ressources/smileys/44.gif\" />", ":up:");
        dataInHtml.replace("<img src=\"ressources/smileys/45.gif\" />", ":(");
        dataInHtml.replace("<img src=\"ressources/smileys/46.gif\" />", ":-)");
        dataInHtml.replace("<img src=\"ressources/smileys/47.gif\" />", ":peur:");
        dataInHtml.replace("<img src=\"ressources/smileys/48.gif\" />", ":bye:");
        dataInHtml.replace("<img src=\"ressources/smileys/49.gif\" />", ":dpdr:");
        dataInHtml.replace("<img src=\"ressources/smileys/50.gif\" />", ":fou:");
        dataInHtml.replace("<img src=\"ressources/smileys/51.gif\" />", ":gne:");
        dataInHtml.replace("<img src=\"ressources/smileys/52.gif\" />", ":dehors:");
        dataInHtml.replace("<img src=\"ressources/smileys/53.gif\" />", ":fier:");
        dataInHtml.replace("<img src=\"ressources/smileys/54.gif\" />", ":coeur:");
        dataInHtml.replace("<img src=\"ressources/smileys/55.gif\" />", ":rouge:");
        dataInHtml.replace("<img src=\"ressources/smileys/56.gif\" />", ":sors:");
        dataInHtml.replace("<img src=\"ressources/smileys/57.gif\" />", ":ouch:");
        dataInHtml.replace("<img src=\"ressources/smileys/58.gif\" />", ":merci:");
        dataInHtml.replace("<img src=\"ressources/smileys/59.gif\" />", ":svp:");
        dataInHtml.replace("<img src=\"ressources/smileys/60.gif\" />", ":ange:");
        dataInHtml.replace("<img src=\"ressources/smileys/61.gif\" />", ":diable:");
        dataInHtml.replace("<img src=\"ressources/smileys/62.gif\" />", ":gni:");
        dataInHtml.replace("<img src=\"ressources/smileys/63.gif\" />", ":spoiler:");
        dataInHtml.replace("<img src=\"ressources/smileys/64.gif\" />", ":hs:");
        dataInHtml.replace("<img src=\"ressources/smileys/65.gif\" />", ":desole:");
        dataInHtml.replace("<img src=\"ressources/smileys/66.gif\" />", ":fete:");
        dataInHtml.replace("<img src=\"ressources/smileys/67.gif\" />", ":sournois:");
        dataInHtml.replace("<img src=\"ressources/smileys/68.gif\" />", ":hum:");
        dataInHtml.replace("<img src=\"ressources/smileys/69.gif\" />", ":bravo:");
        dataInHtml.replace("<img src=\"ressources/smileys/70.gif\" />", ":banzai:");
        dataInHtml.replace("<img src=\"ressources/smileys/71.gif\" />", ":bave:");
        parsingToolClass::replaceWithCapNumber(dataInHtml, expForSmileyToCode, 1, ":", ":");
        //fin
        doc.setHtml(dataInHtml);
        newData->setHtml(dataInHtml);
        newData->setText(doc.toPlainText());
        clipboard->setMimeData(newData);
    }
}

void respawnIrcClass::focusInEvent(QFocusEvent* event)
{
    (void)event;
    sendMessages.setFocus();
}
