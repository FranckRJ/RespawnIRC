#include "mainWindow.hpp"
#include "parsingTool.hpp"

mainWindowClass::mainWindowClass()
{
    QMenuBar* menuBar = new QMenuBar(this);
    QAction* actionConnect = menuBar->addAction("Se connecter");
    QAction* actionSelectTopic = menuBar->addAction("Choisir un topic");

    setMenuBar(menuBar);
    setCentralWidget(&respawnIrc);
    setWindowTitle("RespawnIRC v1.3");
    resize(QDesktopWidget().availableGeometry(this).size() * 0.7);
    respawnIrc.setFocus();

    connect(actionConnect, SIGNAL(triggered()), &respawnIrc, SLOT(showConnect()));
    connect(actionSelectTopic, SIGNAL(triggered()), &respawnIrc, SLOT(showSelectTopic()));
    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)), &respawnIrc, SLOT(clipboardChanged()));
}
