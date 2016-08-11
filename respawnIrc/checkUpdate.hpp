#ifndef CHECKUPDATE_HPP
#define CHECKUPDATE_HPP

class QNetworkAccessManager;
class QNetworkReply;
class QWidget;

#include <QObject>
#include <QString>

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
    QNetworkReply* reply = nullptr;
    bool alwaysShowMessage = false;
    bool alertForSameVersion = false; //vrai pour les snapshots, faux pour les releases
};

#endif
