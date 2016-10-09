#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QVector>
#include <QAction>
#include <QString>
#include <QKeyEvent>
#include <QCloseEvent>

#include "respawnIrc.hpp"

class mainWindowClass : public QMainWindow
{
    Q_OBJECT
public:
    explicit mainWindowClass();
public slots:
    void doStuffBeforeQuit();
    void goToMp();
    void useFavoriteClicked();
    void addFavoriteClicked();
    void delFavoriteClicked();
    void setNewTheme(QString newThemeName);
    void saveWindowGeometry(bool newVal);
protected:
    void keyPressEvent(QKeyEvent* thisKey);
    void closeEvent(QCloseEvent* event);
private:
    respawnIrcClass respawnIrc;
    QVector<QAction*> vectorOfUseFavorite;
    QVector<QAction*> vectorOfAddFavorite;
    QVector<QAction*> vectorOfDelFavorite;
};
#endif
