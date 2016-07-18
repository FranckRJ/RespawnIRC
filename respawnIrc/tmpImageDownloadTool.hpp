#ifndef TMPIMAGEDOWNLOADTOOL_HPP
#define TMPIMAGEDOWNLOADTOOL_HPP

#include <QtCore>
#include <QtNetwork>

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
public slots:
    void analyzeLatestImageDownloaded();
private:
    QNetworkAccessManager* networkManager;
    QNetworkReply* reply = nullptr;
    QStringList listOfImages;
    QStringList listOfImagesUrlNeedDownload;
    QTemporaryDir tmpDir;
};

#endif
