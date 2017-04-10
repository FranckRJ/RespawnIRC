#ifndef VIEWTHEMEINFOS_HPP
#define VIEWTHEMEINFOS_HPP

#include <QWidget>
#include <QString>
#include <QLabel>

class viewThemeInfosClass : public QWidget
{
    Q_OBJECT
public:
    explicit viewThemeInfosClass(QWidget* parent = nullptr);
    void setThemeToShow(QString newThemeName);
private:
    QString boolToYesOrNo(bool boolToCheck) const;
private:
    QLabel* informations;
};

#endif
