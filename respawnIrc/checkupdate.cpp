#include <QtWidgets>

#include "checkUpdate.hpp"
#include "parsingTool.hpp"

checkUpdateClass::checkUpdateClass(QWidget* newParent, QString currentVersionName) : QObject(newParent)
{
    parent = newParent;
    versionName = currentVersionName;
    reply = 0;
}

void checkUpdateClass::startDownloadOfLatestUpdatePage(bool showMessageWhenNoUpdate)
{
    if(reply == 0)
    {
        reply = networkManager.get(parsingToolClass::buildRequestWithThisUrl("https://api.github.com/repos/LEpigeon888/RespawnIRC/releases/latest"));
        alwaysShowMessage = showMessageWhenNoUpdate;

        QObject::connect(reply, &QNetworkReply::finished, this, &checkUpdateClass::analyzeLatestUpdatePage);
    }
}

void checkUpdateClass::analyzeLatestUpdatePage()
{
    QString source = reply->readAll();
    QString newVersionName;
    reply->deleteLater();

    newVersionName = parsingToolClass::getVersionName(source);

    if(newVersionName.isEmpty() == false && versionName != newVersionName)
    {
        QMessageBox message;
        newVersionName.remove(0, 1);
        message.information(parent, "Nouvelle version disponible !", "La version " + newVersionName + " est disponible à cette adresse :" +
                        " <a href=\"https://github.com/LEpigeon888/RespawnIRC/releases/latest\">https://github.com/LEpigeon888/RespawnIRC/releases/latest</a>");
    }
    else if(alwaysShowMessage == true)
    {
        QMessageBox message;
        message.information(parent, "Pas de nouvelle version disponible", "Il n'y a pas de nouvelle version disponible.");
    }

    reply = 0;
}
