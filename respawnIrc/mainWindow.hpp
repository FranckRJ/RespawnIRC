#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets>
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
