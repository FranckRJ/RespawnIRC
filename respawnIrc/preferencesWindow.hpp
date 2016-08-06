#ifndef PREFERENCEWINDOW_HPP
#define PREFERENCEWINDOW_HPP

#include <QtWidgets>

class preferenceWindowClass : public QDialog
{
    Q_OBJECT
public:
    preferenceWindowClass(QWidget* parent);
    QWidget* createWidgetForMainTab();
    QWidget* createWidgetForImageTab();
    QWidget* createWidgetForMessageAndTopicStyleTab();
    QCheckBox* makeNewCheckBox(QString messageInfo, QString boxNameValue);
    QHBoxLayout* makeNewSpinBox(QString messageInfo, QString boxNameValue);
    QHBoxLayout* makeNewComboBox(QString messageInfo, QString boxNameValue, QStringList listOfChoices);
public slots:
    void valueOfCheckboxChanged(bool newVal);
    void valueOfIntBoxChanged(int newVal);
signals:
    void newValueForBoolOption(bool newVal, QString thisOption);
    void newValueForIntOption(int newVal, QString thisOption);
};

#endif
