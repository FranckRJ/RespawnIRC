#ifndef MESSAGEACTIONS_HPP
#define MESSAGEACTIONS_HPP

#include <QObject>
#include <QWidget>
#include <QString>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QNetworkAccessManager>

#include "autoTimeoutReply.hpp"
#include "parsingTool.hpp"

class messageActionsClass : public QObject
{
    Q_OBJECT
public:
    explicit messageActionsClass(QWidget* newParent);
    void updateSettingInfo();
    void setNewTopic(QString newTopicLink);
    void setNewAjaxInfo(ajaxInfoStruct newAjaxInfo);
    void setNewCookie(QNetworkCookie newConnectCookie, QString newWebsiteOfCookie);
    const QNetworkCookie& getConnectCookie() const;
    void deleteMessage(QString idOfMessageDeleted);
private slots:
    void analyzeDeleteInfo();
private:
    QWidget* parent;
    QNetworkCookie currentConnectCookie;
    QString websiteOfCookie;
    QString websiteOfTopic;
    QNetworkAccessManager* networkManager;
    autoTimeoutReplyClass* timeoutForDeleteInfo;
    QNetworkReply* replyForDeleteInfo = nullptr;
    ajaxInfoStruct ajaxInfo;
};

#endif
