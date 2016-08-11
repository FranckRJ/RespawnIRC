#ifndef GETTOPICMESSAGES_HPP
#define GETTOPICMESSAGES_HPP

#include <QObject>
#include <QNetworkCookie>
#include <QList>
#include <QMap>
#include <QString>
#include <QPair>

#include "autoTimeoutReply.hpp"
#include "parsingTool.hpp"

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QStringList;

class getTopicMessagesClass : public QObject
{
    Q_OBJECT
public:
    getTopicMessagesClass(QObject* parent = 0);
public slots:
    void setNewTopic(QString newTopicLink, bool getFirstMessage);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool updateMessages);
    void settingsChanged(bool getTwoLastPages, int timerTime, bool newShowStickers, int newStickerSize,
                         int timeoutTime, int newMaxNbOfQuotes, bool newStickersToSmiley, bool newBetterQuote, bool newDownloadMissingStickers, bool newDownloadNoelshackImages);
    void otherSettingsChanged(int newNoelshackImageWidth, int newNoelshackImageHeight);
    void startGetMessage();
    void getMessages();
    void loadFirstPageFinish();
    void loadSecondPageFinish();
    void analyzeMessages();
signals:
    void newMessagesAreAvailable(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString> > newListOfInput,
                                 ajaxInfoStruct newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty);
    void newNameForTopic(QString newName);
    void newCookiesHaveToBeSet(QList<QNetworkCookie> newListOfCookies, QString currentPseudoOfUser);
    void newMessageStatus(QString newStatus);
    void newNumberOfConnectedAndMP(QString newNumberConnected, QString newNumberMP, bool forceSet);
    void theseStickersAreUsed(QStringList listOfStickersUsed);
    void theseNoelshackImagesAreUsed(QStringList listOfNoelshackImagesUsed);
private:
    QNetworkAccessManager* networkManager;
    autoTimeoutReplyClass* timeoutForFirstPage;
    autoTimeoutReplyClass* timeoutForSecondPage;
    QNetworkReply* replyForFirstPage = nullptr;
    QNetworkReply* replyForSecondPage = nullptr;
    QList<QNetworkCookie> currentCookieList;
    QMap<int, QString> listOfEdit;
    QTimer* timerForGetMessage;
    QString topicLink;
    QString pseudoOfUser;
    bool needToGetFirstMessage;
    bool linkHasChanged = false;
    bool firstTimeGetMessages = true;
    bool needToGetMessages = false;
    bool retrievesMessage = false;
    bool needToSetCookies = false;
    bool loadTwoLastPage;
    bool secondPageLoading = false;
    bool showStickers;
    bool stickerToSmiley;
    bool betterQuote;
    bool downloadMissingStickers;
    bool downloadNoelshackImages;
    int idOfLastMessage = 0;
    int stickersSize;
    int maxNbOfQuotes;
    int noelshackImageWidth;
    int noelshackImageHeight;
};

#endif
