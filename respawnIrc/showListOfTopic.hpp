#ifndef SHOWLISTOFTOPIC_HPP
#define SHOWLISTOFTOPIC_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

#include "autoTimeoutReply.hpp"

class showListOfTopicClass : public QWidget
{
    Q_OBJECT
public:
    showListOfTopicClass(QWidget* parent = 0);
    void setForumLink(QString newForumLink);
    void setNewCookies(QList<QNetworkCookie> newCookies);
    void updateSettings();
public slots:
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
    QStringListModel modelForListView;
    autoTimeoutReplyClass timeoutForReply;
    QNetworkReply* reply;
    QNetworkAccessManager* networkManager;
    QList<QNetworkCookie> currentCookieList;
    QList<QString> listOfLink;
};

#endif // SHOWLISTOFTOPIC_HPP

