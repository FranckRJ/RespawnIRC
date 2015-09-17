#ifndef SHOWTOPICMESSAGES_HPP
#define SHOWTOPICMESSAGES_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

#include "colorPseudoListWindow.hpp"
#include "showListOfTopic.hpp"

class showTopicMessagesClass : public QWidget
{
    Q_OBJECT
public:
    showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct> *newListOfColorPseudo, QWidget* parent = 0);
    void startGetMessage();
    const QList<QPair<QString, QString> >& getListOfInput();
    QString getTopicLink();
    QString getTopicName();
    QString getCaptchaLink();
    QString getMessagesStatus();
    QString getNumberOfConnected();
    QString getPseudoUsed();
    QString getColorOfThisPseudo(QString pseudo);
    const QList<QNetworkCookie>& getListOfCookies();
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser);
    void setMessageStatus(QString newStatus);
    void setNumberOfConnected(QString newNumber, bool forceSet = false);
    void setTopicToErrorMode();
    void updateSettingInfo();
public slots:
    void setNewTopic(QString newTopic);
    void linkClicked(const QUrl& link);
    bool getEditInfo(int idOfMessageToEdit = 0);
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
    void editThisMessage(int idOfMessageEdit);
    void newMessageStatus();
    void newNumberOfConnected();
    void setEditInfo(int idOfMessageEdit, QString messageEdit, QString infoToSend, QString cpatchaLink);
    void newMessagesAvailable();
    void newNameForTopic(QString newName);
    void openThisTopicInNewTab(QString topicLink);
private:
    QTextBrowser messagesBox;
    showListOfTopicClass showListOfTopic;
    QList<QNetworkCookie> currentCookieList;
    QNetworkReply* replyForFirstPage;
    QNetworkReply* replyForSecondPage;
    QNetworkReply* replyForEditInfo;
    QNetworkReply* replyForQuoteInfo;
    QNetworkAccessManager networkManager;
    QTimer timerForGetMessage;
    QList<QPair<QString, QString> > listOfInput;
    QList<QString>* listOfIgnoredPseudo;
    QList<pseudoWithColorStruct> *listOfColorPseudo;
    QMap<int, QString> listOfEdit;
    QString messagesStatus;
    QString numberOfConnected;
    QString topicLink;
    QString topicName;
    QString pseudoOfUser;
    QString captchaLink;
    QString ajaxInfo;
    QString oldAjaxInfo;
    QString lastMessageQuoted;
    bool showQuoteButton;
    bool showBlacklistButton;
    bool showEditButton;
    bool loadTwoLastPage;
    bool ignoreNetworkError;
    bool secondPageLoading;
    bool firstTimeGetMessages;
    bool retrievesMessage;
    bool linkHasChanged;
    bool errorMode;
    int idOfLastMessage;
    int idOfLastMessageOfUser;
    int oldIdOfLastMessageOfUser;
    int numberOfMessageShowedFirstTime;
};

#endif
