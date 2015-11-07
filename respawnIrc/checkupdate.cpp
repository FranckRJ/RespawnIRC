#include <QtWidgets>

#include "checkUpdate.hpp"
#include "parsingTool.hpp"
#include "styleTool.hpp"

checkUpdateClass::checkUpdateClass(QWidget* newParent, QString currentVersionName) : QObject(newParent)
{
    parent = newParent;
    versionName = currentVersionName;
    reply = 0;

    networkManager = new QNetworkAccessManager(this);
}

void checkUpdateClass::startDownloadOfLatestUpdatePage(bool showMessageWhenNoUpdate)
{
    if(networkManager == 0)
    {
        networkManager = new QNetworkAccessManager(this);
    }

    if(reply == 0)
    {
        /*if(networkManager->networkAccessible() != QNetworkAccessManager::Accessible) //a changer
        {
            delete networkManager;
            networkManager = 0;
            return;
        }*/

        reply = networkManager->get(parsingToolClass::buildRequestWithThisUrl("https://api.github.com/repos/LEpigeon888/RespawnIRC/releases/latest"));
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
                        " <a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"https://github.com/LEpigeon888/RespawnIRC/releases/latest\">https://github.com/LEpigeon888/RespawnIRC/releases/latest</a><br />" +
                            "Lien de téléchargement direct : <a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"https://github.com/LEpigeon888/RespawnIRC/releases/download/v" + newVersionName + "/RespawnIRC-v" + newVersionName +
                            ".zip\">https://github.com/LEpigeon888/RespawnIRC/releases/download/v" + newVersionName + "/RespawnIRC-v" + newVersionName + ".zip</a>" +
                            "<br /><br />Le changelog :<br />" + parsingToolClass::getVersionChangelog(source));
    }
    else if(alwaysShowMessage == true)
    {
        QMessageBox message;
        message.information(parent, "Pas de nouvelle version disponible", "Il n'y a pas de nouvelle version disponible.");
    }

    reply = 0;
}
