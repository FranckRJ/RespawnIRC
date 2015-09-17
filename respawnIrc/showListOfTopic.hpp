#ifndef SHOWLISTOFTOPIC_HPP
#define SHOWLISTOFTOPIC_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

class showListOfTopicClass : public QWidget
{
    Q_OBJECT
public:
    showListOfTopicClass(QWidget* parent = 0);
    void setForumLink(QString newForumLink);
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
    QNetworkReply* reply;
    QNetworkAccessManager networkManager;
    QList<QString> listOfLink;
};

#endif // SHOWLISTOFTOPIC_HPP

