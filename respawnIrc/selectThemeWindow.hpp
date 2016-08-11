#ifndef SELECTTHEMEWINDOW_HPP
#define SELECTTHEMEWINDOW_HPP

#include <QDialog>
#include <QListView>
#include <QStringListModel>
#include <QLabel>

class QString;
class QWidget;

class selectThemeWindowClass : public QDialog
{
    Q_OBJECT
public:
    selectThemeWindowClass(QString newCurrentThemeName, QWidget* parent);
    void loadListOfThemes();
public slots:
    void selectThisTheme();
signals:
    void newThemeSelected(QString newTheme);
private:
    QListView listViewOfTheme;
    QStringListModel modelForListViewOfTheme;
    QLabel currentThemeName;
};

#endif
