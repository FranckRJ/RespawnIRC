#ifndef SHOWFORUM_HPP
#define SHOWFORUM_HPP

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
#include <QPixmap>

#include "autoTimeoutReply.hpp"
#include "styleTool.hpp"

class showForumClass : public QWidget
{
    Q_OBJECT
public:
    explicit showForumClass(QString currentThemeName, QWidget* parent = 0);
    bool getLoadNeeded();
    void setForumLink(QString newForumLink);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newWebsiteOfCookies);
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
    QString websiteOfForum;
    QListView listViewOfTopic;
    QStandardItemModel modelForListView;
    autoTimeoutReplyClass timeoutForReply;
    QNetworkReply* reply = nullptr;
    QNetworkAccessManager* networkManager;
    QList<QNetworkCookie> currentCookieList;
    QString websiteOfCookies;
    QList<QString> listOfLink;
    modelInfoStruct baseModelInfo;
    QPixmap pinnedOnTagImage;
    QPixmap pinnedOffTagImage;
    QPixmap hotTagImage;
    QPixmap lockTagImage;
    QPixmap resolvedTagImage;
    QPixmap normalTagImage;
    bool showNumberOfMessages;
    bool cutLongTopicName;
    bool colorModoAndAdminTopic;
    bool showPinnedTagOnTopic;
    bool showHotTagOnTopic;
    bool showLockTagOnTopic;
    bool showResolvedTagOnTopic;
    bool showNormalTagOnTopic;
    bool useIconInsteadOfTag;
    bool loadNeeded = true;
    int topicNameMaxSize;
};

#endif
