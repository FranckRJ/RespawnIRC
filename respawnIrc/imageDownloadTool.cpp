#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QIODevice>
#include <QImage>
#include <QByteArray>

#include "imageDownloadTool.hpp"
#include "parsingTool.hpp"

imageDownloadToolClass::imageDownloadToolClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
    tmpDir.reset(new QTemporaryDir());
}

void imageDownloadToolClass::addOrUpdateRule(QString ruleName, QString directoryPath, bool isInTmpDir, bool alwaysCheckBeforeDL,
                                             QString baseUrl, QString appendAfterName, bool takeOnlyFileNameForSave, int preferedImageWidth,
                                             int preferedImageHeight, bool keepAspectRatio)
{
    imageDownloadRuleStruct newRule;

    newRule.directoryPath = directoryPath;
    newRule.baseUrl = baseUrl;
    newRule.isInTmpDir = isInTmpDir;
    newRule.alwaysCheckBeforeDL = alwaysCheckBeforeDL;
    newRule.appendAfterName = appendAfterName;
    newRule.takeOnlyFileNameForSave = takeOnlyFileNameForSave;
    newRule.preferedImageWidth = preferedImageWidth;
    newRule.preferedImageHeight = preferedImageHeight;
    newRule.keepAspectRatio = keepAspectRatio;

    listOfRulesForImage[ruleName] = newRule;
    listOfExistingImagesForRules[ruleName] = QStringList();

    if(newRule.alwaysCheckBeforeDL == true)
    {
        QString basePath = (newRule.isInTmpDir == true ? tmpDir->path() : QCoreApplication::applicationDirPath());
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

        listOfExistingImagesForRules[ruleName].append(listOfImagesInDir);
    }
}

void imageDownloadToolClass::checkAndStartDownloadMissingImages(QStringList listOfImagesUrlToCheck, QString ruleName)
{
    QMap<QString, imageDownloadRuleStruct>::iterator ruleIte = listOfRulesForImage.find(ruleName);

    if(ruleIte == listOfRulesForImage.end())
    {
        return;
    }
    if(ruleIte.value().isInTmpDir == true && tmpDir->isValid() == false)
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

void imageDownloadToolClass::resetCache()
{
    tmpDir.reset(new QTemporaryDir());

    for(QMap<QString, imageDownloadRuleStruct>::const_iterator ite = listOfRulesForImage.constBegin(); ite != listOfRulesForImage.end(); ++ite)
    {
        if(ite.value().isInTmpDir == true)
        {
            QMap<QString, QStringList>::iterator listOfImagesIte = listOfExistingImagesForRules.find(ite.key());

            if(listOfImagesIte != listOfExistingImagesForRules.end())
            {
                listOfImagesIte.value().clear();
            }
        }
    }

    if(listOfImagesUrlNeedDownload.isEmpty() == false)
    {
        cacheHasBeenResetDuringDownlaod = true;

        if(listOfImagesUrlNeedDownload.size() > 1)
        {
            QList<infoForDownloadImageStruct>::iterator ite = listOfImagesUrlNeedDownload.begin();
            ++ite;
            while(ite != listOfImagesUrlNeedDownload.end())
            {
                QMap<QString, imageDownloadRuleStruct>::iterator ruleIte = listOfRulesForImage.find(ite->ruleForImage);

                if(ruleIte != listOfRulesForImage.end())
                {
                    if(ruleIte->isInTmpDir == true)
                    {
                        listOfImagesUrlNeedDownload.erase(ite++);
                        continue;
                    }
                }
                ++ite;
            }
        }
    }
}

void imageDownloadToolClass::deleteCache()
{
    tmpDir.reset();
}

QString imageDownloadToolClass::getPathOfTmpDir()
{
    return tmpDir->path();
}

int imageDownloadToolClass::getNumberOfDownloadRemaining()
{
    return listOfImagesUrlNeedDownload.size();
}

bool imageDownloadToolClass::checkIfImageUrlExist(QString imageUrl, const imageDownloadRuleStruct& thisRule, QString ruleName)
{
    QMap<QString, QStringList>::iterator listOfImagesIte = listOfExistingImagesForRules.find(ruleName);

    if(listOfImagesIte == listOfExistingImagesForRules.end())
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
        QString basePath = (thisRule.isInTmpDir == true ? tmpDir->path() : QCoreApplication::applicationDirPath());
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
            reply = networkManager->get(parsingTool::buildRequestWithThisUrl(listOfImagesUrlNeedDownload.front().linkOfImage));

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

void imageDownloadToolClass::analyzeLatestImageDownloaded()
{
    QMap<QString, imageDownloadRuleStruct>::iterator ruleIte = listOfRulesForImage.find(listOfImagesUrlNeedDownload.front().ruleForImage);
    QMap<QString, QStringList>::iterator listOfImagesIte = listOfExistingImagesForRules.find(listOfImagesUrlNeedDownload.front().ruleForImage);

    if(ruleIte != listOfRulesForImage.end() && listOfImagesIte != listOfExistingImagesForRules.end())
    {
        if(reply->isReadable() == true && (ruleIte.value().isInTmpDir == false || (tmpDir->isValid() == true && cacheHasBeenResetDuringDownlaod == false)))
        {
            QImage image;
            QByteArray imageInBytes = reply->readAll();
            bool imageIsValide = image.loadFromData(imageInBytes);

            if(imageIsValide == true)
            {
                QFile newImageFile;
                QDir newDir;
                QString basePath = (ruleIte.value().isInTmpDir == true ? tmpDir->path() : QCoreApplication::applicationDirPath());
                QString pathFile = (ruleIte.value().takeOnlyFileNameForSave == true ? getOnlyLevelOfFilePath(listOfImagesUrlNeedDownload.front().linkOfImage) : listOfImagesUrlNeedDownload.front().linkOfImage);
                QString imagePath = (basePath + ruleIte.value().directoryPath + convertUrlToFilePath(pathFile) + ruleIte.value().appendAfterName);
                newDir.mkpath(removeLastLevelOfFilePath(imagePath));
                newImageFile.setFileName(imagePath);
                newImageFile.open(QIODevice::WriteOnly);

                if(ruleIte.value().preferedImageWidth > 0 && ruleIte.value().preferedImageHeight > 0)
                {
                    image = image.scaled(ruleIte.value().preferedImageWidth, ruleIte.value().preferedImageHeight,
                                         ((ruleIte.value().keepAspectRatio == true) ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio), Qt::SmoothTransformation);
                    image.save(&newImageFile, 0, 100);
                }
                else
                {
                    newImageFile.write(imageInBytes);
                }

                newImageFile.close();
                listOfImagesIte.value().append(convertUrlToFilePath(listOfImagesUrlNeedDownload.front().linkOfImage));
            }
        }
    }
    reply->deleteLater();

    listOfImagesUrlNeedDownload.pop_front();
    cacheHasBeenResetDuringDownlaod = false;
    reply = nullptr;

    emit oneDownloadFinished();
    startDownloadMissingImages();
}
