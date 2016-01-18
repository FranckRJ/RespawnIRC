#ifndef CHECKUPDATE_HPP
#define CHECKUPDATE_HPP

#include <QtCore>
#include <QtNetwork>

class checkUpdateClass : public QObject
{
    Q_OBJECT
public:
    checkUpdateClass(QWidget* newParent, QString currentVersionName);
    void startDownloadOfLatestUpdatePage(bool showMessageWhenNoUpdate = false);
    bool itsANewerVersion(QString newVersionName);
public slots:
    void analyzeLatestUpdatePage();
private:
    QWidget* parent;
    QString versionName;
    QNetworkAccessManager* networkManager;
    QNetworkReply* reply;
    bool alwaysShowMessage;
    bool alertForSameVersion;
};

#endif
