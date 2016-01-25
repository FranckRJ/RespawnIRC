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
const QString respawnIrcClass::currentVersionName("v1.19.7");

respawnIrcClass::respawnIrcClass(QWidget* parent) : QWidget(parent), checkUpdate(this, currentVersionName)
{
    tabList.setTabsClosable(true);
    sendButton.setText("Envoyer");
    sendButton.setAutoDefault(true);
    sendButton.setObjectName("sendButton");
    alertImage.load(QCoreApplication::applicationDirPath() + "/ressources/alert.png");
    replyForSendMessage = 0;
    idOfLastMessageEdit = 0;
    inSending = false;
    isInEdit = false;
    beepWhenWarn = true;
    warnUser = true;

    networkManager = new QNetworkAccessManager(this);

    addButtonToButtonLayout();

    QHBoxLayout* infoLayout = new QHBoxLayout();
    infoLayout->addWidget(&messagesStatus);
    infoLayout->addWidget(&numberOfConnectedAndPseudoUsed, 1, Qt::AlignRight);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&tabList, 0, 0, 1, 2);
    mainLayout->addLayout(buttonLayout, 1, 0, 1, 2, Qt::AlignLeft);
    mainLayout->addWidget(&messageLine, 2, 0);
    mainLayout->addWidget(&sendButton, 2, 1);
    mainLayout->addLayout(infoLayout, 3, 0, 1, 2);

    setLayout(mainLayout);

    QObject::connect(&sendButton, &QPushButton::pressed, this, &respawnIrcClass::postMessage);
    QObject::connect(&messageLine, &multiTypeTextBoxClass::returnPressed, &sendButton, &QPushButton::click);
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

    if(listOfShowTopicMessages.isEmpty() == true)
    {
        addNewTab();
    }

    messagesStatus.setText(getCurrentWidget()->getMessagesStatus());
    setNewNumberOfConnectedAndPseudoUsed();

    if(settingToolClass::getThisBoolOption("showTextDecorationButton") == false)
    {
        setShowTextDecorationButton(false);
    }

    setMultilineEdit(settingToolClass::getThisBoolOption("setMultilineEdit"));

    if(settingToolClass::getThisBoolOption("searchForUpdateAtLaunch") == true)
    {
        checkUpdate.startDownloadOfLatestUpdatePage();
    }
}

showTopicMessagesClass* respawnIrcClass::getCurrentWidget()
{
    return listOfShowTopicMessages.at(tabList.currentIndex());
}

multiTypeTextBoxClass* respawnIrcClass::getMessageLine()
{
    return &messageLine;
}

QString respawnIrcClass::buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput)
{
    QString data;

    for(int i = 0; i < listOfInput.size(); ++i)
    {
        data += listOfInput.at(i).first + "=" + listOfInput.at(i).second + "&";
    }

    data += "message_topic=" + QUrl::toPercentEncoding(messageLine.text());

    if(captchaCode.isEmpty() == false)
    {
        data += "&fs_ccode=" + captchaCode;
    }

    data += "&form_alias_rang=1";

    return data;
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

    QObject::connect(buttonBold, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addBold);
    QObject::connect(buttonItalic, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addItalic);
    QObject::connect(buttonUnderline, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addUnderLine);
    QObject::connect(buttonStrike, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addStrike);
    QObject::connect(buttonUList, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addUList);
    QObject::connect(buttonOList, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addOListe);
    QObject::connect(buttonQuote, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addQuote);
    QObject::connect(buttonCode, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addCode);
    QObject::connect(buttonSpoil, &QPushButton::pressed, &messageLine, &multiTypeTextBoxClass::addSpoil);
}

void respawnIrcClass::selectThisTab(int number)
{
    if(listOfShowTopicMessages.size() > number)
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
        getCurrentWidget()->setNewTopic(vectorOfFavoriteLink.at(index));
        listOfTopicLink[tabList.currentIndex()] = vectorOfFavoriteLink.at(index);

        settingToolClass::saveListOfTopicLink(listOfTopicLink);
    }
}

QString respawnIrcClass::addThisFavorite(int index)
{
    if(getCurrentWidget()->getTopicLink().isEmpty() == false && getCurrentWidget()->getTopicName().isEmpty() == false)
    {
        vectorOfFavoriteLink[index] = getCurrentWidget()->getTopicLink();
        settingToolClass::saveThisOption("favoriteLink" + QString::number(index), getCurrentWidget()->getTopicLink());
        settingToolClass::saveThisOption("favoriteName" + QString::number(index), getCurrentWidget()->getTopicName());

        return getCurrentWidget()->getTopicName();
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
    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
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
    selectTopicWindowClass* mySelectTopicWindow = new selectTopicWindowClass(getCurrentWidget()->getTopicLink(), this);
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
    listOfShowTopicMessages.push_back(new showTopicMessagesClass(&listOfIgnoredPseudo, &listOfColorPseudo, currentThemeName, this));

    if(listOfShowTopicMessages.size() > listOfTopicLink.size())
    {
        listOfTopicLink.push_back(QString());
    }

    if(listOfShowTopicMessages.size() > listOfPseudoForTopic.size())
    {
        listOfPseudoForTopic.push_back(QString());
    }

    if(listOfPseudoForTopic.at(listOfShowTopicMessages.size() - 1).isEmpty() == false)
    {
        for(int j = 0; j < listOfAccount.size(); ++j)
        {
            if(listOfAccount.at(j).pseudo.toLower() == listOfPseudoForTopic.at(listOfShowTopicMessages.size() - 1).toLower())
            {
                listOfShowTopicMessages.back()->setNewCookies(listOfAccount.at(j).listOfCookie, listOfAccount.at(j).pseudo);
                break;
            }
            else if(j == listOfAccount.size() - 1)
            {
                listOfPseudoForTopic[listOfShowTopicMessages.size() - 1].clear();
                settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
            }
        }
    }
    else if(pseudoOfUser.isEmpty() == false)
    {
        listOfShowTopicMessages.back()->setNewCookies(currentCookieList, pseudoOfUser);
    }

    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newMessageStatus, this, &respawnIrcClass::setNewMessageStatus);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newNumberOfConnectedAndMP, this, &respawnIrcClass::setNewNumberOfConnectedAndPseudoUsed);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newMessagesAvailable, this, &respawnIrcClass::warnUserForNewMessages);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newNameForTopic, this, &respawnIrcClass::setNewTopicName);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::setEditInfo, this, &respawnIrcClass::setInfoForEditMessage);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::quoteThisMessage, this, &respawnIrcClass::quoteThisMessage);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::addToBlacklist, this, &respawnIrcClass::addThisPeudoToBlacklist);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::editThisMessage, this, &respawnIrcClass::setEditMessage);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::openThisTopicInNewTab, this, &respawnIrcClass::addNewTabWithTopic);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::topicNeedChanged, this, &respawnIrcClass::setNewTopic);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newCookiesHaveToBeSet, this, &respawnIrcClass::setNewCookiesForPseudo);
    tabList.addTab(listOfShowTopicMessages.back(), "Onglet " + QString::number(listOfShowTopicMessages.size()));
    tabList.setCurrentIndex(listOfShowTopicMessages.size() - 1);
}


void respawnIrcClass::addNewTabWithTopic(QString newTopicLink)
{
    addNewTab();
    setNewTopic(newTopicLink);
}

void respawnIrcClass::removeTab(int index)
{
    if(listOfShowTopicMessages.size() > 1)
    {
        tabList.removeTab(index);
        listOfTopicLink.removeAt(index);
        settingToolClass::saveListOfTopicLink(listOfTopicLink);
        listOfPseudoForTopic.removeAt(index);
        settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
        delete listOfShowTopicMessages.takeAt(index);
        currentTabChanged(-1);
    }
}

void respawnIrcClass::checkForUpdate()
{
    checkUpdate.startDownloadOfLatestUpdatePage(true);
}

void respawnIrcClass::updateTopic()
{
    getCurrentWidget()->startGetMessage();
}

void respawnIrcClass::reloadTopic()
{
    getCurrentWidget()->setNewTopic(listOfTopicLink[tabList.currentIndex()]);
}

void respawnIrcClass::reloadAllTopic()
{
    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->setNewTopic(listOfTopicLink[i]);
    }
}

void respawnIrcClass::goToCurrentTopic()
{
    if(getCurrentWidget()->getTopicLink().isEmpty() == false)
    {
        QDesktopServices::openUrl(QUrl(getCurrentWidget()->getTopicLink()));
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Il n'y a pas de topic.");
    }
}

void respawnIrcClass::goToCurrentForum()
{
    if(parsingToolClass::getForumOfTopic(getCurrentWidget()->getTopicLink()).isEmpty() == false)
    {
        QDesktopServices::openUrl(QUrl(parsingToolClass::getForumOfTopic(getCurrentWidget()->getTopicLink())));
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
    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->setNewCookies(QList<QNetworkCookie>(), "");
        listOfPseudoForTopic[i] = "";
    }
    settingToolClass::saveThisOption("pseudo", pseudoOfUser);
    settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::disconnectFromCurrentTab()
{
    getCurrentWidget()->setNewCookies(QList<QNetworkCookie>(), ".");
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

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        if(listOfShowTopicMessages.at(i)->getPseudoUsed().toLower() == thisPseudo.toLower())
        {
            listOfShowTopicMessages.at(i)->setNewCookies(currentCookieList, pseudoOfUser);
            listOfPseudoForTopic[i] = "";
        }
    }
    settingToolClass::saveListOfPseudoForTopic(listOfPseudoForTopic);
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::quoteThisMessage(QString messageToQuote)
{
    if(messageLine.text().isEmpty() == false)
    {
        messageLine.insertText("\n");
    }
    messageLine.insertText(messageToQuote);
    messageLine.insertText("\n\n");
    messageLine.setFocus();
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
            setMultilineEdit(newVal);
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
            messageLine.settingsChanged();
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

void respawnIrcClass::setMultilineEdit(bool newVal)
{
    messageLine.setTextEditSelected(newVal);
    messageLine.setFocus();
}

void respawnIrcClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo)
{
    if(newCookies.isEmpty() == false)
    {
        currentCookieList = newCookies;
        pseudoOfUser = newPseudoOfUser;
        for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
        {
            listOfShowTopicMessages.at(i)->setNewCookies(currentCookieList, newPseudoOfUser);
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
    getCurrentWidget()->setNewCookies(newCookies, newPseudoOfUser);
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

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        if(senderObject == listOfShowTopicMessages.at(i))
        {
            QString pseudoUsed = listOfShowTopicMessages.at(i)->getPseudoUsed();

            for(int j = 0; j < listOfAccount.size(); ++j)
            {
                if(listOfAccount.at(j).pseudo.toLower() == pseudoUsed.toLower())
                {
                    listOfAccount[j].listOfCookie = listOfShowTopicMessages.at(i)->getListOfCookies();
                    saveListOfAccount();
                    break;
                }
            }
        }
    }
}

void respawnIrcClass::setNewTopic(QString newTopic)
{
    getCurrentWidget()->setNewTopic(newTopic);
    listOfTopicLink[tabList.currentIndex()] = newTopic;

    settingToolClass::saveListOfTopicLink(listOfTopicLink);
}

void respawnIrcClass::setNewTheme(QString newThemeName)
{
    currentThemeName = newThemeName;
    emit themeChanged(currentThemeName);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->setNewTheme(currentThemeName);
        listOfShowTopicMessages.at(i)->setNewTopic(listOfTopicLink[i]);
    }

    messageLine.styleChanged();

    settingToolClass::saveThisOption("themeUsed", currentThemeName);
}

void respawnIrcClass::reloadTheme()
{
    setNewTheme(currentThemeName);
}

void respawnIrcClass::setCodeForCaptcha(QString code)
{
    captchaCode = code;
    postMessage();
}

void respawnIrcClass::setNewMessageStatus()
{
    messagesStatus.setText(getCurrentWidget()->getMessagesStatus());
}

void respawnIrcClass::setNewNumberOfConnectedAndPseudoUsed()
{
    QString textToShow;

    if(listOfShowTopicMessages.isEmpty() == false)
    {
        textToShow += getCurrentWidget()->getNumberOfConnectedAndMP();

        if(getCurrentWidget()->getPseudoUsed().isEmpty() == false)
        {
            if(textToShow.isEmpty() == false)
            {
                textToShow += " - ";
            }
            textToShow += getCurrentWidget()->getPseudoUsed();
        }
    }

    numberOfConnectedAndPseudoUsed.setText(textToShow);
}

void respawnIrcClass::setNewTopicName(QString topicName)
{
    QObject* senderObject = sender();

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        if(senderObject == listOfShowTopicMessages.at(i))
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

    if(senderObject != getCurrentWidget())
    {
        for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
        {
            if(senderObject == listOfShowTopicMessages.at(i))
            {
                tabList.setTabIcon(i, QIcon(alertImage));
            }
        }
    }
}

void respawnIrcClass::currentTabChanged(int newIndex)
{
    if(inSending == false)
    {
        sendButton.setText("Envoyer");
        sendButton.setEnabled(true);

        if(isInEdit == true)
        {
            messageLine.clear();
            isInEdit = false;
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

void respawnIrcClass::postMessage()
{
    if(networkManager == 0)
    {
        networkManager = new QNetworkAccessManager(this);
    }

    if(replyForSendMessage == 0 && getCurrentWidget()->getPseudoUsed().isEmpty() == false && getCurrentWidget()->getTopicLink().isEmpty() == false)
    {
        QNetworkRequest request;
        QString data;
        QString captchaLink;

        cookieListForPostMsg = getCurrentWidget()->getListOfCookies();
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(cookieListForPostMsg, QUrl("http://www.jeuxvideo.com"));

        if(isInEdit == true)
        {
            captchaLink = captchaLinkForEditLastMessage;
            request = parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com/forums/ajax_edit_message.php");
        }
        else
        {
            captchaLink = getCurrentWidget()->getCaptchaLink();
            request = parsingToolClass::buildRequestWithThisUrl(getCurrentWidget()->getTopicLink());
        }

        if(captchaLink.isEmpty() == false && captchaCode.isEmpty() == true)
        {
            captchaWindowClass* myCaptchaWindow = new captchaWindowClass(captchaLink, cookieListForPostMsg, this);
            QObject::connect(myCaptchaWindow, &captchaWindowClass::codeForCaptcha, this, &respawnIrcClass::setCodeForCaptcha);
            oldListOfInput = getCurrentWidget()->getListOfInput();
            myCaptchaWindow->exec();
            return;
        }

        sendButton.setEnabled(false);
        inSending = true;

        if(isInEdit == false)
        {
            if(captchaCode.isEmpty() == true)
            {
                data = buildDataWithThisListOfInput(getCurrentWidget()->getListOfInput());
            }
            else
            {
                data = buildDataWithThisListOfInput(oldListOfInput);
            }
        }
        else
        {
            data = "message_topic=" + QUrl::toPercentEncoding(messageLine.text());
            data += "&" + dataForEditLastMessage;

            if(captchaCode.isEmpty() == false)
            {
                data += "&fs_ccode=" + captchaCode;
            }
        }

        replyForSendMessage = networkManager->post(request, data.toLatin1());

        if(replyForSendMessage->isOpen() == true)
        {
            QObject::connect(replyForSendMessage, &QNetworkReply::finished, this, &respawnIrcClass::deleteReplyForSendMessage);
        }
        else
        {
            deleteReplyForSendMessage();
            networkManager->deleteLater();
            networkManager = 0;
        }

    }
}

void respawnIrcClass::deleteReplyForSendMessage()
{
    bool dontEraseEditMessage = false;
    QString source;
    if(replyForSendMessage->isReadable() == true)
    {
        if(replyForSendMessage->rawHeaderList().isEmpty() == false)
        {
            source = replyForSendMessage->readAll();
        }
        else
        {
            source = "weshgrotavu";
        }
    }
    else
    {
        source = "lolmdr";
    }
    replyForSendMessage->deleteLater();
    replyForSendMessage = 0;
    captchaCode.clear();

    if(source.size() == 0 || (isInEdit == true && source.startsWith("{\"erreur\":[]") == true))
    {
        messageLine.clear();
    }
    else if(source.contains("<div class=\"alert-row\"> Le captcha est invalide. </div>") == true ||
            (isInEdit == true && source.startsWith("{\"erreur\":[\"Le captcha est incorrect.\"]") == true))
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Depuis la mise à jour de JVC les captchas ne sont plus supportés, "
                                           "veuillez attendre quelques secondes avant d'envoyer votre message.");
        dontEraseEditMessage = true;
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", parsingToolClass::getErrorMessage(source));
        dontEraseEditMessage = true;
    }

    sendButton.setEnabled(true);
    inSending = false;

    if(isInEdit == true)
    {
        sendButton.setText("Envoyer");
        isInEdit = false;

        if(dontEraseEditMessage == true)
        {
            setEditMessage(idOfLastMessageEdit, false);
        }
    }

    getCurrentWidget()->startGetMessage();
    messageLine.setFocus();
}

void respawnIrcClass::editLastMessage()
{
    setEditMessage(0, true);
}


void respawnIrcClass::setEditMessage(int idOfMessageToEdit, bool useMessageEdit)
{
    if(inSending == false)
    {
        if(isInEdit == false)
        {
            sendButton.setEnabled(false);
            sendButton.setText("Editer");
            if(getCurrentWidget()->getEditInfo(idOfMessageToEdit, useMessageEdit) == false)
            {
                QMessageBox messageBox;
                messageBox.warning(this, "Erreur", "Impossible d'éditer ce message.");
                sendButton.setText("Envoyer");
                sendButton.setEnabled(true);
            }
        }
        else
        {
            isInEdit = false;
            sendButton.setText("Envoyer");
            sendButton.setEnabled(true);
            messageLine.clear();
        }
    }
}

void respawnIrcClass::setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString infoToSend, QString captchaLink, bool useMessageEdit)
{
    if(messageEdit.isEmpty() == false)
    {
        if(useMessageEdit == true)
        {
            messageLine.clear();
            messageLine.insertText(messageEdit);
        }
        dataForEditLastMessage = "id_message=" + QString::number(idOfMessageEdit) + "&" + infoToSend;
        captchaLinkForEditLastMessage = captchaLink;
        isInEdit = true;
        idOfLastMessageEdit = idOfMessageEdit;
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Impossible d'éditer ce message.");
        sendButton.setText("Envoyer");
    }

    sendButton.setEnabled(true);
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
    messageLine.setFocus();
}
