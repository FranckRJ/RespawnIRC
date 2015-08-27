#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtWidgets>
#include <QtCore>

#include "respawnIrc.hpp"

class mainWindowClass : public QMainWindow
{
    Q_OBJECT
public:
    mainWindowClass();
protected:
    void keyPressEvent(QKeyEvent* thisKey);
private:
    respawnIrcClass respawnIrc;
};
#endif
