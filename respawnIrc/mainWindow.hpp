#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGui>
#include <QtCore>

#include "respawnIrc.hpp"

class mainWindowClass : public QMainWindow
{
public:
    mainWindowClass();
private:
    respawnIrcClass respawnIrc;
};
#endif
