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
#include <QScopedPointer>

struct imageDownloadRuleStruct
{
    QString directoryPath;
    QString appendAfterName;
    QString baseUrl;
    bool isInTmpDir;
    bool alwaysCheckBeforeDL;
    bool takeOnlyFileNameForSave;
    bool keepAspectRatio;
    int preferedImageWidth;
    int preferedImageHeight;
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
    explicit imageDownloadToolClass(QObject* parent = nullptr);
    void addOrUpdateRule(QString ruleName, QString directoryPath, bool isInTmpDir = false, bool alwaysCheckBeforeDL = false,
                         QString baseUrl = "", QString appendAfterName = "", bool takeOnlyFileNameForSave = false,
                         int preferedImageWidth = 0, int preferedImageHeight = 0, bool keepAspectRatio = true);
    void checkAndStartDownloadMissingImages(QStringList listOfImagesUrlToCheck, QString ruleName);
    void resetCache();
    void deleteCache();
    QString getPathOfTmpDir();
    int getNumberOfDownloadRemaining();
private:
    bool checkIfImageUrlExist(QString imageUrl, const imageDownloadRuleStruct& thisRule, QString ruleName);
    void startDownloadMissingImages();
    QString convertUrlToFilePath(QString thisUrl);
    QString removeLastLevelOfFilePath(QString thisPath);
    QString getOnlyLevelOfFilePath(QString thisPath);
private slots:
    void analyzeLatestImageDownloaded();
signals:
    void oneDownloadFinished();
private:
    QNetworkAccessManager* networkManager;
    QNetworkReply* reply = nullptr;
    QMap<QString, imageDownloadRuleStruct> listOfRulesForImage;
    QMap<QString, QStringList> listOfExistingImagesForRules;
    QList<infoForDownloadImageStruct> listOfImagesUrlNeedDownload;
    QScopedPointer<QTemporaryDir> tmpDir;
    bool cacheHasBeenResetDuringDownlaod = false;
};

#endif
