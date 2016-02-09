#ifndef SHOWTOPICMESSAGES_HPP
#define SHOWTOPICMESSAGES_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

#include "colorPseudoListWindow.hpp"
#include "getTopicMessages.hpp"
#include "autoTimeoutReply.hpp"
#include "parsingTool.hpp"
#include "styleTool.hpp"

class showTopicMessagesClass : public QWidget
{
    Q_OBJECT
public:
    showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct> *newListOfColorPseudo, QString currentThemeName, QWidget* parent = 0);
    ~showTopicMessagesClass();
    void startGetMessage();
    const QList<QPair<QString, QString> >& getListOfInput();
    QString getTopicLink();
    QString getTopicName();
    QString getMessagesStatus();
    QString getNumberOfConnectedAndMP();
    QString getPseudoUsed();
    QString getColorOfThisPseudo(QString pseudo);
    const QList<QNetworkCookie>& getListOfCookies();
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool updateMessages = true);
    void setTopicToErrorMode();
    void updateSettingInfo();
public slots:
    void setNewTheme(QString newThemeName);
    void setNewTopic(QString newTopic);
    void linkClicked(const QUrl& link);
    bool getEditInfo(int idOfMessageToEdit = 0, bool useMessageEdit = true);
    void getQuoteInfo(QString idOfMessageQuoted);
    void analyzeEditInfo();
    void analyzeQuoteInfo();
    void analyzeMessages(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString> > newListOfInput,
                         QString newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty);
    void setMessageStatus(QString newStatus);
    void setNumberOfConnectedAndMP(QString newNumberConnected, QString newNumberMP, bool forceSet = false);
    void setTopicName(QString newTopicName);
    void setCookiesFromRequest(QList<QNetworkCookie> newListOfCookies, QString currentPseudoOfUser);
signals:
    void quoteThisMessage(QString messageToQuote);
    void addToBlacklist(QString pseudoToBlacklist);
    void editThisMessage(int idOfMessageEdit, bool useMessageEdit);
    void newMessageStatus();
    void newNumberOfConnectedAndMP();
    void setEditInfo(int idOfMessageEdit, QString messageEdit, QString infoToSend, bool useMessageEdit);
    void newMessagesAvailable();
    void newNameForTopic(QString newName);
    void newCookiesHaveToBeSet();
private:
    QRegularExpression expForColorPseudo;
    QTextBrowser messagesBox;
    QString baseModel;
    QThread threadForGetMessages;
    getTopicMessagesClass* getTopicMessages;
    modelInfoStruct baseModelInfo;
    QList<QNetworkCookie> currentCookieList;
    autoTimeoutReplyClass timeoutForEditInfo;
    autoTimeoutReplyClass timeoutForQuoteInfo;
    QNetworkReply* replyForEditInfo = nullptr;
    QNetworkReply* replyForQuoteInfo = nullptr;
    QNetworkAccessManager* networkManager;
    QList<QPair<QString, QString> > listOfInput;
    QList<QString>* listOfIgnoredPseudo;
    QList<pseudoWithColorStruct> *listOfColorPseudo;
    QString messagesStatus = "Rien.";
    QString numberOfConnectedAndMP;
    QString topicLink;
    QString topicName;
    QString pseudoOfUser;
    QString ajaxInfo;
    QString oldAjaxInfo;
    QString lastMessageQuoted;
    QString lastDate;
    QString numberOfConnected;
    messageStruct firstMessageOfTopic;
    bool showQuoteButton;
    bool showBlacklistButton;
    bool showEditButton;
    bool ignoreNetworkError;
    bool firstTimeGetMessages = true;
    bool errorMode = false;
    bool oldUseMessageEdit = false;
    bool needToGetMessages = false;
    bool errorLastTime = false;
    bool colorModoAndAdminPseudo;
    bool colorPEMT;
    bool colorUserPseudoInMessages;
    bool getFirstMessageOfTopic;
    bool warnWhenEdit;
    int idOfLastMessageOfUser = 0;
    int oldIdOfLastMessageOfUser = 0;
    int numberOfMessageShowedFirstTime;
};

#endif
