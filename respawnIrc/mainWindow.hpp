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
private:
    void keyPressEvent(QKeyEvent* thisKey) override;
    void closeEvent(QCloseEvent* event) override;
private slots:
    void doStuffBeforeQuit();
    void goToMp();
    void useFavoriteClicked();
    void addFavoriteClicked();
    void delFavoriteClicked();
    void setNewTheme(QString newThemeName);
private:
    respawnIrcClass respawnIrc;
    QVector<QAction*> vectorOfUseFavorite;
    QVector<QAction*> vectorOfAddFavorite;
    QVector<QAction*> vectorOfDelFavorite;
};
#endif
