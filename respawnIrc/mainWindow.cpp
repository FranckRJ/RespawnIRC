#include "mainWindow.hpp"

mainWindowClass::mainWindowClass()
{
    QMenuBar* menuBar = new QMenuBar(this);
    QAction* actionConnect = menuBar->addAction("Se connecter");
    QAction* actionSelectTopic = menuBar->addAction("Choisir un topic");

    setMenuBar(menuBar);
    setCentralWidget(&respawnIrc);
    setWindowTitle("RespawnIRC v1.0");
    resize(QDesktopWidget().availableGeometry(this).size() * 0.5);
    respawnIrc.setFocus();

    connect(actionConnect, SIGNAL(triggered()), &respawnIrc, SLOT(showConnect()));
    connect(actionSelectTopic, SIGNAL(triggered()), &respawnIrc, SLOT(showSelectTopic()));
}

