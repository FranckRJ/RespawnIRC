#ifndef SHOWTOPICMESSAGES_HPP
#define SHOWTOPICMESSAGES_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

#include "colorPseudoListWindow.hpp"
#include "autoTimeoutReply.hpp"
#include "parsingTool.hpp"
#include "styleTool.hpp"

class showTopicMessagesClass : public QWidget
{
    Q_OBJECT
public:
    showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct> *newListOfColorPseudo, QString currentThemeName, QWidget* parent = 0);
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
    void setMessageStatus(QString newStatus);
    void setNumberOfConnectedAndMP(QString newNumber, bool forceSet = false);
    void setTopicToErrorMode();
    void updateSettingInfo();
public slots:
    void setNewTheme(QString newThemeName);
    void setNewTopic(QString newTopic);
    void linkClicked(const QUrl& link);
    bool getEditInfo(int idOfMessageToEdit = 0, bool useMessageEdit = true);
    void getQuoteInfo(QString idOfMessageQuoted);
    void getMessages();
    void loadFirstPageFinish();
    void loadSecondPageFinish();
    void analyzeEditInfo();
    void analyzeQuoteInfo();
    void analyzeMessages();
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
    QTextBrowser messagesBox;
    QString baseModel;
    modelInfoStruct baseModelInfo;
    QList<QNetworkCookie> currentCookieList;
    autoTimeoutReplyClass timeoutForFirstPage;
    autoTimeoutReplyClass timeoutForSecondPage;
    autoTimeoutReplyClass timeoutForEditInfo;
    autoTimeoutReplyClass timeoutForQuoteInfo;
    QNetworkReply* replyForFirstPage = 0;
    QNetworkReply* replyForSecondPage = 0;
    QNetworkReply* replyForEditInfo = 0;
    QNetworkReply* replyForQuoteInfo = 0;
    QNetworkAccessManager* networkManager;
    QTimer timerForGetMessage;
    QList<QPair<QString, QString> > listOfInput;
    QList<QString>* listOfIgnoredPseudo;
    QList<pseudoWithColorStruct> *listOfColorPseudo;
    QMap<int, QString> listOfEdit;
    QString messagesStatus = "Rien.";
    QString numberOfConnectedAndMP;
    QString topicLink;
    QString topicName;
    QString pseudoOfUser;
    QString ajaxInfo;
    QString oldAjaxInfo;
    QString lastMessageQuoted;
    QString lastDate;
    bool showQuoteButton;
    bool showBlacklistButton;
    bool showEditButton;
    bool showStickers;
    bool loadTwoLastPage;
    bool ignoreNetworkError;
    bool secondPageLoading = false;
    bool firstTimeGetMessages = true;
    bool retrievesMessage = false;
    bool linkHasChanged = false;
    bool errorMode = false;
    bool oldUseMessageEdit = false;
    bool needToGetMessages = false;
    bool errorLastTime = false;
    bool needToSetCookies = false;
    bool colorModoAndAdminPseudo;
    bool colorPEMT;
    bool getFirstMessageOfTopic;
    bool warnWhenEdit;
    messageStruct firstMessageOfTopic;
    int idOfLastMessage = 0;
    int idOfLastMessageOfUser = 0;
    int oldIdOfLastMessageOfUser = 0;
    int numberOfMessageShowedFirstTime;
    int stickersSize;
};

#endif
