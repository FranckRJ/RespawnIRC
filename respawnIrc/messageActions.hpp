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
    bool getEditInfo(long idOfMessageToEdit = 0, bool useMessageEdit = true);
    void getQuoteInfo(QString idOfMessageQuoted, QString messageQuoted);
    void deleteMessage(QString idOfMessageDeleted);
private slots:
    void analyzeEditInfo();
    void analyzeQuoteInfo();
    void analyzeDeleteInfo();
signals:
    void quoteThisMessage(QString messageToQuote);
    void setEditInfo(long idOfMessageEdit, QString messageEdit, QString error, QString infoToSend, bool useMessageEdit);
private:
    QWidget* parent;
    QNetworkCookie currentConnectCookie;
    QString websiteOfCookie;
    QString websiteOfTopic;
    QNetworkAccessManager* networkManager;
    autoTimeoutReplyClass* timeoutForEditInfo;
    autoTimeoutReplyClass* timeoutForQuoteInfo;
    autoTimeoutReplyClass* timeoutForDeleteInfo;
    QNetworkReply* replyForEditInfo = nullptr;
    QNetworkReply* replyForQuoteInfo = nullptr;
    QNetworkReply* replyForDeleteInfo = nullptr;
    ajaxInfoStruct ajaxInfo;
    ajaxInfoStruct oldAjaxInfo;
    QString lastMessageQuoted;
    long oldIdOfMessageToEdit = 0;
    bool oldUseMessageEdit = false;
};

#endif
