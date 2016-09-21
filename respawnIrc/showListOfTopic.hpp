#ifndef SHOWLISTOFTOPIC_HPP
#define SHOWLISTOFTOPIC_HPP

#include <QWidget>
#include <QTimer>
#include <QString>
#include <QListView>
#include <QList>
#include <QStandardItemModel>
#include <QNetworkCookie>
#include <QPoint>
#include <QModelIndex>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "autoTimeoutReply.hpp"
#include "styleTool.hpp"

class showListOfTopicClass : public QWidget
{
    Q_OBJECT
public:
    explicit showListOfTopicClass(QString currentThemeName, QWidget* parent = 0);
    bool getLoadNeeded();
    void setForumLink(QString newForumLink);
    void setNewCookies(QList<QNetworkCookie> newCookies);
    void updateSettings();
    void setLoadNeeded(bool newVal);
public slots:
    void setNewTheme(QString newThemeName);
    void startGetListOfTopic();
    void analyzeReply();
    void clickedOnLink(QModelIndex index);
    void createContextMenu(const QPoint& thisPoint);
signals:
    void openThisTopic(QString topicLink);
    void openThisTopicInNewTab(QString topicLink);
private:
    QTimer timerForGetList;
    QString forumLink;
    QListView listViewOfTopic;
    QStandardItemModel modelForListView;
    autoTimeoutReplyClass timeoutForReply;
    QNetworkReply* reply = nullptr;
    QNetworkAccessManager* networkManager;
    QList<QNetworkCookie> currentCookieList;
    QList<QString> listOfLink;
    modelInfoStruct baseModelInfo;
    bool showNumberOfMessages;
    bool cutLongTopicName;
    bool colorModoAndAdminTopic;
    bool loadNeeded = true;
    int topicNameMaxSize;
};

#endif
