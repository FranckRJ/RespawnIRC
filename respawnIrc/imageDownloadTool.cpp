#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QIODevice>

#include "imageDownloadTool.hpp"
#include "parsingTool.hpp"

imageDownloadToolClass::imageDownloadToolClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

void imageDownloadToolClass::addRule(QString ruleName, QString directoryPath, bool isInTmpDir, bool alwaysCheckBeforeDL,
                                     QString baseUrl, QString appendAfetName, bool takeOnlyFileNameForSave)
{
    imageDownloadRuleStruct newRule;

    newRule.directoryPath = directoryPath;
    newRule.baseUrl = baseUrl;
    newRule.isInTmpDir = isInTmpDir;
    newRule.alwaysCheckBeforeDL = alwaysCheckBeforeDL;
    newRule.appendAfterName = appendAfetName;
    newRule.takeOnlyFileNameForSave = takeOnlyFileNameForSave;

    listOfRulesForImage[ruleName] = newRule;
    listOfExistingsImageForRules[ruleName] = QStringList();

    if(newRule.alwaysCheckBeforeDL == true)
    {
        QString basePath = (newRule.isInTmpDir == true ? tmpDir.path() : QCoreApplication::applicationDirPath());
        QDir imageDir(basePath + newRule.directoryPath);
        QStringList listOfImagesInDir;

        if(imageDir.exists() == true)
        {
            listOfImagesInDir = imageDir.entryList(QDir::Files);
        }

        for(QString& thisImage : listOfImagesInDir)
        {
            thisImage = newRule.baseUrl + thisImage;

            if(newRule.appendAfterName.isEmpty() == false && thisImage.endsWith(newRule.appendAfterName) == true)
            {
                thisImage.remove(thisImage.size() - newRule.appendAfterName.size(), newRule.appendAfterName.size());
            }

            thisImage = convertUrlToFilePath(thisImage);
        }

        listOfExistingsImageForRules[ruleName].append(listOfImagesInDir);
    }
}

bool imageDownloadToolClass::checkIfImageUrlExist(QString imageUrl, imageDownloadRuleStruct thisRule, QString ruleName)
{
    QMap<QString, QStringList>::iterator listOfImagesIte = listOfExistingsImageForRules.find(ruleName);

    if(listOfImagesIte == listOfExistingsImageForRules.end())
    {
        return true;
    }
    if(listOfImagesIte.value().indexOf(convertUrlToFilePath(imageUrl)) != -1)
    {
        return true;
    }

    for(const infoForDownloadImageStruct& thisImageInfo : listOfImagesUrlNeedDownload)
    {
        if(thisImageInfo.ruleForImage == ruleName && convertUrlToFilePath(thisImageInfo.linkOfImage) == convertUrlToFilePath(imageUrl))
        {
            return true;
        }
    }

    if(thisRule.alwaysCheckBeforeDL == true)
    {
        QString basePath = (thisRule.isInTmpDir == true ? tmpDir.path() : QCoreApplication::applicationDirPath());
        QString pathFile = (thisRule.takeOnlyFileNameForSave == true ? getOnlyLevelOfFilePath(imageUrl) : imageUrl);
        QFileInfo imageFile(basePath + thisRule.directoryPath + convertUrlToFilePath(pathFile) + thisRule.appendAfterName);

        if(imageFile.exists() == true && imageFile.isFile() == true)
        {
            listOfImagesIte.value().append(convertUrlToFilePath(imageUrl));
            return true;
        }
    }

    return false;
}

void imageDownloadToolClass::checkAndStartDownloadMissingImages(QStringList listOfImagesUrlToCheck, QString ruleName)
{
    QMap<QString, imageDownloadRuleStruct>::iterator ruleIte = listOfRulesForImage.find(ruleName);

    if(ruleIte == listOfRulesForImage.end())
    {
        return;
    }
    if(ruleIte.value().isInTmpDir == true && tmpDir.isValid() == false)
    {
        return;
    }

    for(QString& thisImageUrl : listOfImagesUrlToCheck)
    {
        thisImageUrl = ruleIte.value().baseUrl + thisImageUrl;

        if(checkIfImageUrlExist(thisImageUrl, ruleIte.value(), ruleName) == false)
        {
            infoForDownloadImageStruct newImageToDownload;

            newImageToDownload.linkOfImage = thisImageUrl;
            newImageToDownload.ruleForImage = ruleName;

            listOfImagesUrlNeedDownload.push_back(newImageToDownload);
        }
    }

    startDownloadMissingImages();
}

void imageDownloadToolClass::startDownloadMissingImages()
{
    if(listOfImagesUrlNeedDownload.isEmpty() == false)
    {
        if(networkManager == nullptr)
        {
            networkManager = new QNetworkAccessManager(this);
        }

        if(reply == nullptr)
        {
            reply = networkManager->get(parsingToolClass::buildRequestWithThisUrl(listOfImagesUrlNeedDownload.front().linkOfImage));

            if(reply->isOpen() == true)
            {
                connect(reply, &QNetworkReply::finished, this, &imageDownloadToolClass::analyzeLatestImageDownloaded);
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

QString imageDownloadToolClass::convertUrlToFilePath(QString thisUrl)
{
    if(thisUrl.startsWith("http://") == true || thisUrl.startsWith("https://") == true)
    {
        thisUrl.remove(0, thisUrl.indexOf("/") + 2);
    }

    return thisUrl.toLower();
}

QString imageDownloadToolClass::removeLastLevelOfFilePath(QString thisPath)
{
    return thisPath.left(thisPath.lastIndexOf("/"));
}

QString imageDownloadToolClass::getOnlyLevelOfFilePath(QString thisPath)
{
    return thisPath.right(thisPath.size() - thisPath.lastIndexOf("/") - 1);
}

QString imageDownloadToolClass::getPathOfTmpDir()
{
    return tmpDir.path();
}

int imageDownloadToolClass::getNumberOfDownloadRemaining()
{
    return listOfImagesUrlNeedDownload.size();
}

void imageDownloadToolClass::analyzeLatestImageDownloaded()
{
    QMap<QString, imageDownloadRuleStruct>::iterator ruleIte = listOfRulesForImage.find(listOfImagesUrlNeedDownload.front().ruleForImage);
    QMap<QString, QStringList>::iterator listOfImagesIte = listOfExistingsImageForRules.find(listOfImagesUrlNeedDownload.front().ruleForImage);

    if(ruleIte != listOfRulesForImage.end() && listOfImagesIte != listOfExistingsImageForRules.end())
    {
        if(reply->isReadable() == true && (ruleIte.value().isInTmpDir == false || tmpDir.isValid() == true))
        {
            QFile newImage;
            QDir newDir;
            QString basePath = (ruleIte.value().isInTmpDir == true ? tmpDir.path() : QCoreApplication::applicationDirPath());
            QString pathFile = (ruleIte.value().takeOnlyFileNameForSave == true ? getOnlyLevelOfFilePath(listOfImagesUrlNeedDownload.front().linkOfImage) : listOfImagesUrlNeedDownload.front().linkOfImage);
            QString imagePath = (basePath + ruleIte.value().directoryPath + convertUrlToFilePath(pathFile) + ruleIte.value().appendAfterName);
            newDir.mkpath(removeLastLevelOfFilePath(imagePath));
            newImage.setFileName(imagePath);
            newImage.open(QIODevice::WriteOnly);
            newImage.write(reply->readAll());
            newImage.close();
            listOfImagesIte.value().append(convertUrlToFilePath(listOfImagesUrlNeedDownload.front().linkOfImage));
        }
    }
    reply->deleteLater();

    listOfImagesUrlNeedDownload.pop_front();
    reply = nullptr;

    emit oneDownloadFinished();
    startDownloadMissingImages();
}
