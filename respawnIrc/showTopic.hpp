#ifndef SHOWTOPIC_HPP
#define SHOWTOPIC_HPP

#include <QWidget>
#include <QTextBrowser>
#include <QThread>
#include <QRegularExpression>
#include <QString>
#include <QNetworkCookie>
#include <QList>
#include <QPair>
#include <QUrl>
#include <QStringList>
#include <QNetworkAccessManager>

#include "colorPseudoListWindow.hpp"
#include "getTopicMessages.hpp"
#include "messageActions.hpp"
#include "parsingTool.hpp"
#include "styleTool.hpp"

struct messageInfoForEditStruct
{
    QString messageContent;
    int messageSize = 0;
    int realPosition = 0;
};

class showTopicClass : public QWidget
{
    Q_OBJECT
public:
    explicit showTopicClass(const QList<QString>* newListOfIgnoredPseudo, const QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent = nullptr);
    ~showTopicClass();
    static void createThread(QObject* parentForThread);
    static void startThread();
    static void stopThread();
    void startGetMessage();
    const QList<QPair<QString, QString>>& getListOfInput() const;
    QString getTopicLinkLastPage() const;
    QString getTopicLinkFirstPage() const;
    QString getTopicName() const;
    QString getMessagesStatus() const;
    QString getNumberOfConnectedAndMP() const;
    QString getPseudoUsed() const;
    const QList<QNetworkCookie>& getListOfCookies() const;
    bool getEditInfo(long idOfMessageToEdit = 0, bool useMessageEdit = true);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newWebsiteOfCookies, QString newPseudoOfUser, bool updateMessages = true);
    void setNewTheme(QString newThemeName);
    void setNewTopic(QString newTopic);
    void updateSettingInfo();
    void addSearchPath(QString newSearchPath);
    void relayoutDocumentHack();
private:
    void addMessageToTheEndOfMessagesBox(const QString& newMessage, long messageID);
    void editThisMessageOfMessagesBox(QString newMessage, long messageID);
    QString getColorOfThisPseudo(QString pseudo);
    void setTopicToErrorMode();
private slots:
    void linkClicked(const QUrl& link);
    void analyzeMessages(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString>> newListOfInput,
                         ajaxInfoStruct newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty);
    void setMessageStatus(QString newStatus);
    void setNumberOfConnectedAndMP(QString newNumberConnected, int newNumberMP, bool forceSet = false);
    void setTopicName(QString newTopicName);
    void setCookiesFromRequest(QList<QNetworkCookie> newListOfCookies, QString currentPseudoOfUser);
    void setUpdatedTopicLink(QString newTopicLink);
signals:
    void quoteThisMessage(QString messageToQuote);
    void addToBlacklist(QString pseudoToBlacklist);
    void editThisMessage(long idOfMessageEdit, bool useMessageEdit);
    void newMessageStatus();
    void newNumberOfConnectedAndMP();
    void newMPAreAvailables(int newNumber, QString withThisPseudo);
    void setEditInfo(long idOfMessageEdit, QString messageEdit, QString infoToSend, bool useMessageEdit);
    void newMessagesAvailable();
    void newNameForTopic(QString newName);
    void newCookiesHaveToBeSet();
    void downloadTheseStickersIfNeeded(QStringList listOfStickersToCheck);
    void downloadTheseNoelshackImagesIfNeeded(QStringList listOfNoelshackImagesToCheck);
    void downloadTheseAvatarsIfNeeded(QStringList listOfAvatarsUsed);
private:
    static QThread* threadForGetMessages;
    QRegularExpression expForColorPseudo;
    QTextBrowser* messagesBox;
    QString baseModel;
    getTopicMessagesClass* getTopicMessages;
    messageActionsClass* messageActions;
    modelInfoStruct baseModelInfo;
    QString websiteOfCookies;
    QList<QPair<QString, QString>> listOfInput;
    const QList<QString>* listOfIgnoredPseudo;
    const QList<pseudoWithColorStruct>* listOfColorPseudo;
    QList<QPair<long, messageInfoForEditStruct>> listOfInfosForEdit;
    QString messagesStatus = "Rien.";
    QString numberOfConnectedAndMP;
    QString topicLinkFirstPage;
    QString topicLinkLastPage;
    QString topicName;
    QString pseudoOfUser;
    QString lastDate;
    QString numberOfConnected;
    messageStruct firstMessageOfTopic;
    bool showQuoteButton;
    bool disableSelfQuoteButton;
    bool showBlacklistButton;
    bool showEditButton;
    bool showDeleteButton;
    bool showSignatures;
    bool showAvatars;
    bool ignoreNetworkError;
    bool firstTimeGetMessages = true;
    bool errorMode = false;
    bool needToGetMessages = false;
    bool colorModoAndAdminPseudo;
    bool colorPEMT;
    bool colorUserPseudoInMessages;
    bool getFirstMessageOfTopic;
    bool warnWhenEdit;
    bool warnOnFirstTime;
    long idOfLastMessageOfUser = 0;
    int realTypeOfEdit;
    int currentTypeOfEdit;
    int currentErrorStreak = 0;
    int numberOfMessageShowedFirstTime;
    int numberOfErrorsBeforeWarning;
    int topicNameMaxSize = 45;
};

#endif
