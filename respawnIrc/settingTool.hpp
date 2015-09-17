#ifndef SETTINGTOOL_HPP
#define SETTINGTOOL_HPP

#include <QtCore>
#include <QtNetwork>

#include "accountListWindow.hpp"
#include "colorPseudoListWindow.hpp"

class settingToolClass
{
public:
    static void initializeDefaultListsOption();
    static QAction* createActionForBoolOption(QString actionName, QString optionName, QMenu* menuForAction);
    static bool getThisBoolOption(QString optionName);
    static int getThisIntOption(QString optionName);
    static QList<accountStruct> getListOfAccount();
    static QString getPseudoOfUser();
    static QList<QString> getListOfPseudoForTopic();
    static QList<QString> getListOfIgnoredPseudo();
    static QList<pseudoWithColorStruct> getListOfColorPseudo();
    static QList<QString> getListOfTopicLink();
    static void saveThisBoolOption(QString optionName, bool value);
    static void saveThisIntOption(QString optionName, int value);
    static void saveListOfAccount(QList<accountStruct>& newListOfAccount);
    static void savePseudoOfUser(QString newPseudo);
    static void saveListOfPseudoForTopic(QList<QString>& newList);
    static void saveListOfIgnoredPseudo(QList<QString>& newList);
    static void saveListOfColorPseudo(QList<pseudoWithColorStruct>& newListOfColorPseudo);
    static void saveListOfTopicLink(QList<QString>& newList);
    static QList<QVariant> createQVariantListWithThisList(QList<QString> list, bool deleteEmptyString = true);
    static QList<QVariant> createQVariantListWithThisList(QList<QNetworkCookie> list);
    static QList<QString> createStringListWithThisQVariantList(QList<QVariant> list);
    static QList<QNetworkCookie> createCookieListWithThisQVariantList(QList<QVariant> list);
private:
    static QSettings setting;
    static QMap<QString, bool> listOfDefaultBoolOption;
    static QMap<QString, int> listOfDefaultIntOption;
};

#endif
