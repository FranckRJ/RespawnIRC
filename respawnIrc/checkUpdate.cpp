#include <QMessageBox>
#include <QStringList>

#include "checkUpdate.hpp"
#include "parsingTool.hpp"
#include "styleTool.hpp"

checkUpdateClass::checkUpdateClass(QWidget* newParent, QString currentVersionName) : QObject(newParent)
{
    parent = newParent;
    versionName = currentVersionName;

    networkManager = new QNetworkAccessManager(this);
}

void checkUpdateClass::startDownloadOfLatestUpdatePage(bool showMessageWhenNoUpdate)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
    }

    if(reply == nullptr)
    {
        reply = networkManager->get(parsingToolClass::buildRequestWithThisUrl("https://api.github.com/repos/FranckRJ/RespawnIRC/releases/latest"));
        alwaysShowMessage = showMessageWhenNoUpdate;

        if(reply->isOpen() == true)
        {
            connect(reply, &QNetworkReply::finished, this, &checkUpdateClass::analyzeLatestUpdatePage);
        }
        else
        {
            analyzeLatestUpdatePage();
            networkManager->deleteLater();
            networkManager = nullptr;
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
            newVersionName.remove(0, 1);
            QMessageBox::information(parent, "Nouvelle version disponible !", "La version " + newVersionName + " est disponible à cette adresse :" +
                            " <a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"https://github.com/FranckRJ/RespawnIRC/releases/latest\">https://github.com/FranckRJ/RespawnIRC/releases/latest</a><br />" +
                                "Lien de téléchargement direct : <a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"https://github.com/FranckRJ/RespawnIRC/releases/download/v" + newVersionName + "/RespawnIRC-v" + newVersionName +
                                ".zip\">https://github.com/FranckRJ/RespawnIRC/releases/download/v" + newVersionName + "/RespawnIRC-v" + newVersionName + ".zip</a>" +
                                "<br /><br />Le changelog :<br />" + parsingToolClass::getVersionChangelog(source));
        }
        else if(alwaysShowMessage == true)
        {
            QMessageBox::information(parent, "Pas de nouvelle version disponible", "Il n'y a pas de nouvelle version disponible.");
        }
    }
    else if(alwaysShowMessage == true)
    {
        QMessageBox::warning(parent, "Erreur", "Impossible de récupérer les informations de la dernière mise à jour.");
    }

    reply = nullptr;
}
