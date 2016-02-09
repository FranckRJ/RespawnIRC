#ifndef PREFERENCEWINDOW_HPP
#define PREFERENCEWINDOW_HPP

#include <QtWidgets>

class preferenceWindowClass : public QDialog
{
    Q_OBJECT
public:
    preferenceWindowClass(QWidget* parent);
    QCheckBox* makeNewCheckBox(QString messageInfo, QString boxNameValue);
public slots:
    void valueOfCheckboxChanged(bool newVal);
signals:
    void newValueForOption(bool newVal, QString thisOption);
};

#endif
