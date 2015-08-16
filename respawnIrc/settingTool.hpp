#ifndef SETTINGTOOL_HPP
#define SETTINGTOOL_HPP

#include <QtCore>
#include <QtNetwork>

class settingToolClass
{
public:
    static QString getPseudoOfUser();
    static QList<QNetworkCookie> getListOfCookie();
    static QList<QString> getListOfIgnoredPseudo();
    static QList<QString> getListOfTopicLink();
    static bool getShowTextDecorationButton();
    static bool getSetMultilineEdit();
    static void savePseudoOfUser(QString newPseudo);
    static void saveListOfCookie(QList<QNetworkCookie> newListOfCookie);
    static void saveListOfIgnoredPseudo(QList<QString> newList);
    static void saveListOfTopicLink(QList<QString> newList);
    static void saveShowTextDecoration(bool newVal);
    static void saveSetMultilineEdit(bool newVal);
    static QList<QVariant> createQVariantListWithThisList(QList<QString> list);
    static QList<QString> createListWithThisQVariantList(QList<QVariant> list);
private:
    static QSettings setting;
};

#endif
