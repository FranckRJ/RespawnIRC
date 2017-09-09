#include <QDebug>
#include <QDateTime>
#include <QMap>

#include "settingTool.hpp"

namespace
{
    QSettings* setting = nullptr;
    QMap<QString, bool> listOfDefaultBoolOption;
    QMap<QString, intSettingStruct> listOfDefaultIntOption;
    QMap<QString, QString> listOfDefaultStringOption;
    QMap<QString, QByteArray> listOfDefaultByteOption;
}

void settingTool::setSettings(QSettings* newSetting)
{
    setting = newSetting;
}

void settingTool::initializeDefaultListsOption()
{
    listOfDefaultBoolOption["showQuoteButton"] = true;
    listOfDefaultBoolOption["disableSelfQuoteButton"] = false;
    listOfDefaultBoolOption["showBlacklistButton"] = true;
    listOfDefaultBoolOption["showEditButton"] = true;
    listOfDefaultBoolOption["showDeleteButton"] = false;
    listOfDefaultBoolOption["showTextDecorationButton"] = true;
    listOfDefaultBoolOption["showListOfTopic"] = true;
    listOfDefaultBoolOption["setMultilineEdit"] = true;
    listOfDefaultBoolOption["ignoreNetworkError"] = false;
    listOfDefaultBoolOption["searchForUpdateAtLaunch"] = true;
    listOfDefaultBoolOption["saveWindowGeometry"] = true;
    listOfDefaultBoolOption["beepWhenWarn"] = true;
    listOfDefaultBoolOption["showStickers"] = true;
    listOfDefaultBoolOption["stickersToSmiley"] = false;
    listOfDefaultBoolOption["useSpellChecker"] = true;
    listOfDefaultBoolOption["warnUser"] = true;
    listOfDefaultBoolOption["getFirstMessageOfTopic"] = false;
    listOfDefaultBoolOption["colorModoAndAdminPseudo"] = true;
    listOfDefaultBoolOption["colorModoAndAdminTopicInTopicList"] = true;
    listOfDefaultBoolOption["colorPEMT"] = true;
    listOfDefaultBoolOption["colorUserPseudoInMessages"] = true;
    listOfDefaultBoolOption["warnWhenEdit"] = true;
    listOfDefaultBoolOption["betterQuote"] = true;
    listOfDefaultBoolOption["changeColorOnEdit"] = true;
    listOfDefaultBoolOption["showNumberOfMessagesInTopicList"] = true;
    listOfDefaultBoolOption["cutLongTopicNameInTopicList"] = true;
    listOfDefaultBoolOption["downloadMissingStickers"] = true;
    listOfDefaultBoolOption["downloadNoelshackImages"] = true;
    listOfDefaultBoolOption["expertMode"] = false;
    listOfDefaultBoolOption["warnOnFirstTime"] = true;
    listOfDefaultBoolOption["showSignatures"] = false;
    listOfDefaultBoolOption["showAvatars"] = false;
    listOfDefaultBoolOption["showPinnedTagOnTopicInTopicList"] = true;
    listOfDefaultBoolOption["showHotTagOnTopicInTopicList"] = true;
    listOfDefaultBoolOption["showLockTagOnTopicInTopicList"] = true;
    listOfDefaultBoolOption["showResolvedTagOnTopicInTopicList"] = true;
    listOfDefaultBoolOption["showGhostTagOnTopicInTopicList"] = true;
    listOfDefaultBoolOption["showNormalTagOnTopicInTopicList"] = true;
    listOfDefaultBoolOption["useIconInsteadOfTagInTopicList"] = true;
    listOfDefaultBoolOption["fastModeEnbled"] = false;
    listOfDefaultBoolOption["beepForNewMP"] = true;
    listOfDefaultBoolOption["hideUglyImages"] = false;
    listOfDefaultBoolOption["useInternalNavigatorForLinks"] = false;
    listOfDefaultBoolOption["betterCodeTag"] = true;
    listOfDefaultBoolOption["smartAvatarResizing"] = true;
    listOfDefaultBoolOption["downloadHighDefAvatar"] = false;
    listOfDefaultBoolOption["smartNoelshackResizing"] = true;
    listOfDefaultIntOption["updateTopicTime"].value = 7500;
    listOfDefaultIntOption["updateTopicTime"].minValue = 2000;
    listOfDefaultIntOption["updateTopicTime"].maxValue = 60000;
    listOfDefaultIntOption["numberOfMessageShowedFirstTime"].value = 10;
    listOfDefaultIntOption["numberOfMessageShowedFirstTime"].minValue = 1;
    listOfDefaultIntOption["numberOfMessageShowedFirstTime"].maxValue = 40;
    listOfDefaultIntOption["stickersSize"].value = 70;
    listOfDefaultIntOption["stickersSize"].minValue = 0;
    listOfDefaultIntOption["stickersSize"].maxValue = 1000;
    listOfDefaultIntOption["timeoutInSecond"].value = 10;
    listOfDefaultIntOption["timeoutInSecond"].minValue = 1;
    listOfDefaultIntOption["timeoutInSecond"].maxValue = 90;
    listOfDefaultIntOption["textBoxSize"].value = 65;
    listOfDefaultIntOption["textBoxSize"].minValue = 10;
    listOfDefaultIntOption["textBoxSize"].maxValue = 500;
    listOfDefaultIntOption["maxNbOfQuotes"].value = 3;
    listOfDefaultIntOption["maxNbOfQuotes"].minValue = 0;
    listOfDefaultIntOption["maxNbOfQuotes"].maxValue = 15;
    listOfDefaultIntOption["nbOfMessagesSend"].value = 0;
    listOfDefaultIntOption["nbOfMessagesSend"].minValue = 0;
    listOfDefaultIntOption["nbOfMessagesSend"].maxValue = 0;
    listOfDefaultIntOption["typeOfImageRefresh"].value = 2;
    listOfDefaultIntOption["typeOfImageRefresh"].minValue = 0;
    listOfDefaultIntOption["typeOfImageRefresh"].maxValue = 2;
    listOfDefaultIntOption["noelshackImageWidth"].value = 68;
    listOfDefaultIntOption["noelshackImageWidth"].minValue = 0;
    listOfDefaultIntOption["noelshackImageWidth"].maxValue = 1000;
    listOfDefaultIntOption["noelshackImageHeight"].value = 51;
    listOfDefaultIntOption["noelshackImageHeight"].minValue = 0;
    listOfDefaultIntOption["noelshackImageHeight"].maxValue = 1000;
    listOfDefaultIntOption["numberOfErrorsBeforeWarning"].value = 5;
    listOfDefaultIntOption["numberOfErrorsBeforeWarning"].minValue = 1;
    listOfDefaultIntOption["numberOfErrorsBeforeWarning"].maxValue = 25;
    listOfDefaultIntOption["updateTopicListTime"].value = 15000;
    listOfDefaultIntOption["updateTopicListTime"].minValue = 4000;
    listOfDefaultIntOption["updateTopicListTime"].maxValue = 60000;
    listOfDefaultIntOption["typeOfEdit"].value = 2;
    listOfDefaultIntOption["typeOfEdit"].minValue = 0;
    listOfDefaultIntOption["typeOfEdit"].maxValue = 2;
    listOfDefaultIntOption["topicNameMaxSizeInTopicList"].value = 35;
    listOfDefaultIntOption["topicNameMaxSizeInTopicList"].minValue = 10;
    listOfDefaultIntOption["topicNameMaxSizeInTopicList"].maxValue = 100;
    listOfDefaultIntOption["numberOfPageToLoadForOpti"].value = 1;
    listOfDefaultIntOption["numberOfPageToLoadForOpti"].minValue = 0;
    listOfDefaultIntOption["numberOfPageToLoadForOpti"].maxValue = 1;
    listOfDefaultIntOption["numberOfMessagesForOptimizationStart"].value = 8;
    listOfDefaultIntOption["numberOfMessagesForOptimizationStart"].minValue = 1;
    listOfDefaultIntOption["numberOfMessagesForOptimizationStart"].maxValue = 15;
    listOfDefaultIntOption["avatarSize"].value = 30;
    listOfDefaultIntOption["avatarSize"].minValue = 0;
    listOfDefaultIntOption["avatarSize"].maxValue = 700;
    listOfDefaultStringOption["pseudo"] = "";
    listOfDefaultStringOption["themeUsed"] = "";

    for(int i = 0; i < 10; ++i)
    {
        listOfDefaultStringOption["favoriteLink" + QString::number(i)] = "";
        listOfDefaultStringOption["favoriteName" + QString::number(i)] = "";
    }

    listOfDefaultByteOption["windowGeometry"] = "";
}

bool settingTool::getThisBoolOption(QString optionName)
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

intSettingStruct settingTool::getThisIntOption(QString optionName)
{
    QMap<QString, intSettingStruct>::iterator iteForList = listOfDefaultIntOption.find(optionName);
    intSettingStruct tmpIntSetting;

    if(iteForList != listOfDefaultIntOption.end())
    {
        tmpIntSetting = iteForList.value();
        tmpIntSetting.value = setting->value(optionName, iteForList.value().value).toInt();
        return tmpIntSetting;
    }
    else
    {
        qDebug() << "Erreur : cette option entiere \"" + optionName + "\" n existe pas.";
        tmpIntSetting.minValue = 0;
        tmpIntSetting.maxValue = 0;
        tmpIntSetting.value = setting->value(optionName, 0).toInt();
        return tmpIntSetting;
    }
}

QString settingTool::getThisStringOption(QString optionName)
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

QByteArray settingTool::getThisByteOption(QString optionName)
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

QList<accountStruct> settingTool::getListOfAccount()
{
    QList<accountStruct> listOfAccount;
    QList<QNetworkCookie> listOfConnectCookie = createCookieListWithThisQVariantList(setting->value("listOfConnectCookie", QList<QVariant>()).toList());
    QList<QString> listOfPseudo = createStringListWithThisQVariantList(setting->value("listOfPseudo", QList<QVariant>()).toList());

    if(listOfConnectCookie.size() == listOfPseudo.size())
    {
        for(int i = 0; i < listOfPseudo.size(); ++i)
        {
            listOfAccount.push_back(accountStruct());
            listOfConnectCookie[i].setExpirationDate(QDateTime::currentDateTime().addYears(8));
            listOfAccount.back().connectCookie = listOfConnectCookie.at(i);
            listOfAccount.back().pseudo = listOfPseudo.at(i);
        }
    }

    return listOfAccount;
}

QList<QString> settingTool::getListOfPseudoForTopic()
{
    QList<QString> listOfPseudoForTopic;

    if(setting->value("listOfPseudoForTopic", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfPseudoForTopic = createStringListWithThisQVariantList(setting->value("listOfPseudoForTopic").toList());
    }

    return listOfPseudoForTopic;
}

QList<QString> settingTool::getListOfIgnoredPseudo()
{
    QList<QString> listOfIgnoredPseudo;

    if(setting->value("listOfIgnoredPseudo", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfIgnoredPseudo = createStringListWithThisQVariantList(setting->value("listOfIgnoredPseudo").toList());
    }

    return listOfIgnoredPseudo;
}

QList<pseudoWithColorStruct> settingTool::getListOfColorPseudo()
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

QList<QString> settingTool::getListOfTopicLink()
{
    QList<QString> listOfTopicLink;

    if(setting->value("listOfTopicLink", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfTopicLink = createStringListWithThisQVariantList(setting->value("listOfTopicLink").toList());
    }

    return listOfTopicLink;
}

void settingTool::saveThisOption(QString optionName, QVariant value)
{
    setting->setValue(optionName, value);
}

void settingTool::saveListOfAccount(QList<accountStruct>& newListOfAccount)
{
    QList<QNetworkCookie> listOfConnectCookie;
    QList<QString> listOfPseudo;

    for(const accountStruct& thisAccout : newListOfAccount)
    {
        listOfConnectCookie.push_back(thisAccout.connectCookie);
        listOfPseudo.push_back(thisAccout.pseudo);
    }

    setting->setValue("listOfConnectCookie", createQVariantListWithThisList(listOfConnectCookie));
    setting->setValue("listOfPseudo", createQVariantListWithThisList(listOfPseudo));
}

void settingTool::saveListOfPseudoForTopic(QList<QString>& newList)
{
    setting->setValue("listOfPseudoForTopic", createQVariantListWithThisList(newList, false));
}

void settingTool::saveListOfIgnoredPseudo(QList<QString>& newList)
{
    setting->setValue("listOfIgnoredPseudo", createQVariantListWithThisList(newList));
}

void settingTool::saveListOfColorPseudo(QList<pseudoWithColorStruct>& newListOfColorPseudo)
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

void settingTool::saveListOfTopicLink(QList<QString>& newList)
{
    setting->setValue("listOfTopicLink", createQVariantListWithThisList(newList));
}

QList<QVariant> settingTool::createQVariantListWithThisList(QList<QString> list, bool deleteEmptyString)
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

QList<QVariant> settingTool::createQVariantListWithThisList(QList<QNetworkCookie> list)
{
    QList<QVariant> newList;

    for(const QNetworkCookie& thisCookie : list)
    {
        newList.push_back(thisCookie.toRawForm());
    }

    return newList;
}

QList<QString> settingTool::createStringListWithThisQVariantList(QList<QVariant> list)
{
    QList<QString> newList;

    for(const QVariant& thisVariant : list)
    {
        newList.push_back(thisVariant.toString());
    }

    return newList;
}

QList<QNetworkCookie> settingTool::createCookieListWithThisQVariantList(QList<QVariant> list)
{
    QList<QNetworkCookie> newList;

    for(const QVariant& thisVariant : list)
    {
        newList.push_back(QNetworkCookie::parseCookies(thisVariant.toByteArray()).first());
    }

    return newList;
}

void settingTool::forceSync()
{
    setting->sync();
}
