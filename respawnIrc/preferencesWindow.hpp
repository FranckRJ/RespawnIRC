#ifndef PREFERENCEWINDOW_HPP
#define PREFERENCEWINDOW_HPP

#include <QDialog>
#include <QMap>
#include <QString>
#include <QWidget>
#include <QCheckBox>
#include <QStringList>
#include <QHBoxLayout>

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
    void applySettingsAndClose();
    void applySettings();
signals:
    void setApplyButtonEnable(bool newVal);
    void newSettingsAvailable(QMap<QString, bool> newBoolOptions, QMap<QString, int> newIntOptions);
private:
    QMap<QString, bool> listOfBoolOptionChanged;
    QMap<QString, int> listOfIntOptionChanged;
};

#endif
