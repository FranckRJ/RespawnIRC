#ifndef PREFERENCEWINDOW_HPP
#define PREFERENCEWINDOW_HPP

#include <QtWidgets>

class preferenceWindowClass : public QDialog
{
    Q_OBJECT
public:
    preferenceWindowClass(QWidget* parent);
    QCheckBox* makeNewCheckBox(QString messageInfo, QString boxNameValue);
    QHBoxLayout* makeNewSpinBox(QString messageInfo, QString boxNameValue);
public slots:
    void valueOfCheckboxChanged(bool newVal);
    void valueOfSpinboxChanged(int newVal);
signals:
    void newValueForBoolOption(bool newVal, QString thisOption);
    void newValueForIntOption(int newVal, QString thisOption);
};

#endif
