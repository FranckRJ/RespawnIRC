#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "tmpImageDownloadTool.hpp"
#include "parsingTool.hpp"

tmpImageDownloadToolClass::tmpImageDownloadToolClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

bool tmpImageDownloadToolClass::checkIfImageUrlExist(QString imageUrl)
{
    if(listOfImages.indexOf(convertUrlToFilePath(imageUrl)) != -1)
    {
        return true;
    }

    for(QString thisImageUrl : listOfImagesUrlNeedDownload)
    {
        if(convertUrlToFilePath(thisImageUrl) == convertUrlToFilePath(imageUrl))
        {
            return true;
        }
    }

    return false;
}

void tmpImageDownloadToolClass::checkAndStartDownloadMissingImages(QStringList listOfImagesUrlToCheck)
{
    if(tmpDir.isValid() == false)
    {
        return;
    }

    for(QString thisImageUrl : listOfImagesUrlToCheck)
    {
        if(checkIfImageUrlExist(thisImageUrl) == false)
        {
            listOfImagesUrlNeedDownload.push_back(thisImageUrl);
        }
    }

    startDownloadMissingImages();
}

void tmpImageDownloadToolClass::startDownloadMissingImages()
{
    if(listOfImagesUrlNeedDownload.isEmpty() == false)
    {
        if(networkManager == nullptr)
        {
            networkManager = new QNetworkAccessManager(this);
        }

        if(reply == nullptr)
        {
            reply = networkManager->get(parsingToolClass::buildRequestWithThisUrl(listOfImagesUrlNeedDownload.front()));

            if(reply->isOpen() == true)
            {
                QObject::connect(reply, &QNetworkReply::finished, this, &tmpImageDownloadToolClass::analyzeLatestImageDownloaded);
            }
            else
            {
                analyzeLatestImageDownloaded();
                networkManager->deleteLater();
                networkManager = nullptr;
            }
        }
    }
}

QString tmpImageDownloadToolClass::convertUrlToFilePath(QString thisUrl)
{
    if(thisUrl.startsWith("http://") == true || thisUrl.startsWith("https://") == true)
    {
        thisUrl.remove(0, thisUrl.indexOf("/") + 2);
    }

    return thisUrl.toLower();
}

QString tmpImageDownloadToolClass::removeLastLevelOfFilePath(QString thisPath)
{
    return thisPath.left(thisPath.lastIndexOf("/"));
}

QString tmpImageDownloadToolClass::getPathOfTmpDir()
{
    return tmpDir.path();
}

int tmpImageDownloadToolClass::getNumberOfDownloadRemaining()
{
    return listOfImagesUrlNeedDownload.size();
}

void tmpImageDownloadToolClass::analyzeLatestImageDownloaded()
{
    if(reply->isReadable() == true && tmpDir.isValid() == true)
    {
        QFile newImage;
        QDir newDir;
        newDir.mkpath(tmpDir.path() + "/img/" + removeLastLevelOfFilePath(convertUrlToFilePath(listOfImagesUrlNeedDownload.front())));
        newImage.setFileName(tmpDir.path() + "/img/" + convertUrlToFilePath(listOfImagesUrlNeedDownload.front()));
        newImage.open(QIODevice::WriteOnly);
        newImage.write(reply->readAll());
        newImage.close();
        listOfImages.append(convertUrlToFilePath(listOfImagesUrlNeedDownload.front()));
    }
    reply->deleteLater();

    listOfImagesUrlNeedDownload.pop_front();
    reply = nullptr;

    emit oneDownloadFinished();
    startDownloadMissingImages();
}
