#include <QtWidgets>

#include "checkUpdate.hpp"
#include "parsingTool.hpp"
#include "styleTool.hpp"

checkUpdateClass::checkUpdateClass(QWidget* newParent, QString currentVersionName) : QObject(newParent)
{
    parent = newParent;
    versionName = currentVersionName;
    alwaysShowMessage = false;
    reply = 0;
    alertForSameVersion = true; //vrai pour les snapshots, faux pour les releases

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
        reply = networkManager->get(parsingToolClass::buildRequestWithThisUrl("https://api.github.com/repos/LEpigeon888/RespawnIRC/releases/latest"));
        alwaysShowMessage = showMessageWhenNoUpdate;

        if(reply->isOpen() == true)
        {
            QObject::connect(reply, &QNetworkReply::finished, this, &checkUpdateClass::analyzeLatestUpdatePage);
        }
        else
        {
            analyzeLatestUpdatePage();
            networkManager->deleteLater();
            networkManager = 0;
        }
    }
}

bool checkUpdateClass::itsANewerVersion(QString newVersionName)
{
    QStringList currentVersionNumbers = versionName.right(versionName.size() - 1).split(".", QString::SkipEmptyParts);
    QStringList newVersionNumbers = newVersionName.right(newVersionName.size() - 1).split(".", QString::SkipEmptyParts);
    bool versionAreEquals = true;

    while(currentVersionNumbers.size() != newVersionNumbers.size())
    {
        if(currentVersionNumbers.size() < newVersionNumbers.size())
        {
            currentVersionNumbers.append("0");
        }
        else
        {
            newVersionNumbers.append("0");
        }
    }

    for(int i = 0; i < currentVersionNumbers.size(); ++i)
    {
        if(newVersionNumbers.at(i).toInt() > currentVersionNumbers.at(i).toInt())
        {
            if(versionAreEquals)
            {
                return true;
            }
        }
        else if(newVersionNumbers.at(i).toInt() != currentVersionNumbers.at(i).toInt())
        {
            versionAreEquals = false;
        }
    }

    if(versionAreEquals)
    {
        return alertForSameVersion;
    }
    else
    {
        return false;
    }
}

void checkUpdateClass::analyzeLatestUpdatePage()
{
    QString source;
    QString newVersionName;

    if(reply->isReadable() == true)
    {
        source = reply->readAll();
    }
    reply->deleteLater();

    if(source.isEmpty() == false)
    {
        newVersionName = parsingToolClass::getVersionName(source);

        if(newVersionName.isEmpty() == false && itsANewerVersion(newVersionName) == true)
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
    }
    else if(alwaysShowMessage == true)
    {
        QMessageBox message;
        message.warning(parent, "Erreur", "Impossible de récupérer les informations de la dernière mise à jour.");
    }

    reply = 0;
}
