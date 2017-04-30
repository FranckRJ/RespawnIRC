#ifndef CHECKUPDATE_HPP
#define CHECKUPDATE_HPP

#include <QWidget>
#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class checkUpdateClass : public QObject
{
    Q_OBJECT
public:
    explicit checkUpdateClass(QWidget* newParent, QString currentVersionName);
    void startDownloadOfLatestUpdatePage(bool showMessageWhenNoUpdate = false);
private:
    bool itsANewerVersion(QString newVersionName) const;
private slots:
    void analyzeLatestUpdatePage();
private:
    QWidget* parent;
    QString versionName;
    QNetworkAccessManager* networkManager;
    QNetworkReply* reply = nullptr;
    bool alwaysShowMessage = false;
    bool alertForSameVersion = true; //vrai pour les snapshots, faux pour les releases
};

#endif
