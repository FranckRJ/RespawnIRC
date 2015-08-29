#include "settingTool.hpp"

QSettings settingToolClass::setting("config.ini", QSettings::IniFormat);

QList<accountStruct> settingToolClass::getListOfAccount()
{
    QList<accountStruct> listOfAccount;
    QList<QNetworkCookie> listOfHelloCookie = createCookieListWithThisQVariantList(setting.value("listOfHelloCookie", QList<QVariant>()).toList());
    QList<QNetworkCookie> listOfConnectCookie = createCookieListWithThisQVariantList(setting.value("listOfConnectCookie", QList<QVariant>()).toList());
    QList<QString> listOfPseudo = createStringListWithThisQVariantList(setting.value("listOfPseudo", QList<QVariant>()).toList());

    if(listOfHelloCookie.size() == listOfConnectCookie.size() && listOfConnectCookie.size() == listOfPseudo.size())
    {
        for(int i = 0; i < listOfPseudo.size(); ++i)
        {
            listOfAccount.push_back(accountStruct());
            listOfAccount.back().listOfCookie.append(listOfHelloCookie.at(i));
            listOfAccount.back().listOfCookie.append(listOfConnectCookie.at(i));
            listOfAccount.back().pseudo = listOfPseudo.at(i);
        }
    }

    return listOfAccount;
}

QString settingToolClass::getPseudoOfUser()
{
    return setting.value("pseudo", "").toString();
}

QList<QString> settingToolClass::getListOfPseudoForTopic()
{
    QList<QString> listOfPseudoForTopic;

    if(setting.value("listOfPseudoForTopic", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfPseudoForTopic = createStringListWithThisQVariantList(setting.value("listOfPseudoForTopic").toList());
    }

    return listOfPseudoForTopic;
}

QList<QString> settingToolClass::getListOfIgnoredPseudo()
{
    QList<QString> listOfIgnoredPseudo;

    if(setting.value("listOfIgnoredPseudo", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfIgnoredPseudo = createStringListWithThisQVariantList(setting.value("listOfIgnoredPseudo").toList());
    }

    return listOfIgnoredPseudo;
}

QList<pseudoWithColorStruct> settingToolClass::getListOfColorPseudo()
{
    QList<pseudoWithColorStruct> listOfColorPseudo;
    QList<QString> listOfPseudo = createStringListWithThisQVariantList(setting.value("listOfPseudoForColor", QList<QVariant>()).toList());
    QList<QString> listOfRed = createStringListWithThisQVariantList(setting.value("listOfRedForColor", QList<QVariant>()).toList());
    QList<QString> listOfGreen = createStringListWithThisQVariantList(setting.value("listOfGreenForColor", QList<QVariant>()).toList());
    QList<QString> listOfBlue = createStringListWithThisQVariantList(setting.value("listOfBlueForColor", QList<QVariant>()).toList());

    if(listOfPseudo.size() == listOfRed.size() && listOfRed.size() == listOfGreen.size() && listOfGreen.size() == listOfBlue.size())
    {
        for(int i = 0; i < listOfPseudo.size(); ++i)
        {
            listOfColorPseudo.push_back(pseudoWithColorStruct());
            listOfColorPseudo.back().pseudo = listOfPseudo.at(i);
            listOfColorPseudo.back().red = listOfRed.at(i).toInt();
            listOfColorPseudo.back().green = listOfGreen.at(i).toInt();
            listOfColorPseudo.back().blue = listOfBlue.at(i).toInt();
        }
    }

    return listOfColorPseudo;
}

QList<QString> settingToolClass::getListOfTopicLink()
{
    QList<QString> listOfTopicLink;

    if(setting.value("listOfTopicLink", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfTopicLink = createStringListWithThisQVariantList(setting.value("listOfTopicLink").toList());
    }

    return listOfTopicLink;
}

bool settingToolClass::getShowQuoteButton()
{
    return setting.value("showQuoteButton", true).toBool();
}

bool settingToolClass::getShowBlacklistButton()
{
    return setting.value("showBlacklistButton", true).toBool();
}

bool settingToolClass::getShowEditButton()
{
    return setting.value("showEditButton", true).toBool();
}

bool settingToolClass::getShowTextDecorationButton()
{
    return setting.value("showTextDecorationButton", true).toBool();
}

bool settingToolClass::getSetMultilineEdit()
{
    return setting.value("setMultilineEdit", true).toBool();
}

bool settingToolClass::getLoadTwoLastPage()
{
    return setting.value("loadTwoLastPage", false).toBool();
}

bool settingToolClass::getIgnoreNetworkError()
{
    return setting.value("ignoreNetworkError", false).toBool();
}

bool settingToolClass::getSearchForUpdateAtLaunch()
{
    return setting.value("searchForUpdateAtLaunch", true).toBool();
}

int settingToolClass::getUpdateTopicTime()
{
    return setting.value("updateTopicTime", 4000).toInt();
}

int settingToolClass::getNumberOfMessageShowedFirstTime()
{
    return setting.value("numberOfMessageShowedFirstTime", 10).toInt();
}

void settingToolClass::saveListOfAccount(QList<accountStruct>& newListOfAccount)
{
    QList<QNetworkCookie> listOfHelloCookie;
    QList<QNetworkCookie> listOfConnectCookie;
    QList<QString> listOfPseudo;

    for(int i = 0; i < newListOfAccount.size(); ++i)
    {
        for(int j = 0; j < newListOfAccount.at(i).listOfCookie.size(); ++j)
        {
            if(newListOfAccount.at(i).listOfCookie.at(j).name() == "dlrowolleh")
            {
                listOfHelloCookie.push_back(newListOfAccount.at(i).listOfCookie.at(j));
            }
            else if(newListOfAccount.at(i).listOfCookie.at(j).name() == "coniunctio")
            {
                listOfConnectCookie.push_back(newListOfAccount.at(i).listOfCookie.at(j));
            }
        }

        listOfPseudo.push_back(newListOfAccount.at(i).pseudo);
    }

    setting.setValue("listOfHelloCookie", createQVariantListWithThisList(listOfHelloCookie));
    setting.setValue("listOfConnectCookie", createQVariantListWithThisList(listOfConnectCookie));
    setting.setValue("listOfPseudo", createQVariantListWithThisList(listOfPseudo));
}

void settingToolClass::savePseudoOfUser(QString newPseudo)
{
    setting.setValue("pseudo", newPseudo);
}

void settingToolClass::saveListOfPseudoForTopic(QList<QString>& newList)
{
    setting.setValue("listOfPseudoForTopic", createQVariantListWithThisList(newList, false));
}

void settingToolClass::saveListOfIgnoredPseudo(QList<QString>& newList)
{
    setting.setValue("listOfIgnoredPseudo", createQVariantListWithThisList(newList));
}

void settingToolClass::saveListOfColorPseudo(QList<pseudoWithColorStruct>& newListOfColorPseudo)
{
    QList<QString> pseudoList;
    QList<QString> redList;
    QList<QString> greenList;
    QList<QString> blueList;

    for(int i = 0; i < newListOfColorPseudo.size(); ++i)
    {
        pseudoList.push_back(newListOfColorPseudo.at(i).pseudo);
        redList.push_back(QString::number(newListOfColorPseudo.at(i).red));
        greenList.push_back(QString::number(newListOfColorPseudo.at(i).green));
        blueList.push_back(QString::number(newListOfColorPseudo.at(i).blue));
    }

    setting.setValue("listOfPseudoForColor", createQVariantListWithThisList(pseudoList));
    setting.setValue("listOfRedForColor", createQVariantListWithThisList(redList));
    setting.setValue("listOfGreenForColor", createQVariantListWithThisList(greenList));
    setting.setValue("listOfBlueForColor", createQVariantListWithThisList(blueList));
}

void settingToolClass::saveListOfTopicLink(QList<QString>& newList)
{
    setting.setValue("listOfTopicLink", createQVariantListWithThisList(newList));
}

void settingToolClass::saveShowQuoteButton(bool newVal)
{
    setting.setValue("showQuoteButton", newVal);
}

void settingToolClass::saveShowBlacklistButton(bool newVal)
{
    setting.setValue("showBlacklistButton", newVal);
}

void settingToolClass::saveShowEditButton(bool newVal)
{
    setting.setValue("showEditButton", newVal);
}

void settingToolClass::saveShowTextDecoration(bool newVal)
{
    setting.setValue("showTextDecorationButton", newVal);
}

void settingToolClass::saveSetMultilineEdit(bool newVal)
{
    setting.setValue("setMultilineEdit", newVal);
}

void settingToolClass::saveLoadTwoLastPage(bool newVal)
{
    setting.setValue("loadTwoLastPage", newVal);
}

void settingToolClass::saveIgnoreNetworkError(bool newVal)
{
    setting.setValue("ignoreNetworkError", newVal);
}

void settingToolClass::saveSearchForUpdateAtLaunch(bool newVal)
{
    setting.setValue("searchForUpdateAtLaunch", newVal);
}

void settingToolClass::saveUpdateTopicTime(int newTime)
{
    setting.setValue("updateTopicTime", newTime);
}

void settingToolClass::saveNumberOfMessageShowedFirstTime(int newNumber)
{
    setting.setValue("numberOfMessageShowedFirstTime", newNumber);
}

QList<QVariant> settingToolClass::createQVariantListWithThisList(QList<QString> list, bool deleteEmptyString)
{
    QList<QVariant> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        if(list.at(i).isEmpty() == false || deleteEmptyString == false)
        {
            newList.push_back(list.at(i));
        }
    }

    return newList;
}

QList<QVariant> settingToolClass::createQVariantListWithThisList(QList<QNetworkCookie> list)
{
    QList<QVariant> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        newList.push_back(list.at(i).toRawForm());
    }

    return newList;
}

QList<QString> settingToolClass::createStringListWithThisQVariantList(QList<QVariant> list)
{
    QList<QString> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        newList.push_back(list.at(i).toString());
    }

    return newList;
}

QList<QNetworkCookie> settingToolClass::createCookieListWithThisQVariantList(QList<QVariant> list)
{
    QList<QNetworkCookie> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        newList.push_back(QNetworkCookie::parseCookies(list.at(i).toByteArray()).first());
    }

    return newList;
}
