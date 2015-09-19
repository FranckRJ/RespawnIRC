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
public slots:
    void useFavoriteClicked();
    void addFavoriteClicked();
    void delFavoriteClicked();
protected:
    void keyPressEvent(QKeyEvent* thisKey);
private:
    respawnIrcClass respawnIrc;
    QVector<QAction*> vectorOfUseFavorite;
    QVector<QAction*> vectorOfAddFavorite;
    QVector<QAction*> vectorOfDelFavorite;
};
#endif
