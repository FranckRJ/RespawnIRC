#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDesktopWidget>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QKeySequence>
#include <QFont>

#include "mainWindow.hpp"
#include "settingTool.hpp"
#include "styleTool.hpp"
#include "shortcutTool.hpp"

mainWindowClass::mainWindowClass() : respawnIrc(this)
{
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* menuFile = menuBar->addMenu("&Fichier");
    QAction* actionConnect = menuFile->addAction("Se connecter");
    QAction* actionShowAccountList = menuFile->addAction("Afficher la liste des comptes");
    QAction* actionDisconnectFromCurrentTab = menuFile->addAction("Se déconnecter sur l'onglet actuel");
    QAction* actionDisconnectFromAllTabs = menuFile->addAction("Se déconnecter sur tous les onglets");
    menuFile->addSeparator();
    QAction* actionTabAddTab = menuFile->addAction("Ajouter un onglet");
    menuFile->addSeparator();
    QAction* actionOpenWebNavigator = menuFile->addAction("Ouvrir RespawnIRC Navigator");
    QAction* actionOpenWebNavigatorAtMP = menuFile->addAction("Accéder aux MP via RespawnIRC Navigator");
    menuFile->addSeparator();
    QAction* actionGoToMp = menuFile->addAction("Accéder à la boîte de réception");
    QAction* actionCheckUpdate = menuFile->addAction("Chercher une mise à jour");
    menuFile->addSeparator();
    QAction* actionQuit = menuFile->addAction("Quitter");
    actionConnect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    actionShowAccountList->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P));
    actionTabAddTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    actionOpenWebNavigator->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    actionOpenWebNavigatorAtMP->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_M));
    actionGoToMp->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    actionCheckUpdate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    actionQuit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));

    QMenu* menuDiscussion = menuBar->addMenu("&Discussion");
    QAction* actionSelectTopic = menuDiscussion->addAction("Choisir un topic");
    QAction* actionUpdateTopic = menuDiscussion->addAction("Forcer la récupération des messages");
    QAction* actionReloadTopic = menuDiscussion->addAction("Recharger le topic");
    QAction* actionReloadAllTopic = menuDiscussion->addAction("Recharger tous les topics");
    menuDiscussion->addSeparator();
    QAction* actionSelectSticker = menuDiscussion->addAction("Sélectionner un sticker");
    QMenu* menuTextDecoration = menuDiscussion->addMenu("Ajouter des décorations de texte");
    QAction* actionAddBold = menuTextDecoration->addAction("Gras");
    QAction* actionAddItalic = menuTextDecoration->addAction("Italique");
    QAction* actionAddUnderline = menuTextDecoration->addAction("Souligné");
    QAction* actionAddStrike = menuTextDecoration->addAction("Barré");
    QAction* actionAddUList = menuTextDecoration->addAction("Liste non-ordonnée");
    QAction* actionAddOList = menuTextDecoration->addAction("Liste ordonnée");
    QAction* actionAddQuote = menuTextDecoration->addAction("Citation");
    QAction* actionAddCode = menuTextDecoration->addAction("Code");
    QAction* actionAddSpoil = menuTextDecoration->addAction("Spoil");
    QAction* actionEditLastMessage = menuDiscussion->addAction("Editer le dernier message");
    menuDiscussion->addSeparator();
    QAction* actionGoToTopic = menuDiscussion->addAction("Accéder au topic");
    QAction* actionGoToForum = menuDiscussion->addAction("Accéder au forum");
    actionSelectTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    actionUpdateTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    actionReloadTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    actionSelectSticker->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    actionEditLastMessage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    actionGoToTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    actionGoToForum->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));

    QMenu* menuSetting = menuBar->addMenu("&Configuration");
    QAction* actionShowListOfIgnoredPseudo = menuSetting->addAction("Gérer les ignorés");
    QAction* actionShowListOfColorPseudo = menuSetting->addAction("Gérer la couleur des pseudos");
    menuSetting->addSeparator();
    QAction* actionManageShortcuts = menuSetting->addAction("Gérer les raccourcis");
    QAction* actionReloadShortcuts = menuSetting->addAction("Recharger les raccourcis");
    menuSetting->addSeparator();
    QAction* actionShowPreferences = menuSetting->addAction("Préférences...");
    actionShowListOfIgnoredPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    actionShowListOfColorPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
    actionShowPreferences->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));

    QMenu* menuFavorites = menuBar->addMenu("&Favoris");
    QMenu* menuUseFavorite = menuFavorites->addMenu("Accéder aux favoris");
    QMenu* menuAddFavorite = menuFavorites->addMenu("Ajouter aux favoris");
    QMenu* menuDelFavorite = menuFavorites->addMenu("Supprimer un favori");

    for(int i = 0; i < 10; ++i)
    {
        QFont thisFont;
        QString nameForUseAndDel;
        QString nameForAdd;
        QString nameOfTopic = settingTool::getThisStringOption("favoriteName" + QString::number(i));

        if(nameOfTopic.isEmpty() == false)
        {
            nameForUseAndDel = nameOfTopic;
            nameForAdd = nameOfTopic;
        }
        else
        {
            nameForUseAndDel = "Vide";
            nameForAdd = "Emplacement " + QString::number(i + 1);
        }

        vectorOfUseFavorite.push_back(menuUseFavorite->addAction(nameForUseAndDel));
        vectorOfUseFavorite.back()->setShortcut(QKeySequence(Qt::CTRL + (Qt::Key_F1 + i)));
        if(nameOfTopic.isEmpty() == true)
        {
            thisFont = vectorOfUseFavorite.back()->font();
            thisFont.setItalic(true);
            vectorOfUseFavorite.back()->setFont(thisFont);
        }

        vectorOfAddFavorite.push_back(menuAddFavorite->addAction(nameForAdd));
        vectorOfAddFavorite.back()->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + (Qt::Key_F1 + i)));

        vectorOfDelFavorite.push_back(menuDelFavorite->addAction(nameForUseAndDel));
        if(nameOfTopic.isEmpty() == true)
        {
            thisFont = vectorOfDelFavorite.back()->font();
            thisFont.setItalic(true);
            vectorOfDelFavorite.back()->setFont(thisFont);
        }

        connect(vectorOfUseFavorite.back(), &QAction::triggered, this, &mainWindowClass::useFavoriteClicked);
        connect(vectorOfAddFavorite.back(), &QAction::triggered, this, &mainWindowClass::addFavoriteClicked);
        connect(vectorOfDelFavorite.back(), &QAction::triggered, this, &mainWindowClass::delFavoriteClicked);
    }

    QMenu* menuTheme = menuBar->addMenu("&Thèmes");
    QAction* actionSelectTheme = menuTheme->addAction("Sélectionner un thème");
    QAction* actionReloadTheme = menuTheme->addAction("Recharger le thème actuel");

    QMenu* menuHelp = menuBar->addMenu("&Aide");
    QAction* actionAbout = menuHelp->addAction("A propos de RespawnIRC");
    QAction* actionAboutQt = menuHelp->addAction("A propos de Qt");
    actionAbout->setShortcut(QKeySequence(Qt::Key_F10));
    actionAboutQt->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F10));

    setMenuBar(menuBar);
    setCentralWidget(&respawnIrc);
    setWindowTitle("RespawnIRC " + respawnIrcClass::currentVersionName);

    if(settingTool::getThisBoolOption("saveWindowGeometry") == false ||
            settingTool::getThisByteOption("windowGeometry").isEmpty() == true)
    {
        resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    }
    else
    {
        restoreGeometry(settingTool::getThisByteOption("windowGeometry"));
    }
    respawnIrc.setFocus();

    connect(actionConnect, &QAction::triggered, &respawnIrc, &respawnIrcClass::showConnect);
    connect(actionShowAccountList, &QAction::triggered, &respawnIrc, &respawnIrcClass::showAccountListWindow);
    connect(actionDisconnectFromCurrentTab, &QAction::triggered, &respawnIrc, &respawnIrcClass::disconnectFromCurrentTab);
    connect(actionDisconnectFromAllTabs, &QAction::triggered, &respawnIrc, &respawnIrcClass::disconnectFromAllTabs);
    connect(actionTabAddTab, &QAction::triggered, respawnIrc.getTabView(), &tabViewTopicInfosClass::addNewTab);
    connect(actionOpenWebNavigator, &QAction::triggered, &respawnIrc, &respawnIrcClass::showWebNavigator);
    connect(actionOpenWebNavigatorAtMP, &QAction::triggered, &respawnIrc, &respawnIrcClass::showWebNavigatorAtMP);
    connect(actionGoToMp, &QAction::triggered, this, &mainWindowClass::goToMp);
    connect(actionCheckUpdate, &QAction::triggered, &respawnIrc, &respawnIrcClass::checkForUpdate);
    connect(actionSelectTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::showSelectTopic);
    connect(actionUpdateTopic, &QAction::triggered, respawnIrc.getTabView(), &tabViewTopicInfosClass::updateCurrentTopic);
    connect(actionReloadTopic, &QAction::triggered, respawnIrc.getTabView(), &tabViewTopicInfosClass::reloadCurrentTopic);
    connect(actionReloadAllTopic, &QAction::triggered, respawnIrc.getTabView(), &tabViewTopicInfosClass::reloadAllTopic);
    connect(actionSelectSticker, &QAction::triggered, &respawnIrc, &respawnIrcClass::showSelectSticker);
    connect(actionAddBold, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addBold);
    connect(actionAddItalic, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addItalic);
    connect(actionAddUnderline, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addUnderLine);
    connect(actionAddStrike, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addStrike);
    connect(actionAddUList, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addUList);
    connect(actionAddOList, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addOListe);
    connect(actionAddQuote, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addQuote);
    connect(actionAddCode, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addCode);
    connect(actionAddSpoil, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addSpoil);
    connect(actionEditLastMessage, &QAction::triggered, &respawnIrc, &respawnIrcClass::editLastMessage);
    connect(actionGoToTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::goToCurrentTopic);
    connect(actionGoToForum, &QAction::triggered, &respawnIrc, &respawnIrcClass::goToCurrentForum);
    connect(actionShowListOfIgnoredPseudo, &QAction::triggered, &respawnIrc, &respawnIrcClass::showIgnoreListWindow);
    connect(actionShowListOfColorPseudo, &QAction::triggered, &respawnIrc, &respawnIrcClass::showColorPseudoListWindow);
    connect(actionManageShortcuts, &QAction::triggered, &respawnIrc, &respawnIrcClass::showManageShortcutWindow);
    connect(actionReloadShortcuts, &QAction::triggered, this, &mainWindowClass::reloadShortcuts);
    connect(actionShowPreferences, &QAction::triggered, &respawnIrc, &respawnIrcClass::showPreferences);
    connect(actionQuit, &QAction::triggered, this, &QMainWindow::close);
    connect(actionSelectTheme, &QAction::triggered, &respawnIrc, &respawnIrcClass::showSelectTheme);
    connect(actionReloadTheme, &QAction::triggered, &respawnIrc, &respawnIrcClass::reloadTheme);
    connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(actionAbout, &QAction::triggered, &respawnIrc, &respawnIrcClass::showAbout);
    connect(QApplication::clipboard(), &QClipboard::changed, &respawnIrc, &respawnIrcClass::clipboardChanged);
    connect(&respawnIrc, &respawnIrcClass::themeChanged, this, &mainWindowClass::setNewTheme);
    connect(QApplication::instance(), &QApplication::aboutToQuit, this, &mainWindowClass::doStuffBeforeQuit);

    respawnIrc.setNewTheme(settingTool::getThisStringOption("themeUsed"));
}

void mainWindowClass::keyPressEvent(QKeyEvent* thisKey)
{
    QString keyPressed = QKeySequence(thisKey->key()).toString();
    if(thisKey->modifiers().testFlag(Qt::ControlModifier) == true)
    {
        if(keyPressed == "&")
        {
            respawnIrc.getTabView()->selectThisTab(0);
        }
        else if(keyPressed == "\u00C9" || keyPressed == "É")
        {
            respawnIrc.getTabView()->selectThisTab(1);
        }
        else if(keyPressed == "\"")
        {
            respawnIrc.getTabView()->selectThisTab(2);
        }
        else if(keyPressed == "'")
        {
            respawnIrc.getTabView()->selectThisTab(3);
        }
        else if(keyPressed == "(")
        {
            respawnIrc.getTabView()->selectThisTab(4);
        }
        else if(keyPressed == "-")
        {
            respawnIrc.getTabView()->selectThisTab(5);
        }
        else if(keyPressed == "\u00C8" || keyPressed == "È")
        {
            respawnIrc.getTabView()->selectThisTab(6);
        }
        else if(keyPressed == "_")
        {
            respawnIrc.getTabView()->selectThisTab(7);
        }
        else if(keyPressed == "\u00C7" || keyPressed == "Ç")
        {
            respawnIrc.getTabView()->selectThisTab(8);
        }
        else if(keyPressed == "@")
        {
            respawnIrc.getTabView()->selectThisTab(9);
        }
        else if(keyPressed == "Enter" || keyPressed == "Return")
        {
            respawnIrc.messageHaveToBePosted();
        }
    }
}

void mainWindowClass::closeEvent(QCloseEvent* event)
{
    settingTool::saveThisOption("windowGeometry", saveGeometry());
    QMainWindow::closeEvent(event);
}

void mainWindowClass::doStuffBeforeQuit()
{
    respawnIrc.doStuffBeforeQuit();
    settingTool::forceSync();
}

void mainWindowClass::goToMp()
{
    QDesktopServices::openUrl(QUrl("http://www.jeuxvideo.com/messages-prives/boite-reception.php"));
}

void mainWindowClass::reloadShortcuts()
{
    shortcutTool::initializeAllShortcutsRules();
}

void mainWindowClass::useFavoriteClicked()
{
    QAction* thisAction = dynamic_cast<QAction*>(sender());

    respawnIrc.useThisFavorite(vectorOfUseFavorite.indexOf(thisAction));
}

void mainWindowClass::addFavoriteClicked()
{
    QAction* thisAction = dynamic_cast<QAction*>(sender());
    QString newTopicName = respawnIrc.addThisFavorite(vectorOfAddFavorite.indexOf(thisAction));

    if(newTopicName.isEmpty() == false)
    {
        QFont thisFont;
        int index = vectorOfAddFavorite.indexOf(thisAction);

        vectorOfAddFavorite[index]->setText(newTopicName);

        thisFont = vectorOfUseFavorite[index]->font();
        thisFont.setItalic(false);
        vectorOfUseFavorite[index]->setFont(thisFont);
        vectorOfUseFavorite[index]->setText(newTopicName);

        thisFont = vectorOfDelFavorite[index]->font();
        thisFont.setItalic(false);
        vectorOfDelFavorite[index]->setFont(thisFont);
        vectorOfDelFavorite[index]->setText(newTopicName);
    }
}

void mainWindowClass::delFavoriteClicked()
{
    QFont thisFont;
    QAction* thisAction = dynamic_cast<QAction*>(sender());
    int index = vectorOfDelFavorite.indexOf(thisAction);

    respawnIrc.delThisFavorite(index);

    thisFont = vectorOfDelFavorite[index]->font();
    thisFont.setItalic(true);
    vectorOfDelFavorite[index]->setFont(thisFont);
    vectorOfDelFavorite[index]->setText("Vide");

    thisFont = vectorOfUseFavorite[index]->font();
    thisFont.setItalic(true);
    vectorOfUseFavorite[index]->setFont(thisFont);
    vectorOfUseFavorite[index]->setText("Vide");

    vectorOfAddFavorite[index]->setText("Emplacement " + QString::number(index));
}

void mainWindowClass::setNewTheme(QString newThemeName)
{
    setStyleSheet(styleTool::getStyle(newThemeName));
    styleTool::getModelInfo(newThemeName);
}
