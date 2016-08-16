#ifndef IMAGEDOWNLOADTOOL_HPP
#define IMAGEDOWNLOADTOOL_HPP

#include <QObject>
#include <QTemporaryDir>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QList>
#include <QMap>

struct imageDownloadRuleStruct
{
    QString directoryPath;
    QString appendAfterName;
    QString baseUrl;
    bool isInTmpDir = false;
    bool alwaysCheckBeforeDL = false;
    bool takeOnlyFileNameForSave = false;
};

struct infoForDownloadImageStruct
{
    QString linkOfImage;
    QString ruleForImage;
};

class imageDownloadToolClass : public QObject
{
    Q_OBJECT
public:
    explicit imageDownloadToolClass(QObject* parent = 0);
    void addRule(QString ruleName, QString directoryPath, bool isInTmpDir = false, bool alwaysCheckBeforeDL = false,
                 QString baseUrl = "", QString appendAfetName = "", bool takeOnlyFileNameForSave = false);
    bool checkIfImageUrlExist(QString imageUrl, imageDownloadRuleStruct thisRule, QString ruleName);
    void checkAndStartDownloadMissingImages(QStringList listOfImagesUrlToCheck, QString ruleName);
    void startDownloadMissingImages();
    QString convertUrlToFilePath(QString thisUrl);
    QString removeLastLevelOfFilePath(QString thisPath);
    QString getOnlyLevelOfFilePath(QString thisPath);
    QString getPathOfTmpDir();
    int getNumberOfDownloadRemaining();
public slots:
    void analyzeLatestImageDownloaded();
signals:
    void oneDownloadFinished();
private:
    QNetworkAccessManager* networkManager;
    QNetworkReply* reply = nullptr;
    QMap<QString, imageDownloadRuleStruct> listOfRulesForImage;
    QMap<QString, QStringList> listOfExistingsImageForRules;
    QList<infoForDownloadImageStruct> listOfImagesUrlNeedDownload;
    QTemporaryDir tmpDir;
};

#endif
