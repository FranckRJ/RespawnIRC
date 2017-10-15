#ifndef PREFERENCEWINDOW_HPP
#define PREFERENCEWINDOW_HPP

#include <QMap>
#include <QString>
#include <QWidget>
#include <QCheckBox>
#include <QStringList>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>

#include "baseDialog.hpp"

class preferenceWindowClass : public baseDialogClass
{
    Q_OBJECT
public:
    explicit preferenceWindowClass(QWidget* parent);
private:
    QWidget* createWidgetForMainTab();
    QWidget* createWidgetForMessagesTab();
    QWidget* createWidgetForTopicListTab();
    QWidget* createWidgetForImageTab();
    QCheckBox* makeNewCheckBox(QString messageInfo, QString boxNameValue, QCheckBox* useThisCheckBox = nullptr);
    QHBoxLayout* makeNewSpinBox(QString messageInfo, QString boxNameValue, QSpinBox* useThisSpinBox = nullptr, QHBoxLayout* useThisLayout = nullptr);
    QHBoxLayout* makeNewComboBox(QString messageInfo, QString boxNameValue, QStringList listOfChoices, QComboBox* useThisCombokBox = nullptr, QHBoxLayout* useThisLayout = nullptr);
    QCheckBox* makeFastModeCheckBox();
private slots:
    void valueOfCheckboxChanged(bool newVal);
    void valueOfIntBoxChanged(int newVal);
    void valueOfFastModeCheckBoxChanged(bool newVal);
    void applySettingsAndClose();
    void applySettings();
signals:
    void setApplyButtonEnable(bool newVal);
    void newSettingsAvailable(QMap<QString, bool> newBoolOptions, QMap<QString, int> newIntOptions);
private:
    QMap<QString, bool> listOfBoolOptionChanged;
    QMap<QString, int> listOfIntOptionChanged;
    QComboBox* typeOfPageLoad;
    QHBoxLayout* layoutTypeOfPageLoad;
    QCheckBox* showTopicListEnabled;
    QSpinBox* topicRefreshTimeNumber;
    QHBoxLayout* layoutTopicRefreshTimeNumber;
    bool expertMode;
    bool updateValues = true;
};

#endif
