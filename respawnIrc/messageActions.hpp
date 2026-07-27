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
    bool getEditInfo(long idOfMessageToEdit, QString urlForFormValues, bool useMessageEdit);
    void deleteMessage(QString urlForDeletion);
private slots:
    void analyzeEditInfo();
    void analyzeDeleteInfo();
signals:
    void setEditInfo(long idOfMessageEdit, QString messageEdit, QString error,
                     QList<QPair<QString, QString>> listOfFieldForEdit, bool useMessageEdit);
private:
    QWidget* parent;
    QNetworkCookie currentConnectCookie;
    QString websiteOfCookie;
    QString websiteOfTopic;
    QNetworkAccessManager* networkManager;
    autoTimeoutReplyClass* timeoutForEditInfo;
    autoTimeoutReplyClass* timeoutForDeleteInfo;
    QNetworkReply* replyForEditInfo = nullptr;
    QNetworkReply* replyForDeleteInfo = nullptr;
    long oldIdOfMessageToEdit = 0;
    bool oldUseMessageEdit = false;
    ajaxInfoStruct ajaxInfo;
};

#endif
