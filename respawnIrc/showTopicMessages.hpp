#ifndef SHOWTOPICMESSAGES_HPP
#define SHOWTOPICMESSAGES_HPP

#include <QWidget>
#include <QTextBrowser>
#include <QThread>
#include <QRegularExpression>
#include <QString>
#include <QNetworkCookie>
#include <QList>
#include <QMap>
#include <QPair>
#include <QUrl>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextCursor>

#include "colorPseudoListWindow.hpp"
#include "getTopicMessages.hpp"
#include "autoTimeoutReply.hpp"
#include "parsingTool.hpp"
#include "styleTool.hpp"

struct messagePositionInfoStruct
{
    QTextCursor cursor;
    int messageSize = 0;
    int realPosition;
};

class showTopicMessagesClass : public QWidget
{
    Q_OBJECT
public:
    explicit showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent = 0);
    ~showTopicMessagesClass();
    static void startThread();
    static void stopThread();
    void startGetMessage();
    const QList<QPair<QString, QString> >& getListOfInput();
    QString getTopicLinkLastPage();
    QString getTopicLinkFirstPage();
    QString getTopicName();
    QString getMessagesStatus();
    QString getNumberOfConnectedAndMP();
    QString getPseudoUsed();
    QString getColorOfThisPseudo(QString pseudo);
    const QList<QNetworkCookie>& getListOfCookies();
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool updateMessages = true);
    void setTopicToErrorMode();
    void updateSettingInfo();
    void addSearchPath(QString newSearchPath);
    void relayoutDocumentHack();
    void addMessageToTheEndOfMessagesBox(const QString& newMessage, long messageID);
    void editThisMessageOfMessagesBox(const QString& newMessage, long messageID);
public slots:
    void setNewTheme(QString newThemeName);
    void setNewTopic(QString newTopic);
    void linkClicked(const QUrl& link);
    bool getEditInfo(long idOfMessageToEdit = 0, bool useMessageEdit = true);
    void getQuoteInfo(QString idOfMessageQuoted);
    void deleteMessage(QString idOfMessageDeleted);
    void analyzeEditInfo();
    void analyzeQuoteInfo();
    void analyzeDeleteInfo();
    void analyzeMessages(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString> > newListOfInput,
                         ajaxInfoStruct newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty);
    void setMessageStatus(QString newStatus);
    void setNumberOfConnectedAndMP(QString newNumberConnected, QString newNumberMP, bool forceSet = false);
    void setTopicName(QString newTopicName);
    void setCookiesFromRequest(QList<QNetworkCookie> newListOfCookies, QString currentPseudoOfUser);
    void setUpdatedTopicLink(QString newTopicLink);
signals:
    void quoteThisMessage(QString messageToQuote);
    void addToBlacklist(QString pseudoToBlacklist);
    void editThisMessage(long idOfMessageEdit, bool useMessageEdit);
    void newMessageStatus();
    void newNumberOfConnectedAndMP();
    void setEditInfo(long idOfMessageEdit, QString messageEdit, QString infoToSend, bool useMessageEdit);
    void newMessagesAvailable();
    void newNameForTopic(QString newName);
    void newCookiesHaveToBeSet();
    void downloadTheseStickersIfNeeded(QStringList listOfStickersToCheck);
    void downloadTheseNoelshackImagesIfNeeded(QStringList listOfNoelshackImagesToCheck);
private:
    static QThread threadForGetMessages;
    QRegularExpression expForColorPseudo;
    QTextBrowser messagesBox;
    QString baseModel;
    getTopicMessagesClass* getTopicMessages;
    modelInfoStruct baseModelInfo;
    QList<QNetworkCookie> currentCookieList;
    autoTimeoutReplyClass timeoutForEditInfo;
    autoTimeoutReplyClass timeoutForQuoteInfo;
    autoTimeoutReplyClass timeoutForDeleteInfo;
    QNetworkReply* replyForEditInfo = nullptr;
    QNetworkReply* replyForQuoteInfo = nullptr;
    QNetworkReply* replyForDeleteInfo = nullptr;
    QNetworkAccessManager* networkManager;
    QList<QPair<QString, QString> > listOfInput;
    QList<QString>* listOfIgnoredPseudo;
    QList<pseudoWithColorStruct>* listOfColorPseudo;
    QMap<long, messagePositionInfoStruct> listOfInfosForEdit;
    QString messagesStatus = "Rien.";
    QString numberOfConnectedAndMP;
    QString topicLinkFirstPage;
    QString topicLinkLastPage;
    QString topicName;
    QString pseudoOfUser;
    ajaxInfoStruct ajaxInfo;
    ajaxInfoStruct oldAjaxInfo;
    QString lastMessageQuoted;
    QString lastDate;
    QString numberOfConnected;
    messageStruct firstMessageOfTopic;
    bool showQuoteButton;
    bool showBlacklistButton;
    bool showEditButton;
    bool showDeleteButton;
    bool showSignatures;
    bool ignoreNetworkError;
    bool firstTimeGetMessages = true;
    bool errorMode = false;
    bool oldUseMessageEdit = false;
    bool needToGetMessages = false;
    bool colorModoAndAdminPseudo;
    bool colorPEMT;
    bool colorUserPseudoInMessages;
    bool getFirstMessageOfTopic;
    bool warnWhenEdit;
    bool warnOnFirstTime;
    bool realRealTimeEdit;
    bool currentRealTimeEdit;
    long idOfLastMessageOfUser = 0;
    long oldIdOfLastMessageOfUser = 0;
    int currentErrorStreak = 0;
    int numberOfMessageShowedFirstTime;
    int numberOfErrorsBeforeWarning;
    int topicNameMaxSize = 45;
};

#endif
