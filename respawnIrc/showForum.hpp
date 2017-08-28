#ifndef SHOWFORUM_HPP
#define SHOWFORUM_HPP

#include <QWidget>
#include <QTimer>
#include <QString>
#include <QListView>
#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QNetworkCookie>
#include <QPoint>
#include <QModelIndex>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "autoTimeoutReply.hpp"
#include "styleTool.hpp"
#include "parsingTool.hpp"

class showForumClass : public QWidget
{
    Q_OBJECT
public:
    explicit showForumClass(QString currentThemeName, QWidget* parent = nullptr);
    bool getLoadNeeded() const;
    void setForumLink(QString newForumLink);
    void setNewCookie(QNetworkCookie newConnectCookie, QString newWebsiteOfCookie);
    void setNewTheme(QString newThemeName);
    void updateSettings();
private:
    QStandardItem* createItemDependingOnTopic(const topicStruct& thisTopic);
    void setLoadNeeded(bool newVal);
private slots:
    void startGetListOfTopic();
    void analyzeReply();
    void clickedOnLink(QModelIndex index);
    void createContextMenu(const QPoint& thisPoint);
signals:
    void openThisTopic(QString topicLink);
    void openThisTopicInNewTab(QString topicLink);
private:
    QTimer* timerForGetList;
    QString forumLink;
    QString websiteOfForum;
    QListView* listViewOfTopic;
    QStandardItemModel* modelForListView;
    autoTimeoutReplyClass* timeoutForReply;
    QNetworkReply* reply = nullptr;
    QNetworkAccessManager* networkManager;
    QNetworkCookie currentConnectCookie;
    QString websiteOfCookie;
    QList<QString> listOfLink;
    modelInfoStruct baseModelInfo;
    bool showNumberOfMessages;
    bool cutLongTopicName;
    bool colorModoAndAdminTopic;
    bool showPinnedTagOnTopic;
    bool showHotTagOnTopic;
    bool showLockTagOnTopic;
    bool showResolvedTagOnTopic;
    bool showGhostTagOnTopic;
    bool showNormalTagOnTopic;
    bool useIconInsteadOfTag;
    bool useInternalNavigatorForLinks;
    bool loadNeeded = true;
    int topicNameMaxSize;
};

#endif
