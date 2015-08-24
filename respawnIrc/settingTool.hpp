#ifndef SETTINGTOOL_HPP
#define SETTINGTOOL_HPP

#include <QtCore>
#include <QtNetwork>

#include "accountlistwindow.hpp"

class settingToolClass
{
public:
    static QList<accountStruct> getListOfAccount();
    static QString getPseudoOfUser();
    static QList<QString> getListOfPseudoForTopic();
    static QList<QString> getListOfIgnoredPseudo();
    static QList<QString> getListOfTopicLink();
    static bool getShowTextDecorationButton();
    static bool getSetMultilineEdit();
    static bool getLoadTwoLastPage();
    static bool getSearchForUpdateAtLaunch();
    static int getUpdateTopicTime();
    static int getNumberOfMessageShowedFirstTime();
    static void saveListOfAccount(QList<accountStruct> newListOfAccount);
    static void savePseudoOfUser(QString newPseudo);
    static void saveListOfPseudoForTopic(QList<QString> newList);
    static void saveListOfIgnoredPseudo(QList<QString> newList);
    static void saveListOfTopicLink(QList<QString> newList);
    static void saveShowTextDecoration(bool newVal);
    static void saveSetMultilineEdit(bool newVal);
    static void saveLoadTwoLastPage(bool newVal);
    static void saveSearchForUpdateAtLaunch(bool newVal);
    static void saveUpdateTopicTime(int newTime);
    static void saveNumberOfMessageShowedFirstTime(int newNumber);
    static QList<QVariant> createQVariantListWithThisList(QList<QString> list, bool deleteEmptyString = true);
    static QList<QVariant> createQVariantListWithThisList(QList<QNetworkCookie> list);
    static QList<QString> createStringListWithThisQVariantList(QList<QVariant> list);
    static QList<QNetworkCookie> createCookieListWithThisQVariantList(QList<QVariant> list);
private:
    static QSettings setting;
};

#endif
