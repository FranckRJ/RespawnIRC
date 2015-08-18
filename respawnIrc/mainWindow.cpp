#include "mainWindow.hpp"
#include "settingTool.hpp"

mainWindowClass::mainWindowClass()
{
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* menuFile = menuBar->addMenu("&Fichier");
    QAction* actionConnect = menuFile->addAction("Se connecter");
    menuFile->addSeparator();
    QAction* actionQuit = menuFile->addAction("Quitter");
    actionQuit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));
    actionConnect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));

    QMenu* menuDiscussion = menuBar->addMenu("&Discussion");
    QAction* actionTabAddTab = menuDiscussion->addAction("Ajouter un onglet");
    menuDiscussion->addSeparator();
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
    actionTabAddTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    actionSelectTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    actionUpdateTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    actionReloadTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_E));
    actionGoToTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    actionGoToForum->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));

    QMenu* menuSetting = menuBar->addMenu("&Configuration");
    QAction* actionShowListOfIgnoredPseudo = menuSetting->addAction("Afficher la liste des ignorés");
    menuSetting->addSeparator();
    QAction* actionShowUpdateTopicTime = menuSetting->addAction("Taux de rafraichissement des topics");
    QAction* actionShowNumberOfMessageShowedFirstTime = menuSetting->addAction("Nombre de message affiché au premier chargement");
    menuSetting->addSeparator();
    QAction* actionShowTextDecorationButtons = menuSetting->addAction("Afficher les boutons de décoration de texte");
    QAction* actionSetMultilineEdit = menuSetting->addAction("Saisi du message en mode multiligne");
    QAction* actionLoadTwoLastPage = menuSetting->addAction("Charger les deux dernières pages");
    actionShowListOfIgnoredPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    actionShowTextDecorationButtons->setCheckable(true);
    actionShowTextDecorationButtons->setChecked(settingToolClass::getShowTextDecorationButton());
    actionSetMultilineEdit->setCheckable(true);
    actionSetMultilineEdit->setChecked(settingToolClass::getSetMultilineEdit());
    actionLoadTwoLastPage->setCheckable(true);
    actionLoadTwoLastPage->setChecked(settingToolClass::getLoadTwoLastPage());

    QMenu* menuHelp = menuBar->addMenu("&Aide");
    QAction* actionAboutQt = menuHelp->addAction("A propos de Qt");
    actionAboutQt->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F10));

    setMenuBar(menuBar);
    setCentralWidget(&respawnIrc);
    setWindowTitle("RespawnIRC v1.8");
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    respawnIrc.setFocus();

    connect(actionConnect, SIGNAL(triggered()), &respawnIrc, SLOT(showConnect()));
    connect(actionTabAddTab, SIGNAL(triggered()), &respawnIrc, SLOT(addNewTab()));
    connect(actionSelectTopic, SIGNAL(triggered()), &respawnIrc, SLOT(showSelectTopic()));
    connect(actionUpdateTopic, SIGNAL(triggered()), &respawnIrc, SLOT(updateTopic()));
    connect(actionReloadTopic, SIGNAL(triggered()), &respawnIrc, SLOT(reloadTopic()));
    connect(actionAddBold, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addBold()));
    connect(actionAddItalic, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addItalic()));
    connect(actionAddUnderline, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addUnderLine()));
    connect(actionAddStrike, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addStrike()));
    connect(actionAddUList, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addUList()));
    connect(actionAddOList, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addOListe()));
    connect(actionAddQuote, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addQuote()));
    connect(actionAddCode, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addCode()));
    connect(actionAddSpoil, SIGNAL(triggered()), respawnIrc.getMessageLine(), SLOT(addSpoil()));
    connect(actionGoToTopic, SIGNAL(triggered()), &respawnIrc, SLOT(goToCurrentTopic()));
    connect(actionGoToForum, SIGNAL(triggered()), &respawnIrc, SLOT(goToCurrentForum()));
    connect(actionShowListOfIgnoredPseudo, SIGNAL(triggered()), &respawnIrc, SLOT(showIgnoreListWindow()));
    connect(actionShowUpdateTopicTime, SIGNAL(triggered()), &respawnIrc, SLOT(showUpdateTopicTimeWindow()));
    connect(actionShowNumberOfMessageShowedFirstTime, SIGNAL(triggered()), &respawnIrc, SLOT(showNumberOfMessageShowedFirstTimeWindow()));
    connect(actionShowTextDecorationButtons, SIGNAL(toggled(bool)), &respawnIrc, SLOT(setShowTextDecorationButton(bool)));
    connect(actionSetMultilineEdit, SIGNAL(toggled(bool)), &respawnIrc, SLOT(setMultilineEdit(bool)));
    connect(actionLoadTwoLastPage, SIGNAL(toggled(bool)), &respawnIrc, SLOT(setLoadTwoLastPage(bool)));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), &respawnIrc, SLOT(clipboardChanged()));
}
