#include "mainWindow.hpp"
#include "parsingTool.hpp"

mainWindowClass::mainWindowClass()
{
    QMenuBar* menuBar = new QMenuBar(this);
    QMenu* menuFile = menuBar->addMenu("Fichier");
    QAction* actionQuit = menuFile->addAction("Quitter");
    QMenu* menuAccount = menuBar->addMenu("Comptes");
    QAction* actionConnect = menuAccount->addAction("Se connecter");
    QMenu* menuTopic = menuBar->addMenu("Topic");
    QAction* actionSelectTopic = menuTopic->addAction("Choisir un topic");
    QAction* actionGoToTopic = menuTopic->addAction("Accéder au topic");
    QMenu* menuTab = menuBar->addMenu("Onglet");
    QAction* actionTabAddTab = menuTab->addAction("Ajouter un onglet");
    QMenu* menuHelp = menuBar->addMenu("Aide");
    QAction* actionAboutQt = menuHelp->addAction("A propos de Qt");

    setMenuBar(menuBar);
    setCentralWidget(&respawnIrc);
    setWindowTitle("RespawnIRC v1.4.1");
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    respawnIrc.setFocus();

    connect(actionConnect, SIGNAL(triggered()), &respawnIrc, SLOT(showConnect()));
    connect(actionSelectTopic, SIGNAL(triggered()), &respawnIrc, SLOT(showSelectTopic()));
    connect(actionGoToTopic, SIGNAL(triggered()), &respawnIrc, SLOT(goToCurrentTopic()));
    connect(actionTabAddTab, SIGNAL(triggered()), &respawnIrc, SLOT(addNewTab()));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), &respawnIrc, SLOT(clipboardChanged()));
}
