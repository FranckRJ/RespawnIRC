#ifndef STICKERDOWNLOADTOOL_HPP
#define STICKERDOWNLOADTOOL_HPP

#include <QtCore>
#include <QtNetwork>

class stickerDownloadToolClass : public QObject
{
    Q_OBJECT
public:
    stickerDownloadToolClass(QObject* parent = 0);
    void updateListOfStickers();
    bool checkIfStickerExist(QString stickerName);
    void checkAndStartDownloadMissingStickers(QStringList listOfStickersToCheck);
    void startDownloadMissingStickers();
    int getNumberOfDownloadRemaining();
public slots:
    void analyzeLatestStickerDownloaded();
signals:
    void oneDownloadFinished();
private:
    QNetworkAccessManager* networkManager;
    QNetworkReply* reply = nullptr;
    QStringList listOfStickers;
    QStringList listOfStickersNeedDownload;
};

#endif
