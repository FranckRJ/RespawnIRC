#include "mainWindow.hpp"
#include "settingTool.hpp"
#include "styleTool.hpp"

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
    QAction* actionGoToMp = menuFile->addAction("Accéder à la boîte de réception");
    QAction* actionCheckUpdate = menuFile->addAction("Chercher une mise à jour");
    menuFile->addSeparator();
    QAction* actionQuit = menuFile->addAction("Quitter");
    actionConnect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    actionShowAccountList->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P));
    actionTabAddTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    actionGoToMp->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
    actionCheckUpdate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    actionQuit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));

    QMenu* menuDiscussion = menuBar->addMenu("&Discussion");
    QAction* actionSelectTopic = menuDiscussion->addAction("Choisir un topic");
    QAction* actionUpdateTopic = menuDiscussion->addAction("Forcer la récupération des messages");
    QAction* actionReloadTopic = menuDiscussion->addAction("Recharger le topic");
    QAction* actionReloadAllTopic = menuDiscussion->addAction("Recharger tous les topics");
    menuDiscussion->addSeparator();
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
    actionEditLastMessage->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    actionGoToTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    actionGoToForum->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));

    QMenu* menuSetting = menuBar->addMenu("&Configuration");
    QAction* actionShowListOfIgnoredPseudo = menuSetting->addAction("Gérer les ignorés");
    QAction* actionShowListOfColorPseudo = menuSetting->addAction("Gérer la couleur des pseudos");
    menuSetting->addSeparator();
    QAction* actionShowUpdateTopicTime = menuSetting->addAction("Taux de rafraichissement des topics");
    QAction* actionShowNumberOfMessageShowedFirstTime = menuSetting->addAction("Nombre de message affiché au premier chargement");
    QAction* actionShowStickersSize = menuSetting->addAction("Taille des stickers");
    menuSetting->addSeparator();
    settingToolClass::createActionForBoolOption("Ajouter un bouton pour citer un message", "showQuoteButton", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Ajouter un bouton pour ignorer un pseudo", "showBlacklistButton", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Ajouter un bouton pour éditer un message", "showEditButton", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Afficher les stickers", "showStickers", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Colorer le pseudo des modo/admin", "colorModoAndAdminPseudo", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Colorer les PEMT", "colorPEMT", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    menuSetting->addSeparator();
    settingToolClass::createActionForBoolOption("Afficher les boutons de décoration de texte", "showTextDecorationButton", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Afficher la liste des topics", "showListOfTopic", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Saisie du message en mode multiligne", "setMultilineEdit", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Vérifier l'orthographe", "useSpellChecker", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Charger les deux dernières pages", "loadTwoLastPage", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Récupérer le premier message du topic", "getFirstMessageOfTopic", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Ignorer les erreurs réseau", "ignoreNetworkError", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Chercher les mises à jour au lancement", "searchForUpdateAtLaunch", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Beeper lors de la réception d'un message", "beepWhenWarn", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Avertir visuellement lors de la réception d'un message", "warnUser", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    settingToolClass::createActionForBoolOption("Avertir lors de l'édition d'un message", "warnWhenEdit", menuSetting, &respawnIrc, SLOT(setThisBoolOption(bool)));
    menuSetting->addSeparator();
    settingToolClass::createActionForBoolOption("Sauvegarder la taille de la fenêtre", "saveWindowGeometry", menuSetting, this, SLOT(saveWindowGeometry(bool)));
    actionShowListOfIgnoredPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    actionShowListOfColorPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));

    QMenu* menuFavorites = menuBar->addMenu("&Favoris");
    QMenu* menuUseFavorite = menuFavorites->addMenu("Accéder aux favoris");
    QMenu* menuAddFavorite = menuFavorites->addMenu("Ajouter aux favoris");
    QMenu* menuDelFavorite = menuFavorites->addMenu("Supprimer un favori");

    for(int i = 0; i < 10; ++i)
    {
        QFont thisFont;
        QString nameForUseAndDel;
        QString nameForAdd;
        QString nameOfTopic = settingToolClass::getThisStringOption("favoriteName" + QString::number(i));

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

        QObject::connect(vectorOfUseFavorite.back(), &QAction::triggered, this, &mainWindowClass::useFavoriteClicked);
        QObject::connect(vectorOfAddFavorite.back(), &QAction::triggered, this, &mainWindowClass::addFavoriteClicked);
        QObject::connect(vectorOfDelFavorite.back(), &QAction::triggered, this, &mainWindowClass::delFavoriteClicked);
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

    if(settingToolClass::getThisBoolOption("saveWindowGeometry") == false ||
            settingToolClass::getThisByteOption("windowGeometry").isEmpty() == true)
    {
        resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    }
    else
    {
        restoreGeometry(settingToolClass::getThisByteOption("windowGeometry"));
    }
    respawnIrc.setFocus();

    QObject::connect(actionConnect, &QAction::triggered, &respawnIrc, &respawnIrcClass::showConnect);
    QObject::connect(actionShowAccountList, &QAction::triggered, &respawnIrc, &respawnIrcClass::showAccountListWindow);
    QObject::connect(actionDisconnectFromCurrentTab, &QAction::triggered, &respawnIrc, &respawnIrcClass::disconnectFromCurrentTab);
    QObject::connect(actionDisconnectFromAllTabs, &QAction::triggered, &respawnIrc, &respawnIrcClass::disconnectFromAllTabs);
    QObject::connect(actionTabAddTab, &QAction::triggered, &respawnIrc, &respawnIrcClass::addNewTab);
    QObject::connect(actionGoToMp, &QAction::triggered, this, &mainWindowClass::goToMp);
    QObject::connect(actionCheckUpdate, &QAction::triggered, &respawnIrc, &respawnIrcClass::checkForUpdate);
    QObject::connect(actionSelectTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::showSelectTopic);
    QObject::connect(actionUpdateTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::updateTopic);
    QObject::connect(actionReloadTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::reloadTopic);
    QObject::connect(actionReloadAllTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::reloadAllTopic);
    QObject::connect(actionAddBold, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addBold);
    QObject::connect(actionAddItalic, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addItalic);
    QObject::connect(actionAddUnderline, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addUnderLine);
    QObject::connect(actionAddStrike, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addStrike);
    QObject::connect(actionAddUList, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addUList);
    QObject::connect(actionAddOList, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addOListe);
    QObject::connect(actionAddQuote, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addQuote);
    QObject::connect(actionAddCode, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addCode);
    QObject::connect(actionAddSpoil, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addSpoil);
    QObject::connect(actionEditLastMessage, &QAction::triggered, &respawnIrc, &respawnIrcClass::editLastMessage);
    QObject::connect(actionGoToTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::goToCurrentTopic);
    QObject::connect(actionGoToForum, &QAction::triggered, &respawnIrc, &respawnIrcClass::goToCurrentForum);
    QObject::connect(actionShowListOfIgnoredPseudo, &QAction::triggered, &respawnIrc, &respawnIrcClass::showIgnoreListWindow);
    QObject::connect(actionShowListOfColorPseudo, &QAction::triggered, &respawnIrc, &respawnIrcClass::showColorPseudoListWindow);
    QObject::connect(actionShowUpdateTopicTime, &QAction::triggered, &respawnIrc, &respawnIrcClass::showUpdateTopicTimeWindow);
    QObject::connect(actionShowNumberOfMessageShowedFirstTime, &QAction::triggered, &respawnIrc, &respawnIrcClass::showNumberOfMessageShowedFirstTimeWindow);
    QObject::connect(actionShowStickersSize, &QAction::triggered, &respawnIrc, &respawnIrcClass::showStickersSizeWindow);
    QObject::connect(actionQuit, &QAction::triggered, this, &QMainWindow::close);
    QObject::connect(actionSelectTheme, &QAction::triggered, &respawnIrc, &respawnIrcClass::showSelectTheme);
    QObject::connect(actionReloadTheme, &QAction::triggered, &respawnIrc, &respawnIrcClass::reloadTheme);
    QObject::connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    QObject::connect(actionAbout, &QAction::triggered, &respawnIrc, &respawnIrcClass::showAbout);
    QObject::connect(QApplication::clipboard(), &QClipboard::changed, &respawnIrc, &respawnIrcClass::clipboardChanged);
    QObject::connect(&respawnIrc, &respawnIrcClass::themeChanged, this, &mainWindowClass::setNewTheme);

    respawnIrc.setNewTheme(settingToolClass::getThisStringOption("themeUsed"));
}

void mainWindowClass::goToMp()
{
    QDesktopServices::openUrl(QUrl("http://www.jeuxvideo.com/messages-prives/boite-reception.php"));
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
    setStyleSheet(styleToolClass::getStyle(newThemeName));
    styleToolClass::getModelInfo(newThemeName);
}

void mainWindowClass::saveWindowGeometry(bool newVal)
{
    settingToolClass::saveThisOption("saveWindowGeometry", newVal);
}

void mainWindowClass::keyPressEvent(QKeyEvent* thisKey)
{
    QString keyPressed = QKeySequence(thisKey->key()).toString();
    if(thisKey->modifiers().testFlag(Qt::ControlModifier) == true)
    {
        if(keyPressed == "&")
        {
            respawnIrc.selectThisTab(0);
        }
        else if(keyPressed == "\u00C9")
        {
            respawnIrc.selectThisTab(1);
        }
        else if(keyPressed == "\"")
        {
            respawnIrc.selectThisTab(2);
        }
        else if(keyPressed == "'")
        {
            respawnIrc.selectThisTab(3);
        }
        else if(keyPressed == "(")
        {
            respawnIrc.selectThisTab(4);
        }
        else if(keyPressed == "-")
        {
            respawnIrc.selectThisTab(5);
        }
        else if(keyPressed == "\u00C8")
        {
            respawnIrc.selectThisTab(6);
        }
        else if(keyPressed == "_")
        {
            respawnIrc.selectThisTab(7);
        }
        else if(keyPressed == "\u00C7")
        {
            respawnIrc.selectThisTab(8);
        }
        else if(keyPressed == "@")
        {
            respawnIrc.selectThisTab(9);
        }
    }
}

void mainWindowClass::closeEvent(QCloseEvent* event)
{
    settingToolClass::saveThisOption("windowGeometry", saveGeometry());
    QMainWindow::closeEvent(event);
}
