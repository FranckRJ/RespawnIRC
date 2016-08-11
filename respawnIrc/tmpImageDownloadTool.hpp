#ifndef TMPIMAGEDOWNLOADTOOL_HPP
#define TMPIMAGEDOWNLOADTOOL_HPP

#include <QObject>
#include <QTemporaryDir>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class tmpImageDownloadToolClass : public QObject
{
    Q_OBJECT
public:
    tmpImageDownloadToolClass(QObject* parent = 0);
    bool checkIfImageUrlExist(QString imageUrl);
    void checkAndStartDownloadMissingImages(QStringList listOfImagesUrlToCheck);
    void startDownloadMissingImages();
    QString convertUrlToFilePath(QString thisUrl);
    QString removeLastLevelOfFilePath(QString thisPath);
    QString getPathOfTmpDir();
    int getNumberOfDownloadRemaining();
public slots:
    void analyzeLatestImageDownloaded();
signals:
    void oneDownloadFinished();
private:
    QNetworkAccessManager* networkManager;
    QNetworkReply* reply = nullptr;
    QStringList listOfImages;
    QStringList listOfImagesUrlNeedDownload;
    QTemporaryDir tmpDir;
};

#endif
