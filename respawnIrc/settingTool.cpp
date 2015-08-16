#include "settingTool.hpp"

QSettings settingToolClass::setting("config.ini", QSettings::IniFormat);

QString settingToolClass::getPseudoOfUser()
{
    return setting.value("pseudo", "").toString();
}

QList<QNetworkCookie> settingToolClass::getListOfCookie()
{
    QList<QNetworkCookie> newCookies;

    if(setting.value("dlrowolleh", "").toString().isEmpty() == false && setting.value("coniunctio", "").toString().isEmpty() == false)
    {
        newCookies.append(QNetworkCookie(QByteArray("dlrowolleh"), setting.value("dlrowolleh").toByteArray()));
        newCookies.append(QNetworkCookie(QByteArray("coniunctio"), setting.value("coniunctio").toByteArray()));
    }

    return newCookies;
}

QList<QString> settingToolClass::getListOfIgnoredPseudo()
{
    QList<QString> listOfIgnoredPseudo;

    if(setting.value("listOfIgnoredPseudo", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfIgnoredPseudo = createListWithThisQVariantList(setting.value("listOfIgnoredPseudo").toList());
    }

    return listOfIgnoredPseudo;
}

QList<QString> settingToolClass::getListOfTopicLink()
{
    QList<QString> listOfTopicLink;

    if(setting.value("listOfTopicLink", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfTopicLink = createListWithThisQVariantList(setting.value("listOfTopicLink").toList());
    }

    return listOfTopicLink;
}

bool settingToolClass::getShowTextDecorationButton()
{
    return setting.value("showTextDecorationButton", true).toBool();
}

bool settingToolClass::getSetMultilineEdit()
{
    return setting.value("setMultilineEdit", true).toBool();
}

void settingToolClass::savePseudoOfUser(QString newPseudo)
{
    setting.setValue("pseudo", newPseudo);
}

void settingToolClass::saveListOfCookie(QList<QNetworkCookie> newListOfCookie)
{
    for(int i = 0; i < newListOfCookie.size(); ++i)
    {
        setting.setValue(newListOfCookie.at(i).name(), newListOfCookie.at(i).value());
    }
}

void settingToolClass::saveListOfIgnoredPseudo(QList<QString> newList)
{
    setting.setValue("listOfIgnoredPseudo", createQVariantListWithThisList(newList));
}

void settingToolClass::saveListOfTopicLink(QList<QString> newList)
{
    setting.setValue("listOfTopicLink", createQVariantListWithThisList(newList));
}

void settingToolClass::saveShowTextDecoration(bool newVal)
{
    setting.setValue("showTextDecorationButton", newVal);
}

void settingToolClass::saveSetMultilineEdit(bool newVal)
{
    setting.setValue("setMultilineEdit", newVal);
}

QList<QVariant> settingToolClass::createQVariantListWithThisList(QList<QString> list)
{
    QList<QVariant> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        if(list.at(i).isEmpty() == false)
        {
            newList.push_back(list.at(i));
        }
    }

    return newList;
}

QList<QString> settingToolClass::createListWithThisQVariantList(QList<QVariant> list)
{
    QList<QString> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        newList.push_back(list.at(i).toString());
    }

    return newList;
}
