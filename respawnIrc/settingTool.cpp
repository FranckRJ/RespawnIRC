#include "settingTool.hpp"

QSettings* settingToolClass::setting = nullptr;
QMap<QString, bool> settingToolClass::listOfDefaultBoolOption;
QMap<QString, int> settingToolClass::listOfDefaultIntOption;
QMap<QString, QString> settingToolClass::listOfDefaultStringOption;
QMap<QString, QByteArray> settingToolClass::listOfDefaultByteOption;

void settingToolClass::setSettings(QSettings* newSetting)
{
    setting = newSetting;
}

void settingToolClass::initializeDefaultListsOption()
{
    listOfDefaultBoolOption["showQuoteButton"] = true;
    listOfDefaultBoolOption["showBlacklistButton"] = true;
    listOfDefaultBoolOption["showEditButton"] = true;
    listOfDefaultBoolOption["showTextDecorationButton"] = true;
    listOfDefaultBoolOption["showListOfTopic"] = true;
    listOfDefaultBoolOption["setMultilineEdit"] = true;
    listOfDefaultBoolOption["loadTwoLastPage"] = true;
    listOfDefaultBoolOption["ignoreNetworkError"] = false;
    listOfDefaultBoolOption["searchForUpdateAtLaunch"] = true;
    listOfDefaultBoolOption["saveWindowGeometry"] = true;
    listOfDefaultBoolOption["beepWhenWarn"] = true;
    listOfDefaultBoolOption["showStickers"] = true;
    listOfDefaultBoolOption["useSpellChecker"] = true;
    listOfDefaultBoolOption["warnUser"] = true;
    listOfDefaultBoolOption["getFirstMessageOfTopic"] = false;
    listOfDefaultBoolOption["colorModoAndAdminPseudo"] = true;
    listOfDefaultBoolOption["colorPEMT"] = true;
    listOfDefaultBoolOption["colorUserPseudoInMessages"] = true;
    listOfDefaultBoolOption["warnWhenEdit"] = true;
    listOfDefaultIntOption["updateTopicTime"] = 3500;
    listOfDefaultIntOption["numberOfMessageShowedFirstTime"] = 10;
    listOfDefaultIntOption["stickersSize"] = 70;
    listOfDefaultIntOption["timeoutInSecond"] = 10;
    listOfDefaultStringOption["pseudo"] = "";
    listOfDefaultStringOption["themeUsed"] = "";

    for(int i = 0; i < 10; ++i)
    {
        listOfDefaultStringOption["favoriteLink" + QString::number(i)] = "";
        listOfDefaultStringOption["favoriteName" + QString::number(i)] = "";
    }

    listOfDefaultByteOption["windowGeometry"] = "";
}


QAction* settingToolClass::createActionForBoolOption(QString actionName, QString optionName, QMenu* menuForAction, QObject* pointer, const char* method)
{
    QAction* newAction = menuForAction->addAction(actionName);
    newAction->setCheckable(true);
    newAction->setChecked(getThisBoolOption(optionName));
    newAction->setObjectName(optionName);

    QObject::connect(newAction, SIGNAL(toggled(bool)), pointer, method);

    return newAction;
}

bool settingToolClass::getThisBoolOption(QString optionName)
{
    QMap<QString, bool>::iterator iteForList = listOfDefaultBoolOption.find(optionName);

    if(iteForList != listOfDefaultBoolOption.end())
    {
        return setting->value(optionName, iteForList.value()).toBool();
    }
    else
    {
        qDebug() << "Erreur : cette option booleenne \"" + optionName + "\" n existe pas.";
        return setting->value(optionName, false).toBool();
    }
}

int settingToolClass::getThisIntOption(QString optionName)
{
    QMap<QString, int>::iterator iteForList = listOfDefaultIntOption.find(optionName);

    if(iteForList != listOfDefaultIntOption.end())
    {
        return setting->value(optionName, iteForList.value()).toInt();
    }
    else
    {
        qDebug() << "Erreur : cette option entiere \"" + optionName + "\" n existe pas.";
        return setting->value(optionName, 0).toInt();
    }
}

QString settingToolClass::getThisStringOption(QString optionName)
{
    QMap<QString, QString>::iterator iteForList = listOfDefaultStringOption.find(optionName);

    if(iteForList != listOfDefaultStringOption.end())
    {
        return setting->value(optionName, iteForList.value()).toString();
    }
    else
    {
        qDebug() << "Erreur : cette option string \"" + optionName + "\" n existe pas.";
        return setting->value(optionName, "").toString();
    }
}

QByteArray settingToolClass::getThisByteOption(QString optionName)
{
    QMap<QString, QByteArray>::iterator iteForList = listOfDefaultByteOption.find(optionName);

    if(iteForList != listOfDefaultByteOption.end())
    {
        return setting->value(optionName, iteForList.value()).toByteArray();
    }
    else
    {
        qDebug() << "Erreur : cette option byte \"" + optionName + "\" n existe pas.";
        return setting->value(optionName, "").toByteArray();
    }
}

QList<accountStruct> settingToolClass::getListOfAccount()
{
    QList<accountStruct> listOfAccount;
    QList<QNetworkCookie> listOfHelloCookie = createCookieListWithThisQVariantList(setting->value("listOfHelloCookie", QList<QVariant>()).toList());
    QList<QNetworkCookie> listOfConnectCookie = createCookieListWithThisQVariantList(setting->value("listOfConnectCookie", QList<QVariant>()).toList());
    QList<QString> listOfPseudo = createStringListWithThisQVariantList(setting->value("listOfPseudo", QList<QVariant>()).toList());

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

QList<QString> settingToolClass::getListOfPseudoForTopic()
{
    QList<QString> listOfPseudoForTopic;

    if(setting->value("listOfPseudoForTopic", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfPseudoForTopic = createStringListWithThisQVariantList(setting->value("listOfPseudoForTopic").toList());
    }

    return listOfPseudoForTopic;
}

QList<QString> settingToolClass::getListOfIgnoredPseudo()
{
    QList<QString> listOfIgnoredPseudo;

    if(setting->value("listOfIgnoredPseudo", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfIgnoredPseudo = createStringListWithThisQVariantList(setting->value("listOfIgnoredPseudo").toList());
    }

    return listOfIgnoredPseudo;
}

QList<pseudoWithColorStruct> settingToolClass::getListOfColorPseudo()
{
    QList<pseudoWithColorStruct> listOfColorPseudo;
    QList<QString> listOfPseudo = createStringListWithThisQVariantList(setting->value("listOfPseudoForColor", QList<QVariant>()).toList());
    QList<QString> listOfRed = createStringListWithThisQVariantList(setting->value("listOfRedForColor", QList<QVariant>()).toList());
    QList<QString> listOfGreen = createStringListWithThisQVariantList(setting->value("listOfGreenForColor", QList<QVariant>()).toList());
    QList<QString> listOfBlue = createStringListWithThisQVariantList(setting->value("listOfBlueForColor", QList<QVariant>()).toList());

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

    if(setting->value("listOfTopicLink", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfTopicLink = createStringListWithThisQVariantList(setting->value("listOfTopicLink").toList());
    }

    return listOfTopicLink;
}

void settingToolClass::saveThisOption(QString optionName, QVariant value)
{
    setting->setValue(optionName, value);
}

void settingToolClass::saveListOfAccount(QList<accountStruct>& newListOfAccount)
{
    QList<QNetworkCookie> listOfHelloCookie;
    QList<QNetworkCookie> listOfConnectCookie;
    QList<QString> listOfPseudo;

    for(const accountStruct& thisAccout : newListOfAccount)
    {
        for(const QNetworkCookie& thisCookie : thisAccout.listOfCookie)
        {
            if(thisCookie.name() == "dlrowolleh")
            {
                listOfHelloCookie.push_back(thisCookie);
            }
            else if(thisCookie.name() == "coniunctio")
            {
                listOfConnectCookie.push_back(thisCookie);
            }
        }

        listOfPseudo.push_back(thisAccout.pseudo);
    }

    setting->setValue("listOfHelloCookie", createQVariantListWithThisList(listOfHelloCookie));
    setting->setValue("listOfConnectCookie", createQVariantListWithThisList(listOfConnectCookie));
    setting->setValue("listOfPseudo", createQVariantListWithThisList(listOfPseudo));
}

void settingToolClass::saveListOfPseudoForTopic(QList<QString>& newList)
{
    setting->setValue("listOfPseudoForTopic", createQVariantListWithThisList(newList, false));
}

void settingToolClass::saveListOfIgnoredPseudo(QList<QString>& newList)
{
    setting->setValue("listOfIgnoredPseudo", createQVariantListWithThisList(newList));
}

void settingToolClass::saveListOfColorPseudo(QList<pseudoWithColorStruct>& newListOfColorPseudo)
{
    QList<QString> pseudoList;
    QList<QString> redList;
    QList<QString> greenList;
    QList<QString> blueList;

    for(const pseudoWithColorStruct& thisColor : newListOfColorPseudo)
    {
        pseudoList.push_back(thisColor.pseudo);
        redList.push_back(QString::number(thisColor.red));
        greenList.push_back(QString::number(thisColor.green));
        blueList.push_back(QString::number(thisColor.blue));
    }

    setting->setValue("listOfPseudoForColor", createQVariantListWithThisList(pseudoList));
    setting->setValue("listOfRedForColor", createQVariantListWithThisList(redList));
    setting->setValue("listOfGreenForColor", createQVariantListWithThisList(greenList));
    setting->setValue("listOfBlueForColor", createQVariantListWithThisList(blueList));
}

void settingToolClass::saveListOfTopicLink(QList<QString>& newList)
{
    setting->setValue("listOfTopicLink", createQVariantListWithThisList(newList));
}

QList<QVariant> settingToolClass::createQVariantListWithThisList(QList<QString> list, bool deleteEmptyString)
{
    QList<QVariant> newList;

    for(const QString& thisString : list)
    {
        if(thisString.isEmpty() == false || deleteEmptyString == false)
        {
            newList.push_back(thisString);
        }
    }

    return newList;
}

QList<QVariant> settingToolClass::createQVariantListWithThisList(QList<QNetworkCookie> list)
{
    QList<QVariant> newList;

    for(const QNetworkCookie& thisCookie : list)
    {
        newList.push_back(thisCookie.toRawForm());
    }

    return newList;
}

QList<QString> settingToolClass::createStringListWithThisQVariantList(QList<QVariant> list)
{
    QList<QString> newList;

    for(const QVariant& thisVariant : list)
    {
        newList.push_back(thisVariant.toString());
    }

    return newList;
}

QList<QNetworkCookie> settingToolClass::createCookieListWithThisQVariantList(QList<QVariant> list)
{
    QList<QNetworkCookie> newList;

    for(const QVariant& thisVariant : list)
    {
        newList.push_back(QNetworkCookie::parseCookies(thisVariant.toByteArray()).first());
    }

    return newList;
}
