#ifndef SETTINGTOOL_HPP
#define SETTINGTOOL_HPP

#include <QList>
#include <QSettings>
#include <QString>
#include <QByteArray>
#include <QVariant>
#include <QNetworkCookie>

#include "accountListWindow.hpp"
#include "colorPseudoListWindow.hpp"

struct intSettingStruct
{
    int minValue;
    int maxValue;
    int value;
};

namespace settingToolClass
{
    void setSettings(QSettings* newSetting);
    void initializeDefaultListsOption();
    bool getThisBoolOption(QString optionName);
    intSettingStruct getThisIntOption(QString optionName);
    QString getThisStringOption(QString optionName);
    QByteArray getThisByteOption(QString optionName);
    QList<accountStruct> getListOfAccount();
    QList<QString> getListOfPseudoForTopic();
    QList<QString> getListOfIgnoredPseudo();
    QList<pseudoWithColorStruct> getListOfColorPseudo();
    QList<QString> getListOfTopicLink();
    void saveThisOption(QString optionName, QVariant value);
    void saveListOfAccount(QList<accountStruct>& newListOfAccount);
    void saveListOfPseudoForTopic(QList<QString>& newList);
    void saveListOfIgnoredPseudo(QList<QString>& newList);
    void saveListOfColorPseudo(QList<pseudoWithColorStruct>& newListOfColorPseudo);
    void saveListOfTopicLink(QList<QString>& newList);
    QList<QVariant> createQVariantListWithThisList(QList<QString> list, bool deleteEmptyString = true);
    QList<QVariant> createQVariantListWithThisList(QList<QNetworkCookie> list);
    QList<QString> createStringListWithThisQVariantList(QList<QVariant> list);
    QList<QNetworkCookie> createCookieListWithThisQVariantList(QList<QVariant> list);
    void forceSync();
}

#endif
