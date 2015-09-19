#include "respawnIrc.hpp"
#include "connectWindow.hpp"
#include "selectTopicWindow.hpp"
#include "ignoreListWindow.hpp"
#include "chooseNumberWindow.hpp"
#include "captchaWindow.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

const QString respawnIrcClass::currentVersionName("v1.13.1");

respawnIrcClass::respawnIrcClass(QWidget* parent) : QWidget(parent), checkUpdate(this, currentVersionName)
{
    tabList.setTabsClosable(true);
    sendButton.setText("Envoyer");
    sendButton.setAutoDefault(true);
    alertImage.load("ressources/alert.png");
    replyForSendMessage = 0;
    inSending = false;
    isInEdit = false;

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
    myAccountListWindow->exec();
}

void respawnIrcClass::showSelectTopic()
{
    selectTopicWindow* mySelectTopicWindow = new selectTopicWindow(getCurrentWidget()->getTopicLink(), this);
    QObject::connect(mySelectTopicWindow, &selectTopicWindow::newTopicSelected, this, &respawnIrcClass::setNewTopic);
    mySelectTopicWindow->exec();
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
    chooseNumberWindowClass* myChooseNumberWindow = new chooseNumberWindowClass(2500, 10000, settingToolClass::getThisIntOption("updateTopicTime"), this);
    QObject::connect(myChooseNumberWindow, &chooseNumberWindowClass::newNumberSet, this, &respawnIrcClass::setUpdateTopicTime);
    myChooseNumberWindow->exec();
}

void respawnIrcClass::showNumberOfMessageShowedFirstTimeWindow()
{
    chooseNumberWindowClass* myChooseNumberWindow = new chooseNumberWindowClass(1, 40, settingToolClass::getThisIntOption("numberOfMessageShowedFirstTime"), this);
    QObject::connect(myChooseNumberWindow, &chooseNumberWindowClass::newNumberSet, this, &respawnIrcClass::setNumberOfMessageShowedFirstTime);
    myChooseNumberWindow->exec();
}

void respawnIrcClass::showAbout()
{
    QString versionName = currentVersionName;
    QMessageBox messageBox;
    versionName.remove(0, 1);
    messageBox.information(this, "A propos de RespawnIRC", "<b>RespawnIRC version " + versionName + ".</b><br /><br />" +
                           "Ce logiciel à été developpé à l'aide Qt 5.<br />" +
                           "Lien du dépôt github : <a href=\"https://github.com/LEpigeon888/RespawnIRC\">https://github.com/LEpigeon888/RespawnIRC</a>");
}

void respawnIrcClass::addNewTab()
{
    listOfShowTopicMessages.push_back(new showTopicMessagesClass(&listOfIgnoredPseudo, &listOfColorPseudo, this));

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
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newNumberOfConnected, this, &respawnIrcClass::setNewNumberOfConnectedAndPseudoUsed);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newMessagesAvailable, this, &respawnIrcClass::warnUserForNewMessages);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::newNameForTopic, this, &respawnIrcClass::setNewTopicName);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::setEditInfo, this, &respawnIrcClass::setInfoForEditMessage);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::quoteThisMessage, this, &respawnIrcClass::quoteThisMessage);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::addToBlacklist, this, &respawnIrcClass::addThisPeudoToBlacklist);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::editThisMessage, this, &respawnIrcClass::setEditMessage);
    QObject::connect(listOfShowTopicMessages.back(), &showTopicMessagesClass::openThisTopicInNewTab, this, &respawnIrcClass::addNewTabWithTopic);
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

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}

void respawnIrcClass::setNumberOfMessageShowedFirstTime(int newNumber)
{
    settingToolClass::saveThisOption("numberOfMessageShowedFirstTime", newNumber);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}

void respawnIrcClass::setShowQuoteButton(bool newVal)
{
    settingToolClass::saveThisOption("showQuoteButton", newVal);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}

void respawnIrcClass::setShowBlacklistButton(bool newVal)
{
    settingToolClass::saveThisOption("showBlacklistButton", newVal);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}

void respawnIrcClass::setShowEditButton(bool newVal)
{
    settingToolClass::saveThisOption("showEditButton", newVal);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}


void respawnIrcClass::setShowTextDecorationButton(bool newVal)
{
    setButtonInButtonLayoutVisible(newVal);

    settingToolClass::saveThisOption("showTextDecorationButton", newVal);
}

void respawnIrcClass::setShowListOfTopic(bool newVal)
{
    settingToolClass::saveThisOption("showListOfTopic", newVal);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}

void respawnIrcClass::setMultilineEdit(bool newVal)
{
    messageLine.setTextEditSelected(newVal);
    messageLine.setFocus();

    settingToolClass::saveThisOption("setMultilineEdit", newVal);
}

void respawnIrcClass::setLoadTwoLastPage(bool newVal)
{
    settingToolClass::saveThisOption("loadTwoLastPage", newVal);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}

void respawnIrcClass::setIgnoreNetworkError(bool newVal)
{
    settingToolClass::saveThisOption("ignoreNetworkError", newVal);

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->updateSettingInfo();
    }
}

void respawnIrcClass::setSearchForUpdateAtLaunch(bool newVal)
{
    settingToolClass::saveThisOption("searchForUpdateAtLaunch", newVal);
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

void respawnIrcClass::setNewTopic(QString newTopic)
{
    getCurrentWidget()->setNewTopic(newTopic);
    listOfTopicLink[tabList.currentIndex()] = newTopic;

    settingToolClass::saveListOfTopicLink(listOfTopicLink);
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
        textToShow += getCurrentWidget()->getNumberOfConnected();

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
    QApplication::alert(this);
    QObject* senderObject = sender();

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
    if(replyForSendMessage == 0 && getCurrentWidget()->getPseudoUsed().isEmpty() == false && getCurrentWidget()->getTopicLink().isEmpty() == false)
    {
        QNetworkRequest request;
        QString data;
        QString captchaLink;

        cookieListForPostMsg = getCurrentWidget()->getListOfCookies();
        networkManager.clearAccessCache();
        networkManager.setCookieJar(new QNetworkCookieJar(this));
        networkManager.cookieJar()->setCookiesFromUrl(cookieListForPostMsg, QUrl("http://www.jeuxvideo.com"));

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

        replyForSendMessage = networkManager.post(request, data.toLatin1());
        QObject::connect(replyForSendMessage, &QNetworkReply::finished, this, &respawnIrcClass::deleteReplyForSendMessage);
    }
}

void respawnIrcClass::deleteReplyForSendMessage()
{
    QString source = replyForSendMessage->readAll();
    replyForSendMessage->deleteLater();
    replyForSendMessage = 0;
    captchaCode.clear();

    if(source.size() == 0 || (isInEdit == true && source.startsWith("{\"erreur\":[]") == true))
    {
        messageLine.clear();
    }
    else if(source.contains("<div class=\"alert-row\"> Le code de confirmation est incorrect. </div>") == true ||
            (isInEdit == true && source.startsWith("{\"erreur\":[\"Le code de confirmation est incorrect.\"]") == true))
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le code de confirmation est incorrect.");
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le message n'a pas été envoyé.");
    }

    sendButton.setEnabled(true);
    inSending = false;

    if(isInEdit == true)
    {
        messageLine.clear();
        sendButton.setText("Envoyer");
        isInEdit = false;
    }

    getCurrentWidget()->startGetMessage();
    messageLine.setFocus();
}

void respawnIrcClass::setEditMessage(int idOfMessageToEdit)
{
    if(inSending == false)
    {
        if(isInEdit == false)
        {
            sendButton.setEnabled(false);
            sendButton.setText("Editer");
            if(getCurrentWidget()->getEditInfo(idOfMessageToEdit) == false)
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

void respawnIrcClass::setInfoForEditMessage(int idOfMessageEdit, QString messageEdit, QString infoToSend, QString captchaLink)
{
    if(messageEdit.isEmpty() == false)
    {
        messageLine.clear();
        messageLine.insertText(messageEdit);
        dataForEditLastMessage = "id_message=" + QString::number(idOfMessageEdit) + "&" + infoToSend;
        captchaLinkForEditLastMessage = captchaLink;
        isInEdit = true;
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

    if(dataInHtml.contains("<img src=\"smileys/"))
    {
        QTextDocument doc;
        QString newDataInHtml = dataInHtml;
        //a changer, si possible
        newDataInHtml.replace("<img src=\"smileys/1.gif\" />", ":)");
        newDataInHtml.replace("<img src=\"smileys/2.gif\" />", ":question:");
        newDataInHtml.replace("<img src=\"smileys/3.gif\" />", ":g)");
        newDataInHtml.replace("<img src=\"smileys/4.gif\" />", ":d)");
        newDataInHtml.replace("<img src=\"smileys/5.gif\" />", ":cd:");
        newDataInHtml.replace("<img src=\"smileys/6.gif\" />", ":globe:");
        newDataInHtml.replace("<img src=\"smileys/7.gif\" />", ":p)");
        newDataInHtml.replace("<img src=\"smileys/8.gif\" />", ":malade:");
        newDataInHtml.replace("<img src=\"smileys/9.gif\" />", ":pacg:");
        newDataInHtml.replace("<img src=\"smileys/10.gif\" />", ":pacd:");
        newDataInHtml.replace("<img src=\"smileys/11.gif\" />", ":noel:");
        newDataInHtml.replace("<img src=\"smileys/12.gif\" />", ":o))");
        newDataInHtml.replace("<img src=\"smileys/13.gif\" />", ":snif2:");
        newDataInHtml.replace("<img src=\"smileys/14.gif\" />", ":-(");
        newDataInHtml.replace("<img src=\"smileys/15.gif\" />", ":-((");
        newDataInHtml.replace("<img src=\"smileys/16.gif\" />", ":mac:");
        newDataInHtml.replace("<img src=\"smileys/17.gif\" />", ":gba:");
        newDataInHtml.replace("<img src=\"smileys/18.gif\" />", ":hap:");
        newDataInHtml.replace("<img src=\"smileys/19.gif\" />", ":nah:");
        newDataInHtml.replace("<img src=\"smileys/20.gif\" />", ":snif:");
        newDataInHtml.replace("<img src=\"smileys/21.gif\" />", ":mort:");
        newDataInHtml.replace("<img src=\"smileys/22.gif\" />", ":ouch:");
        newDataInHtml.replace("<img src=\"smileys/23.gif\" />", ":-)))");
        newDataInHtml.replace("<img src=\"smileys/24.gif\" />", ":content:");
        newDataInHtml.replace("<img src=\"smileys/25.gif\" />", ":nonnon:");
        newDataInHtml.replace("<img src=\"smileys/26.gif\" />", ":cool:");
        newDataInHtml.replace("<img src=\"smileys/27.gif\" />", ":sleep:");
        newDataInHtml.replace("<img src=\"smileys/28.gif\" />", ":doute:");
        newDataInHtml.replace("<img src=\"smileys/29.gif\" />", ":hello:");
        newDataInHtml.replace("<img src=\"smileys/30.gif\" />", ":honte:");
        newDataInHtml.replace("<img src=\"smileys/31.gif\" />", ":-p");
        newDataInHtml.replace("<img src=\"smileys/32.gif\" />", ":lol:");
        newDataInHtml.replace("<img src=\"smileys/33.gif\" />", ":non2:");
        newDataInHtml.replace("<img src=\"smileys/34.gif\" />", ":monoeil:");
        newDataInHtml.replace("<img src=\"smileys/35.gif\" />", ":non:");
        newDataInHtml.replace("<img src=\"smileys/36.gif\" />", ":ok:");
        newDataInHtml.replace("<img src=\"smileys/37.gif\" />", ":oui:");
        newDataInHtml.replace("<img src=\"smileys/38.gif\" />", ":rechercher:");
        newDataInHtml.replace("<img src=\"smileys/39.gif\" />", ":rire:");
        newDataInHtml.replace("<img src=\"smileys/40.gif\" />", ":-D");
        newDataInHtml.replace("<img src=\"smileys/41.gif\" />", ":rire2:");
        newDataInHtml.replace("<img src=\"smileys/42.gif\" />", ":salut:");
        newDataInHtml.replace("<img src=\"smileys/43.gif\" />", ":sarcastic:");
        newDataInHtml.replace("<img src=\"smileys/44.gif\" />", ":up:");
        newDataInHtml.replace("<img src=\"smileys/45.gif\" />", ":(");
        newDataInHtml.replace("<img src=\"smileys/46.gif\" />", ":-)");
        newDataInHtml.replace("<img src=\"smileys/47.gif\" />", ":peur:");
        newDataInHtml.replace("<img src=\"smileys/48.gif\" />", ":bye:");
        newDataInHtml.replace("<img src=\"smileys/49.gif\" />", ":dpdr:");
        newDataInHtml.replace("<img src=\"smileys/50.gif\" />", ":fou:");
        newDataInHtml.replace("<img src=\"smileys/51.gif\" />", ":gne:");
        newDataInHtml.replace("<img src=\"smileys/52.gif\" />", ":dehors:");
        newDataInHtml.replace("<img src=\"smileys/53.gif\" />", ":fier:");
        newDataInHtml.replace("<img src=\"smileys/54.gif\" />", ":coeur:");
        newDataInHtml.replace("<img src=\"smileys/55.gif\" />", ":rouge:");
        newDataInHtml.replace("<img src=\"smileys/56.gif\" />", ":sors:");
        newDataInHtml.replace("<img src=\"smileys/57.gif\" />", ":ouch:");
        newDataInHtml.replace("<img src=\"smileys/58.gif\" />", ":merci:");
        newDataInHtml.replace("<img src=\"smileys/59.gif\" />", ":svp:");
        newDataInHtml.replace("<img src=\"smileys/60.gif\" />", ":ange:");
        newDataInHtml.replace("<img src=\"smileys/61.gif\" />", ":diable:");
        newDataInHtml.replace("<img src=\"smileys/62.gif\" />", ":gni:");
        newDataInHtml.replace("<img src=\"smileys/63.gif\" />", ":spoiler:");
        newDataInHtml.replace("<img src=\"smileys/64.gif\" />", ":hs:");
        newDataInHtml.replace("<img src=\"smileys/65.gif\" />", ":desole:");
        newDataInHtml.replace("<img src=\"smileys/66.gif\" />", ":fete:");
        newDataInHtml.replace("<img src=\"smileys/67.gif\" />", ":sournois:");
        newDataInHtml.replace("<img src=\"smileys/68.gif\" />", ":hum:");
        newDataInHtml.replace("<img src=\"smileys/69.gif\" />", ":bravo:");
        newDataInHtml.replace("<img src=\"smileys/70.gif\" />", ":banzai:");
        newDataInHtml.replace("<img src=\"smileys/71.gif\" />", ":bave:");
        newDataInHtml.replace("<img src=\"smileys/cimer.gif\" />", ":cimer:");
        newDataInHtml.replace("<img src=\"smileys/ddb.gif\" />", ":ddb:");
        newDataInHtml.replace("<img src=\"smileys/hapoelparty.gif\" />", ":hapoelparty:");
        newDataInHtml.replace("<img src=\"smileys/loveyou.gif\" />", ":loveyou:");
        newDataInHtml.replace("<img src=\"smileys/nyu.gif\" />", ":cute:");
        newDataInHtml.replace("<img src=\"smileys/objection.gif\" />", ":objection:");
        newDataInHtml.replace("<img src=\"smileys/pave.gif\" />", ":pave:");
        newDataInHtml.replace("<img src=\"smileys/pf.gif\" />", ":pf:");
        newDataInHtml.replace("<img src=\"smileys/play.gif\" />", ":play:");
        newDataInHtml.replace("<img src=\"smileys/siffle.gif\" />", ":siffle:");
        //fin
        doc.setHtml(newDataInHtml);
        QString text = doc.toPlainText();

        if(text != data->text())
        {
            QMimeData* newData = new QMimeData();
            newData->setHtml(dataInHtml);
            newData->setText(doc.toPlainText());
            clipboard->setMimeData(newData);
        }
    }
}

void respawnIrcClass::focusInEvent(QFocusEvent* event)
{
    (void)event;
    messageLine.setFocus();
}
