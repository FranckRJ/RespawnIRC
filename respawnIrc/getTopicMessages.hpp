#ifndef GETTOPICMESSAGES_HPP
#define GETTOPICMESSAGES_HPP

#include <QObject>
#include <QNetworkCookie>
#include <QVector>
#include <QList>
#include <QMap>
#include <QString>
#include <QPair>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QStringList>

#include "autoTimeoutReply.hpp"
#include "parsingTool.hpp"

struct settingsForMessageParsingStruct
{
    infoForMessageParsingStruct infoForMessageParsing;
    bool loadTwoLastPage;
    int numberOfPagesToLoad;
    int timerTime;
    int timeoutTime;
    bool downloadMissingStickers;
    bool downloadNoelshackImages;
};

class getTopicMessagesClass : public QObject
{
    Q_OBJECT
public:
    explicit getTopicMessagesClass(QObject* parent = 0);
public slots:
    void setNewTopic(QString newTopicLink, bool getFirstMessage, QString website);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool updateMessages);
    void settingsChanged(settingsForMessageParsingStruct newSettings);
    void startGetMessage();
    void getMessages();
    void loadForThisPageFinish();
    void analyzeMessages();
signals:
    void newMessagesAreAvailable(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString> > newListOfInput,
                                 ajaxInfoStruct newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty);
    void newNameForTopic(QString newName);
    void newCookiesHaveToBeSet(QList<QNetworkCookie> newListOfCookies, QString currentPseudoOfUser);
    void newMessageStatus(QString newStatus);
    void newNumberOfConnectedAndMP(QString newNumberConnected, QString newNumberMP, bool forceSet);
    void newLinkForTopic(QString newTopicLink);
    void theseStickersAreUsed(QStringList listOfStickersUsed);
    void theseNoelshackImagesAreUsed(QStringList listOfNoelshackImagesUsed);
private:
    QNetworkAccessManager* networkManager;
    QVector<autoTimeoutReplyClass*> listOfTimeoutForReplys;
    QVector<QNetworkReply*> listOfReplys;
    QList<QNetworkCookie> currentCookieList;
    QMap<long, QString> listOfEdit;
    QTimer* timerForGetMessage;
    QString topicLink;
    QString mWebsite;
    QString pseudoOfUser;
    settingsForMessageParsingStruct settingsForMessageParsing;
    bool needToGetFirstMessage;
    bool linkHasChanged = false;
    bool firstTimeGetMessages = true;
    bool needToGetMessages = false;
    bool retrievesMessage = false;
    bool needToSetCookies = false;
    long idOfLastMessage = 0;
    int numberOfPagesToDownload;
};

#endif
