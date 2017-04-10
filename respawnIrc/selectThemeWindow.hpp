#ifndef SELECTTHEMEWINDOW_HPP
#define SELECTTHEMEWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QListView>
#include <QStringListModel>
#include <QModelIndex>
#include <QLabel>
#include <QString>

#include "viewThemeInfos.hpp"

class selectThemeWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit selectThemeWindowClass(QString newCurrentThemeName, QWidget* parent);
private:
    void loadListOfThemes();
private slots:
    void selectThisTheme();
    void changeThemeInfos(const QModelIndex& selection);
signals:
    void newThemeSelected(QString newTheme);
private:
    QListView* listViewOfTheme;
    QStringListModel* modelForListViewOfTheme;
    QLabel* currentThemeName;
    viewThemeInfosClass* viewThemeInfos;
};

#endif
