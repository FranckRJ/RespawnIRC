#ifndef SETTINGTOOL_HPP
#define SETTINGTOOL_HPP

class QSettings;
class QString;
class QByteArray;
class QVariant;
class QNetworkCookie;
struct accountStruct;
struct pseudoWithColorStruct;

#include <QList>

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
}

#endif
