#ifndef VIEWTHEMEINFOS_HPP
#define VIEWTHEMEINFOS_HPP

#include <QWidget>
#include <QString>
#include <QLabel>

class viewThemeInfosClass : public QWidget
{
    Q_OBJECT
public:
    explicit viewThemeInfosClass(QWidget* parent = 0);
    void setThemeToShow(QString newThemeName);
    QString boolToYesOrNo(bool boolToCheck);
private:
    QLabel* informations;
};

#endif
