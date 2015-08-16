#include "mainWindow.hpp"
#include "parsingTool.hpp"

mainWindowClass::mainWindowClass()
{
    QMenuBar* menuBar = new QMenuBar(this);

    QMenu* menuFile = menuBar->addMenu("Fichier");
    QAction* actionQuit = menuFile->addAction("Quitter");
    actionQuit->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F4));

    QMenu* menuAccount = menuBar->addMenu("Comptes");
    QAction* actionConnect = menuAccount->addAction("Se connecter");
    actionConnect->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));

    QMenu* menuTopic = menuBar->addMenu("Topic");
    QAction* actionSelectTopic = menuTopic->addAction("Choisir un topic");
    QAction* actionGoToTopic = menuTopic->addAction("Accéder au topic");
    QAction* actionGoToForum = menuTopic->addAction("Accéder au forum");
    actionSelectTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    actionGoToTopic->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    actionGoToForum->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_O));

    QMenu* menuTab = menuBar->addMenu("Onglet");
    QAction* actionTabAddTab = menuTab->addAction("Ajouter un onglet");
    actionTabAddTab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));

    QMenu* menuPseudo = menuBar->addMenu("Pseudo");
    QAction* actionShowListOfIgnoredPseudo = menuPseudo->addAction("Afficher la liste des ignorés");
    actionShowListOfIgnoredPseudo->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));

    QMenu* menuHelp = menuBar->addMenu("Aide");
    QAction* actionAboutQt = menuHelp->addAction("A propos de Qt");
    actionAboutQt->setShortcut(QKeySequence(Qt::ALT + Qt::Key_F10));

    setMenuBar(menuBar);
    setCentralWidget(&respawnIrc);
    setWindowTitle("RespawnIRC v1.6");
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    respawnIrc.setFocus();

    connect(actionConnect, SIGNAL(triggered()), &respawnIrc, SLOT(showConnect()));
    connect(actionSelectTopic, SIGNAL(triggered()), &respawnIrc, SLOT(showSelectTopic()));
    connect(actionGoToTopic, SIGNAL(triggered()), &respawnIrc, SLOT(goToCurrentTopic()));
    connect(actionGoToForum, SIGNAL(triggered()), &respawnIrc, SLOT(goToCurrentForum()));
    connect(actionTabAddTab, SIGNAL(triggered()), &respawnIrc, SLOT(addNewTab()));
    connect(actionShowListOfIgnoredPseudo, SIGNAL(triggered()), &respawnIrc, SLOT(showIgnoreListWindow()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), &respawnIrc, SLOT(clipboardChanged()));
}
