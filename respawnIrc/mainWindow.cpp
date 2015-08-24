#include "mainWindow.hpp"
#include "settingTool.hpp"

mainWindowClass::mainWindowClass() : respawnIrc(this)
{
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* menuFile = menuBar->addMenu("&Fichier");
    QAction* actionConnect = menuFile->addAction("Se connecter");
    QAction* actionShowAccountList = menuFile->addAction("Afficher la liste des comptes");
    menuFile->addSeparator();
    QAction* actionTabAddTab = menuFile->addAction("Ajouter un onglet");
    menuFile->addSeparator();
    QAction* actionCheckUpdate = menuFile->addAction("Chercher une mise à jour");
    menuFile->addSeparator();
    QAction* actionQuit = menuFile->addAction("Quitter");
    actionConnect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    actionShowAccountList->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P));
    actionTabAddTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    actionCheckUpdate->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    actionQuit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));

    QMenu* menuDiscussion = menuBar->addMenu("&Discussion");
    QAction* actionSelectTopic = menuDiscussion->addAction("Choisir un topic");
    QAction* actionUpdateTopic = menuDiscussion->addAction("Forcer la récupération des messages");
    QAction* actionReloadTopic = menuDiscussion->addAction("Recharger le topic");
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
    menuDiscussion->addSeparator();
    QAction* actionGoToTopic = menuDiscussion->addAction("Accéder au topic");
    QAction* actionGoToForum = menuDiscussion->addAction("Accéder au forum");
    actionSelectTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    actionUpdateTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    actionReloadTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_E));
    actionGoToTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    actionGoToForum->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));

    QMenu* menuSetting = menuBar->addMenu("&Configuration");
    QAction* actionShowListOfIgnoredPseudo = menuSetting->addAction("Gérer les ignorés");
    QAction* actionShowListOfColorPseudo = menuSetting->addAction("Gérer la couleur des pseudos");
    menuSetting->addSeparator();
    QAction* actionShowUpdateTopicTime = menuSetting->addAction("Taux de rafraichissement des topics");
    QAction* actionShowNumberOfMessageShowedFirstTime = menuSetting->addAction("Nombre de message affiché au premier chargement");
    menuSetting->addSeparator();
    QAction* actionShowTextDecorationButtons = menuSetting->addAction("Afficher les boutons de décoration de texte");
    QAction* actionSetMultilineEdit = menuSetting->addAction("Saisi du message en mode multiligne");
    QAction* actionLoadTwoLastPage = menuSetting->addAction("Charger les deux dernières pages");
    QAction* actionSearchForUpdateAtLaunch = menuSetting->addAction("Chercher les mises à jour au lancement");
    actionShowListOfIgnoredPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    actionShowListOfColorPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
    actionShowTextDecorationButtons->setCheckable(true);
    actionShowTextDecorationButtons->setChecked(settingToolClass::getShowTextDecorationButton());
    actionSetMultilineEdit->setCheckable(true);
    actionSetMultilineEdit->setChecked(settingToolClass::getSetMultilineEdit());
    actionLoadTwoLastPage->setCheckable(true);
    actionLoadTwoLastPage->setChecked(settingToolClass::getLoadTwoLastPage());
    actionSearchForUpdateAtLaunch->setCheckable(true);
    actionSearchForUpdateAtLaunch->setChecked(settingToolClass::getSearchForUpdateAtLaunch());

    QMenu* menuHelp = menuBar->addMenu("&Aide");
    QAction* actionAboutQt = menuHelp->addAction("A propos de Qt");
    actionAboutQt->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F10));

    setMenuBar(menuBar);
    setCentralWidget(&respawnIrc);
    setWindowTitle("RespawnIRC " + respawnIrcClass::currentVersionName);
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    respawnIrc.setFocus();

    QObject::connect(actionConnect, &QAction::triggered, &respawnIrc, &respawnIrcClass::showConnect);
    QObject::connect(actionShowAccountList, &QAction::triggered, &respawnIrc, &respawnIrcClass::showAccountListWindow);
    QObject::connect(actionTabAddTab, &QAction::triggered, &respawnIrc, &respawnIrcClass::addNewTab);
    QObject::connect(actionCheckUpdate, &QAction::triggered, &respawnIrc, &respawnIrcClass::checkForUpdate);
    QObject::connect(actionSelectTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::showSelectTopic);
    QObject::connect(actionUpdateTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::updateTopic);
    QObject::connect(actionReloadTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::reloadTopic);
    QObject::connect(actionAddBold, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addBold);
    QObject::connect(actionAddItalic, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addItalic);
    QObject::connect(actionAddUnderline, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addUnderLine);
    QObject::connect(actionAddStrike, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addStrike);
    QObject::connect(actionAddUList, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addUList);
    QObject::connect(actionAddOList, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addOListe);
    QObject::connect(actionAddQuote, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addQuote);
    QObject::connect(actionAddCode, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addCode);
    QObject::connect(actionAddSpoil, &QAction::triggered, respawnIrc.getMessageLine(), &multiTypeTextBoxClass::addSpoil);
    QObject::connect(actionGoToTopic, &QAction::triggered, &respawnIrc, &respawnIrcClass::goToCurrentTopic);
    QObject::connect(actionGoToForum, &QAction::triggered, &respawnIrc, &respawnIrcClass::goToCurrentForum);
    QObject::connect(actionShowListOfIgnoredPseudo, &QAction::triggered, &respawnIrc, &respawnIrcClass::showIgnoreListWindow);
    QObject::connect(actionShowListOfColorPseudo, &QAction::triggered, &respawnIrc, &respawnIrcClass::showColorPseudoListWindow);
    QObject::connect(actionShowUpdateTopicTime, &QAction::triggered, &respawnIrc, &respawnIrcClass::showUpdateTopicTimeWindow);
    QObject::connect(actionShowNumberOfMessageShowedFirstTime, &QAction::triggered, &respawnIrc, &respawnIrcClass::showNumberOfMessageShowedFirstTimeWindow);
    QObject::connect(actionShowTextDecorationButtons, &QAction::toggled, &respawnIrc, &respawnIrcClass::setShowTextDecorationButton);
    QObject::connect(actionSetMultilineEdit, &QAction::toggled, &respawnIrc, &respawnIrcClass::setMultilineEdit);
    QObject::connect(actionLoadTwoLastPage, &QAction::toggled, &respawnIrc, &respawnIrcClass::setLoadTwoLastPage);
    QObject::connect(actionSearchForUpdateAtLaunch, &QAction::toggled, &respawnIrc, &respawnIrcClass::setSearchForUpdateAtLaunch);
    QObject::connect(actionQuit, &QAction::triggered, this, &QMainWindow::close);
    QObject::connect(actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    QObject::connect(QApplication::clipboard(), &QClipboard::changed, &respawnIrc, &respawnIrcClass::clipboardChanged);
}
