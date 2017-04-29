#include <QSound>
#include <QObject>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDesktopServices>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QTextDocument>
#include <QGridLayout>
#include <QPushButton>

#include "respawnIrc.hpp"
#include "webNavigator.hpp"
#include "connectWindow.hpp"
#include "selectTopicWindow.hpp"
#include "selectThemeWindow.hpp"
#include "selectStickerWindow.hpp"
#include "preferencesWindow.hpp"
#include "ignoreListWindow.hpp"
#include "manageShortcutWindow.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"
#include "styleTool.hpp"
#include "configDependentVar.hpp"

const QString respawnIrcClass::currentVersionName("v2.8.3");

namespace
{
    QRegularExpression expForSmileyToCode(R"rgx(<img src="resources/smileys/(.*?)\..*?" />)rgx", configDependentVar::regexpBaseOptions);
    QRegularExpression expForNoelshackToLink(R"rgx(<a href="(http[^n]*noelshack\.com[^"]*)"><img src="img/image\.noelshack\.com[^"]*".*?</a>)rgx", configDependentVar::regexpBaseOptions);
    QRegularExpression expForStickerToCode(R"rgx(<img src="resources/stickers/(.*?)\.png"[^>]*>)rgx", configDependentVar::regexpBaseOptions);
    QRegularExpression expForImageTag(R"rgx(<img[^>]*>)rgx", configDependentVar::regexpBaseOptions);
}

respawnIrcClass::respawnIrcClass(QWidget* parent) : QWidget(parent)
{
    showTopicClass::createThread(this);
    sendMessages = new sendMessagesClass(this);
    tabViewTopicInfos = new tabViewTopicInfosClass(&listOfIgnoredPseudo, &listOfColorPseudo, &listOfAccount, this);
    checkUpdate = new checkUpdateClass(this, currentVersionName);
    messagesStatus = new QLabel(this);
    numberOfConnectedAndPseudoUsed = new QLabel(this);
    addButtonToButtonLayout();

    QHBoxLayout* infoLayout = new QHBoxLayout();
    infoLayout->addWidget(messagesStatus);
    infoLayout->addWidget(numberOfConnectedAndPseudoUsed, 1, Qt::AlignRight);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(tabViewTopicInfos, 0, 0);
    mainLayout->addLayout(buttonLayout, 1, 0, 1, 1, Qt::AlignLeft);
    mainLayout->addWidget(sendMessages, 2, 0);
    mainLayout->addLayout(infoLayout, 3, 0);

    setLayout(mainLayout);

    connect(sendMessages, &sendMessagesClass::needToPostMessage, this, &respawnIrcClass::messageHaveToBePosted);
    connect(sendMessages, &sendMessagesClass::needToSetEditMessage, this, &respawnIrcClass::setEditMessage);
    connect(sendMessages, &sendMessagesClass::needToGetMessages, tabViewTopicInfos, &tabViewTopicInfosClass::updateCurrentTopic);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::currentTabHasChanged, this, &respawnIrcClass::tabOfTabViewChanged);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::newMessageStatus, this, &respawnIrcClass::setNewMessageStatus);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::newNumberOfConnectedAndMP, this, &respawnIrcClass::setNewNumberOfConnectedAndPseudoUsed);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::newMPAreAvailables, this, &respawnIrcClass::warnUserForNewMP);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::newMessagesAvailable, this, &respawnIrcClass::warnUserForNewMessages);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::setEditInfo, sendMessages, &sendMessagesClass::setInfoForEditMessage);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::quoteThisMessage, sendMessages, &sendMessagesClass::quoteThisMessage);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::addToBlacklist, this, &respawnIrcClass::addThisPeudoToBlacklist);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::editThisMessage, this, &respawnIrcClass::setEditMessage);
    connect(tabViewTopicInfos, &tabViewTopicInfosClass::newCookiesHaveToBeSet, this, &respawnIrcClass::setNewCookiesForPseudo);

    loadSettings();

    showTopicClass::startThread();
}

void respawnIrcClass::doStuffBeforeQuit()
{
    tabViewTopicInfos->doStuffBeforeQuit();

    showTopicClass::stopThread();

    sendMessages->doStuffBeforeQuit();
}

void respawnIrcClass::useThisFavorite(int index)
{
    if(vectorOfFavoriteLink.at(index).isEmpty() == false)
    {
        tabViewTopicInfos->setNewTopicForCurrentTab(vectorOfFavoriteLink.at(index));
    }
}

QString respawnIrcClass::addThisFavorite(int index)
{
    if(tabViewTopicInfos->getTopicLinkFirstPageOfCurrentTab().isEmpty() == false && tabViewTopicInfos->getTopicNameOfCurrentTab().isEmpty() == false)
    {
        vectorOfFavoriteLink[index] = tabViewTopicInfos->getTopicLinkFirstPageOfCurrentTab();
        settingTool::saveThisOption("favoriteLink" + QString::number(index), tabViewTopicInfos->getTopicLinkFirstPageOfCurrentTab());
        settingTool::saveThisOption("favoriteName" + QString::number(index), tabViewTopicInfos->getTopicNameOfCurrentTab());

        return tabViewTopicInfos->getTopicNameOfCurrentTab();
    }
    else
    {
        return "";
    }
}

void respawnIrcClass::delThisFavorite(int index)
{
    vectorOfFavoriteLink[index].clear();
    settingTool::saveThisOption("favoriteLink" + QString::number(index), "");
    settingTool::saveThisOption("favoriteName" + QString::number(index), "");
}

tabViewTopicInfosClass* respawnIrcClass::getTabView()
{
    return tabViewTopicInfos;
}

multiTypeTextBoxClass* respawnIrcClass::getMessageLine()
{
    return sendMessages->getMessageLine();
}

void respawnIrcClass::showWebNavigator()
{
    webNavigatorClass* myWebNavigator = new webNavigatorClass(this, tabViewTopicInfos->getTopicLinkLastPageOfCurrentTab(), tabViewTopicInfos->getListOfCookiesOfCurrentTab());
    myWebNavigator->exec();
}

void respawnIrcClass::showWebNavigatorAtMP()
{
    webNavigatorClass* myWebNavigator = new webNavigatorClass(this, "http://www.jeuxvideo.com/messages-prives/boite-reception.php", tabViewTopicInfos->getListOfCookiesOfCurrentTab());
    myWebNavigator->exec();
}

void respawnIrcClass::showConnect()
{
    connectWindowClass* myConnectWindow = new connectWindowClass(this);
    connect(myConnectWindow, &connectWindowClass::newCookiesAvailable, this, &respawnIrcClass::setNewCookies);
    myConnectWindow->exec();
}

void respawnIrcClass::showAccountListWindow()
{
    accountListWindowClass* myAccountListWindow = new accountListWindowClass(&listOfAccount, this);
    connect(myAccountListWindow, &accountListWindowClass::listHasChanged, this, &respawnIrcClass::saveListOfAccount);
    connect(myAccountListWindow, &accountListWindowClass::useThisAccount, this, &respawnIrcClass::setNewCookies);
    connect(myAccountListWindow, &accountListWindowClass::useThisAccountForOneTopic, this, &respawnIrcClass::setNewCookiesForCurrentTopic);
    connect(myAccountListWindow, &accountListWindowClass::eraseThisPseudo, this, &respawnIrcClass::disconnectFromThisPseudo);
    myAccountListWindow->exec();
}

void respawnIrcClass::showSelectTopic()
{
    selectTopicWindowClass* mySelectTopicWindow = new selectTopicWindowClass(tabViewTopicInfos->getTopicLinkFirstPageOfCurrentTab(), this);
    connect(mySelectTopicWindow, &selectTopicWindowClass::newTopicSelected, tabViewTopicInfos, &tabViewTopicInfosClass::setNewTopicForCurrentTab);
    mySelectTopicWindow->exec();
}

void respawnIrcClass::showSelectSticker()
{
    selectStickerWindowClass* mySelectStickerWindow = new selectStickerWindowClass(this);
    connect(mySelectStickerWindow, &selectStickerWindowClass::addThisSticker, sendMessages->getMessageLine(), &multiTypeTextBoxClass::insertText);
    mySelectStickerWindow->exec();
}

void respawnIrcClass::showSelectTheme()
{
    selectThemeWindowClass* mySelectThemeWindow = new selectThemeWindowClass(currentThemeName, this);
    connect(mySelectThemeWindow, &selectThemeWindowClass::newThemeSelected, this, &respawnIrcClass::setNewTheme);
    mySelectThemeWindow->exec();
}

void respawnIrcClass::showPreferences()
{
    preferenceWindowClass* myPreferencesWindow = new preferenceWindowClass(this);
    connect(myPreferencesWindow, &preferenceWindowClass::newSettingsAvailable, this, &respawnIrcClass::setTheseOptions);
    myPreferencesWindow->exec();
}

void respawnIrcClass::showIgnoreListWindow()
{
    ignoreListWindowClass* myIgnoreListWindow = new ignoreListWindowClass(&listOfIgnoredPseudo, this);
    connect(myIgnoreListWindow, &ignoreListWindowClass::listHasChanged, this, &respawnIrcClass::saveListOfIgnoredPseudo);
    myIgnoreListWindow->exec();
}

void respawnIrcClass::showColorPseudoListWindow()
{
    colorPseudoListWindowClass* myColorPseudoListWindow = new colorPseudoListWindowClass(&listOfColorPseudo, this);
    connect(myColorPseudoListWindow, &colorPseudoListWindowClass::listHasChanged, this, &respawnIrcClass::saveListOfColorPseudo);
    myColorPseudoListWindow->exec();
}

void respawnIrcClass::showManageShortcutWindow()
{
    manageShortcutWindowClass* myManageShortcutWindow = new manageShortcutWindowClass(this);
    myManageShortcutWindow->exec();
}

void respawnIrcClass::showAbout()
{
    QString versionName = currentVersionName;
    versionName.remove(0, 1);
    QMessageBox::information(this, "A propos de RespawnIRC", "<b>RespawnIRC version " + versionName + ".</b><br /><br />" +
                           "Ce logiciel à été developpé à l'aide de Qt 5 ainsi que de Hunspell <a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"http://hunspell.github.io/\">http://hunspell.github.io/</a>.<br />" +
                           "Lien du dépôt github : <a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"https://github.com/FranckRJ/RespawnIRC\">https://github.com/FranckRJ/RespawnIRC</a><br />" +
                           "Lien du site : <a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"https://franckrj.github.io/RespawnIRC/\">https://franckrj.github.io/RespawnIRC/</a><br /><br />"
                           "Nombre de messages que vous avez posté depuis la version 2.2 : <b>" + QString::number(settingTool::getThisIntOption("nbOfMessagesSend").value + sendMessages->getNbOfMessagesSend()) + "</b>");
}

void respawnIrcClass::checkForUpdate()
{
    checkUpdate->startDownloadOfLatestUpdatePage(true);
}

void respawnIrcClass::goToCurrentTopic()
{
    if(tabViewTopicInfos->getTopicLinkLastPageOfCurrentTab().isEmpty() == false)
    {
        QDesktopServices::openUrl(QUrl(tabViewTopicInfos->getTopicLinkLastPageOfCurrentTab()));
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Il n'y a pas de topic.");
    }
}

void respawnIrcClass::goToCurrentForum()
{
    if(parsingTool::getForumOfTopic(tabViewTopicInfos->getTopicLinkFirstPageOfCurrentTab()).isEmpty() == false)
    {
        QDesktopServices::openUrl(QUrl(parsingTool::getForumOfTopic(tabViewTopicInfos->getTopicLinkFirstPageOfCurrentTab())));
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Il n'y a pas de forum.");
    }
}

void respawnIrcClass::disconnectFromAllTabs()
{
    currentCookieList = QList<QNetworkCookie>();
    pseudoOfUser = "";
    tabViewTopicInfos->setNewCookies(currentCookieList, pseudoOfUser, typeOfSaveForPseudo::REMEMBER);
    settingTool::saveThisOption("pseudo", pseudoOfUser);
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::disconnectFromCurrentTab()
{
    tabViewTopicInfos->setNewCookiesForCurrentTab(QList<QNetworkCookie>(), "", typeOfSaveForPseudo::REMEMBER);
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::setNewTheme(QString newThemeName)
{
    currentThemeName = newThemeName;
    tabViewTopicInfos->setNewTheme(currentThemeName);
    styleTool::loadThemeFont(currentThemeName);

    emit themeChanged(currentThemeName);

    sendMessages->styleChanged();

    settingTool::saveThisOption("themeUsed", currentThemeName);
}

void respawnIrcClass::reloadTheme()
{
    setNewTheme(currentThemeName);
}

void respawnIrcClass::messageHaveToBePosted()
{
    sendMessages->postMessage(tabViewTopicInfos->getPseudoUsedOfCurrentTab(), tabViewTopicInfos->getTopicLinkFirstPageOfCurrentTab(),
                             tabViewTopicInfos->getListOfCookiesOfCurrentTab(), tabViewTopicInfos->getListOfInputsOfCurrentTab());
}

void respawnIrcClass::editLastMessage()
{
    setEditMessage(0, true);
}

void respawnIrcClass::clipboardChanged()
{
    if(QApplication::focusWidget() != nullptr)
    {
        QClipboard* clipboard = QApplication::clipboard();
        const QMimeData* data = clipboard->mimeData();
        QString dataInHtml = data->html();

        if(dataInHtml.contains("<img src=\"") && dataInHtml != lastClipboardDataChanged)
        {
            QTextDocument doc;
            QMimeData* newData = new QMimeData();
            //à changer, si possible
            dataInHtml.replace("<img src=\"resources/smileys/1.gif\" />", ":)");
            dataInHtml.replace("<img src=\"resources/smileys/2.gif\" />", ":question:");
            dataInHtml.replace("<img src=\"resources/smileys/3.gif\" />", ":g)");
            dataInHtml.replace("<img src=\"resources/smileys/4.gif\" />", ":d)");
            dataInHtml.replace("<img src=\"resources/smileys/5.gif\" />", ":cd:");
            dataInHtml.replace("<img src=\"resources/smileys/6.gif\" />", ":globe:");
            dataInHtml.replace("<img src=\"resources/smileys/7.gif\" />", ":p)");
            dataInHtml.replace("<img src=\"resources/smileys/8.gif\" />", ":malade:");
            dataInHtml.replace("<img src=\"resources/smileys/9.gif\" />", ":pacg:");
            dataInHtml.replace("<img src=\"resources/smileys/10.gif\" />", ":pacd:");
            dataInHtml.replace("<img src=\"resources/smileys/11.gif\" />", ":noel:");
            dataInHtml.replace("<img src=\"resources/smileys/12.gif\" />", ":o))");
            dataInHtml.replace("<img src=\"resources/smileys/13.gif\" />", ":snif2:");
            dataInHtml.replace("<img src=\"resources/smileys/14.gif\" />", ":-(");
            dataInHtml.replace("<img src=\"resources/smileys/15.gif\" />", ":-((");
            dataInHtml.replace("<img src=\"resources/smileys/16.gif\" />", ":mac:");
            dataInHtml.replace("<img src=\"resources/smileys/17.gif\" />", ":gba:");
            dataInHtml.replace("<img src=\"resources/smileys/18.gif\" />", ":hap:");
            dataInHtml.replace("<img src=\"resources/smileys/19.gif\" />", ":nah:");
            dataInHtml.replace("<img src=\"resources/smileys/20.gif\" />", ":snif:");
            dataInHtml.replace("<img src=\"resources/smileys/21.gif\" />", ":mort:");
            dataInHtml.replace("<img src=\"resources/smileys/22.gif\" />", ":ouch:");
            dataInHtml.replace("<img src=\"resources/smileys/23.gif\" />", ":-)))");
            dataInHtml.replace("<img src=\"resources/smileys/24.gif\" />", ":content:");
            dataInHtml.replace("<img src=\"resources/smileys/25.gif\" />", ":nonnon:");
            dataInHtml.replace("<img src=\"resources/smileys/26.gif\" />", ":cool:");
            dataInHtml.replace("<img src=\"resources/smileys/27.gif\" />", ":sleep:");
            dataInHtml.replace("<img src=\"resources/smileys/28.gif\" />", ":doute:");
            dataInHtml.replace("<img src=\"resources/smileys/29.gif\" />", ":hello:");
            dataInHtml.replace("<img src=\"resources/smileys/30.gif\" />", ":honte:");
            dataInHtml.replace("<img src=\"resources/smileys/31.gif\" />", ":-p");
            dataInHtml.replace("<img src=\"resources/smileys/32.gif\" />", ":lol:");
            dataInHtml.replace("<img src=\"resources/smileys/33.gif\" />", ":non2:");
            dataInHtml.replace("<img src=\"resources/smileys/34.gif\" />", ":monoeil:");
            dataInHtml.replace("<img src=\"resources/smileys/35.gif\" />", ":non:");
            dataInHtml.replace("<img src=\"resources/smileys/36.gif\" />", ":ok:");
            dataInHtml.replace("<img src=\"resources/smileys/37.gif\" />", ":oui:");
            dataInHtml.replace("<img src=\"resources/smileys/38.gif\" />", ":rechercher:");
            dataInHtml.replace("<img src=\"resources/smileys/39.gif\" />", ":rire:");
            dataInHtml.replace("<img src=\"resources/smileys/40.gif\" />", ":-D");
            dataInHtml.replace("<img src=\"resources/smileys/41.gif\" />", ":rire2:");
            dataInHtml.replace("<img src=\"resources/smileys/42.gif\" />", ":salut:");
            dataInHtml.replace("<img src=\"resources/smileys/43.gif\" />", ":sarcastic:");
            dataInHtml.replace("<img src=\"resources/smileys/44.gif\" />", ":up:");
            dataInHtml.replace("<img src=\"resources/smileys/45.gif\" />", ":(");
            dataInHtml.replace("<img src=\"resources/smileys/46.gif\" />", ":-)");
            dataInHtml.replace("<img src=\"resources/smileys/47.gif\" />", ":peur:");
            dataInHtml.replace("<img src=\"resources/smileys/48.gif\" />", ":bye:");
            dataInHtml.replace("<img src=\"resources/smileys/49.gif\" />", ":dpdr:");
            dataInHtml.replace("<img src=\"resources/smileys/50.gif\" />", ":fou:");
            dataInHtml.replace("<img src=\"resources/smileys/51.gif\" />", ":gne:");
            dataInHtml.replace("<img src=\"resources/smileys/52.gif\" />", ":dehors:");
            dataInHtml.replace("<img src=\"resources/smileys/53.gif\" />", ":fier:");
            dataInHtml.replace("<img src=\"resources/smileys/54.gif\" />", ":coeur:");
            dataInHtml.replace("<img src=\"resources/smileys/55.gif\" />", ":rouge:");
            dataInHtml.replace("<img src=\"resources/smileys/56.gif\" />", ":sors:");
            dataInHtml.replace("<img src=\"resources/smileys/57.gif\" />", ":ouch2:");
            dataInHtml.replace("<img src=\"resources/smileys/58.gif\" />", ":merci:");
            dataInHtml.replace("<img src=\"resources/smileys/59.gif\" />", ":svp:");
            dataInHtml.replace("<img src=\"resources/smileys/60.gif\" />", ":ange:");
            dataInHtml.replace("<img src=\"resources/smileys/61.gif\" />", ":diable:");
            dataInHtml.replace("<img src=\"resources/smileys/62.gif\" />", ":gni:");
            dataInHtml.replace("<img src=\"resources/smileys/63.gif\" />", ":spoiler:");
            dataInHtml.replace("<img src=\"resources/smileys/64.gif\" />", ":hs:");
            dataInHtml.replace("<img src=\"resources/smileys/65.gif\" />", ":desole:");
            dataInHtml.replace("<img src=\"resources/smileys/66.gif\" />", ":fete:");
            dataInHtml.replace("<img src=\"resources/smileys/67.gif\" />", ":sournois:");
            dataInHtml.replace("<img src=\"resources/smileys/68.gif\" />", ":hum:");
            dataInHtml.replace("<img src=\"resources/smileys/69.gif\" />", ":bravo:");
            dataInHtml.replace("<img src=\"resources/smileys/70.gif\" />", ":banzai:");
            dataInHtml.replace("<img src=\"resources/smileys/71.gif\" />", ":bave:");
            dataInHtml.replace("<img src=\"resources/smileys/nyu.gif\" />", ":cute:");
            //fin "à changer"
            parsingTool::replaceWithCapNumber(dataInHtml, expForSmileyToCode, 1, ":", ":");
            parsingTool::replaceWithCapNumber(dataInHtml, expForNoelshackToLink, 1);
            parsingTool::replaceWithCapNumber(dataInHtml, expForStickerToCode, 1, "[[sticker:p/", "]]");
            dataInHtml.replace(expForImageTag, "");
            lastClipboardDataChanged = dataInHtml;
            doc.setHtml(dataInHtml);
            newData->setHtml(dataInHtml);
            newData->setText(doc.toPlainText());
            clipboard->setMimeData(newData);
        }
    }
}

void respawnIrcClass::loadSettings()
{
    QList<QString> listOfTopicLink;
    QList<QString> listOfPseudoForTopic;

    beepWhenWarn = settingTool::getThisBoolOption("beepWhenWarn");
    beepForNewMP = settingTool::getThisBoolOption("beepForNewMP");
    warnUser = settingTool::getThisBoolOption("warnUser");

    for(int i = 0; i < 10; ++i)
    {
        vectorOfFavoriteLink.push_back(settingTool::getThisStringOption("favoriteLink" + QString::number(i)));
    }

    pseudoOfUser = settingTool::getThisStringOption("pseudo");
    listOfAccount = settingTool::getListOfAccount();

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
            settingTool::saveThisOption("pseudo", pseudoOfUser);
        }
    }

    listOfIgnoredPseudo = settingTool::getListOfIgnoredPseudo();
    listOfColorPseudo = settingTool::getListOfColorPseudo();
    listOfTopicLink = settingTool::getListOfTopicLink();
    listOfPseudoForTopic = settingTool::getListOfPseudoForTopic();

    for(int i = 0; i < listOfTopicLink.size(); ++i)
    {
        tabViewTopicInfos->addNewTabWithPseudo((i < listOfPseudoForTopic.size() ? listOfPseudoForTopic.at(i) : ""));
        tabViewTopicInfos->setBufferForTopicLinkFirstPage(listOfTopicLink.at(i), i);
    }

    if(tabViewTopicInfos->getTabListIsEmpty() == true)
    {
        tabViewTopicInfos->addNewTab();
    }

    tabViewTopicInfos->selectThisTab(0);

    messagesStatus->setText(tabViewTopicInfos->getMessageStatusOfCurrentTab());
    setNewNumberOfConnectedAndPseudoUsed();

    if(settingTool::getThisBoolOption("showTextDecorationButton") == false)
    {
        setShowTextDecorationButton(false);
    }

    sendMessages->setMultilineEdit(settingTool::getThisBoolOption("setMultilineEdit"));

    if(settingTool::getThisBoolOption("searchForUpdateAtLaunch") == true)
    {
        checkUpdate->startDownloadOfLatestUpdatePage();
    }
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
    buttonBold->setObjectName("boldButton");
    buttonItalic->setMaximumWidth(buttonItalic->fontMetrics().boundingRect(buttonItalic->text()).width() + 10);
    buttonItalic->setMaximumHeight(20);
    buttonItalic->setObjectName("italicButton");
    buttonUnderline->setMaximumWidth(buttonUnderline->fontMetrics().boundingRect(buttonUnderline->text()).width() + 10);
    buttonUnderline->setMaximumHeight(20);
    buttonUnderline->setObjectName("underlineButton");
    buttonStrike->setMaximumWidth(buttonStrike->fontMetrics().boundingRect(buttonStrike->text()).width() + 10);
    buttonStrike->setMaximumHeight(20);
    buttonStrike->setObjectName("strikeButton");
    buttonUList->setMaximumWidth(buttonUList->fontMetrics().boundingRect(buttonUList->text()).width() + 10);
    buttonUList->setMaximumHeight(20);
    buttonUList->setObjectName("ulistButton");
    buttonOList->setMaximumWidth(buttonOList->fontMetrics().boundingRect(buttonOList->text()).width() + 10);
    buttonOList->setMaximumHeight(20);
    buttonOList->setObjectName("olistButton");
    buttonQuote->setMaximumWidth(buttonQuote->fontMetrics().boundingRect(buttonQuote->text()).width() + 10);
    buttonQuote->setMaximumHeight(20);
    buttonQuote->setObjectName("quoteButton");
    buttonCode->setMaximumWidth(buttonCode->fontMetrics().boundingRect(buttonCode->text()).width() + 10);
    buttonCode->setMaximumHeight(20);
    buttonCode->setObjectName("codeButton");
    buttonSpoil->setMaximumWidth(buttonSpoil->fontMetrics().boundingRect(buttonSpoil->text()).width() + 10);
    buttonSpoil->setMaximumHeight(20);
    buttonSpoil->setObjectName("spoilButton");

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

    connect(buttonBold, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addBold);
    connect(buttonItalic, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addItalic);
    connect(buttonUnderline, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addUnderLine);
    connect(buttonStrike, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addStrike);
    connect(buttonUList, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addUList);
    connect(buttonOList, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addOListe);
    connect(buttonQuote, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addQuote);
    connect(buttonCode, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addCode);
    connect(buttonSpoil, &QPushButton::clicked, getMessageLine(), &multiTypeTextBoxClass::addSpoil);
}

void respawnIrcClass::setShowTextDecorationButton(bool newVal)
{
    for(int i = 0; i < buttonLayout->count(); ++i)
    {
        buttonLayout->itemAt(i)->widget()->setVisible(newVal);
    }
}

void respawnIrcClass::focusInEvent(QFocusEvent* event)
{
    (void)event;
    sendMessages->setFocus();
}

void respawnIrcClass::disconnectFromThisPseudo(QString thisPseudo)
{
    if(pseudoOfUser.toLower() == thisPseudo.toLower())
    {
        currentCookieList = QList<QNetworkCookie>();
        pseudoOfUser = "";
    }

    tabViewTopicInfos->setNewCookies(currentCookieList, pseudoOfUser, typeOfSaveForPseudo::DEFAULT, thisPseudo);
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

void respawnIrcClass::setTheseOptions(QMap<QString, bool> newBoolOptions, QMap<QString, int> newIntOptions)
{
    QMap<QString, bool>::iterator boolIte;
    QMap<QString, int>::iterator intIte;
    bool sendMessageAlreadyUpdated = false;
    bool reloadForAllTopicNeeded = false;

    for(boolIte = newBoolOptions.begin(); boolIte != newBoolOptions.end(); ++boolIte)
    {
        settingTool::saveThisOption(boolIte.key(), boolIte.value());
    }
    for(intIte = newIntOptions.begin(); intIte != newIntOptions.end(); ++intIte)
    {
        settingTool::saveThisOption(intIte.key(), intIte.value());
    }

    if((boolIte = newBoolOptions.find("showTextDecorationButton")) != newBoolOptions.end())
    {
        setShowTextDecorationButton(boolIte.value());
        newBoolOptions.erase(boolIte);
    }
    if((boolIte = newBoolOptions.find("setMultilineEdit")) != newBoolOptions.end())
    {
        sendMessages->setMultilineEdit(boolIte.value());
        newBoolOptions.erase(boolIte);
    }
    if((boolIte = newBoolOptions.find("beepWhenWarn")) != newBoolOptions.end())
    {
        beepWhenWarn = boolIte.value();
        newBoolOptions.erase(boolIte);
    }
    if((boolIte = newBoolOptions.find("beepForNewMP")) != newBoolOptions.end())
    {
        beepForNewMP = boolIte.value();
        newBoolOptions.erase(boolIte);
    }
    if((boolIte = newBoolOptions.find("warnUser")) != newBoolOptions.end())
    {
        warnUser = boolIte.value();
        newBoolOptions.erase(boolIte);
    }
    if((boolIte = newBoolOptions.find("useSpellChecker")) != newBoolOptions.end())
    {
        if(sendMessageAlreadyUpdated == false)
        {
            sendMessages->settingsChanged();
            sendMessageAlreadyUpdated = true;
        }
        newBoolOptions.erase(boolIte);
    }
    if((boolIte = newBoolOptions.find("changeColorOnEdit")) != newBoolOptions.end())
    {
        if(sendMessageAlreadyUpdated == false)
        {
            sendMessages->settingsChanged();
            sendMessageAlreadyUpdated = true;
        }
        newBoolOptions.erase(boolIte);
    }

    if((intIte = newIntOptions.find("textBoxSize")) != newIntOptions.end())
    {
        if(sendMessageAlreadyUpdated == false)
        {
            sendMessages->settingsChanged();
            sendMessageAlreadyUpdated = true;
        }
        newIntOptions.erase(intIte);
    }
    if((intIte = newIntOptions.find("typeOfImageRefresh")) != newIntOptions.end())
    {
        tabViewTopicInfos->updateSettings();
        newIntOptions.erase(intIte);
    }
    if((intIte = newIntOptions.find("typeOfEdit")) != newIntOptions.end())
    {
        reloadForAllTopicNeeded = true;
    }

    if(newBoolOptions.isEmpty() == false || newIntOptions.isEmpty() == false)
    {
        tabViewTopicInfos->updateSettingInfoForList();
        if(reloadForAllTopicNeeded == true)
        {
            tabViewTopicInfos->reloadAllTopic();
        }
    }
}

void respawnIrcClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo)
{
    if(newCookies.isEmpty() == false)
    {
        currentCookieList = newCookies;
        pseudoOfUser = newPseudoOfUser;
        tabViewTopicInfos->setNewCookies(currentCookieList, pseudoOfUser, (savePseudo == true ? typeOfSaveForPseudo::DEFAULT : typeOfSaveForPseudo::DONT_REMEMBER));
        setNewNumberOfConnectedAndPseudoUsed();

        if(saveAccountList == true)
        {
            accountListWindowClass::addAcountToThisList(currentCookieList, pseudoOfUser, &listOfAccount);
            settingTool::saveListOfAccount(listOfAccount);
        }

        if(savePseudo == true)
        {
            settingTool::saveThisOption("pseudo", pseudoOfUser);
        }
    }
}

void respawnIrcClass::setNewCookiesForCurrentTopic(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool savePseudo)
{
    tabViewTopicInfos->setNewCookiesForCurrentTab(newCookies, newPseudoOfUser, (savePseudo == true ? typeOfSaveForPseudo::REMEMBER : typeOfSaveForPseudo::DONT_REMEMBER));
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::setNewCookiesForPseudo(QString pseudo, const QList<QNetworkCookie>& cookiesForPseudo)
{
    for(accountStruct& thisAccout : listOfAccount)
    {
        if(thisAccout.pseudo.toLower() == pseudo.toLower())
        {
            thisAccout.listOfCookie = cookiesForPseudo;
            saveListOfAccount();
            break;
        }
    }
}

void respawnIrcClass::setNewMessageStatus()
{
    messagesStatus->setText(tabViewTopicInfos->getMessageStatusOfCurrentTab());
}

void respawnIrcClass::setNewNumberOfConnectedAndPseudoUsed()
{
    QString textToShow;

    if(tabViewTopicInfos->getTabListIsEmpty() == false)
    {
        textToShow += tabViewTopicInfos->getNumberOfConnectedAndMPOfCurrentTab();

        if(tabViewTopicInfos->getPseudoUsedOfCurrentTab().isEmpty() == false)
        {
            if(textToShow.isEmpty() == false)
            {
                textToShow += " - ";
            }
            textToShow += tabViewTopicInfos->getPseudoUsedOfCurrentTab();
        }
    }

    numberOfConnectedAndPseudoUsed->setText(textToShow);
}

void respawnIrcClass::saveListOfAccount()
{
    settingTool::saveListOfAccount(listOfAccount);
}

void respawnIrcClass::saveListOfIgnoredPseudo()
{
    settingTool::saveListOfIgnoredPseudo(listOfIgnoredPseudo);
}

void respawnIrcClass::saveListOfColorPseudo()
{
    settingTool::saveListOfColorPseudo(listOfColorPseudo);
}

void respawnIrcClass::warnUserForNewMessages()
{
    if(warnUser == true)
    {
        QApplication::alert(this);
    }

    if(QApplication::focusWidget() == nullptr && beepWhenWarn == true)
    {
        QSound::play(QCoreApplication::applicationDirPath() + "/resources/beep.wav");
    }
}

void respawnIrcClass::warnUserForNewMP(int newNumber, QString withThisPseudo)
{
    QMap<QString, int>::iterator iteForMPNumber = numberOfMPPerPseudos.find(withThisPseudo.toLower());
    int oldValueMPNumber = 0;

    if(iteForMPNumber != numberOfMPPerPseudos.end())
    {
        oldValueMPNumber = iteForMPNumber.value();
    }
    numberOfMPPerPseudos[withThisPseudo.toLower()] = newNumber;

    if(beepForNewMP == true && newNumber > oldValueMPNumber)
    {
        QSound::play(QCoreApplication::applicationDirPath() + "/resources/new_mp.wav");
    }
}

void respawnIrcClass::tabOfTabViewChanged()
{
    if(sendMessages->getIsSending() == false)
    {
        sendMessages->setEnableSendButton(true);

        if(sendMessages->getIsInEdit() == true)
        {
            sendMessages->clearMessageLine();
        }

        sendMessages->setIsInEdit(false);
    }

    setNewMessageStatus();
    setNewNumberOfConnectedAndPseudoUsed();
}

void respawnIrcClass::setEditMessage(long idOfMessageToEdit, bool useMessageEdit)
{
    if(sendMessages->getIsSending() == false)
    {
        if(sendMessages->getIsInEdit() == false)
        {
            sendMessages->setIsInEdit(true);
            sendMessages->setEnableSendButton(false);
            if(tabViewTopicInfos->getEditInfoForCurrentTab(idOfMessageToEdit, useMessageEdit) == false)
            {
                QMessageBox::warning(this, "Erreur", "Impossible d'éditer ce message.");
                sendMessages->setIsInEdit(false);
                sendMessages->setEnableSendButton(true);
            }
        }
        else
        {
            sendMessages->setIsInEdit(false);
            sendMessages->setEnableSendButton(true);
            sendMessages->clearMessageLine();
        }
    }
}
