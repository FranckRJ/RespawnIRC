#ifndef SHOWTOPICMESSAGES_HPP
#define SHOWTOPICMESSAGES_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

#include "colorPseudoListWindow.hpp"

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
    void setNewTopic(QString newTopic);
    void setMessageStatus(QString newStatus);
    void setNumberOfConnected(QString newNumber, bool forceSet = false);
    void setTopicToErrorMode();
    void updateSettingInfo();
public slots:
    void getMessages();
    void loadFirstPageFinish();
    void loadSecondPageFinish();
    void analyzeMessages();
signals:
    void newMessageStatus();
    void newNumberOfConnected();
    void newMessagesAvailable();
    void newNameForTopic(QString newName);
private:
    QTextBrowser messagesBox;
    QList<QNetworkCookie> currentCookieList;
    QNetworkReply* replyForFirstPage;
    QNetworkReply* replyForSecondPage;
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
    bool loadTwoLastPage;
    bool secondPageLoading;
    bool firstTimeGetMessages;
    bool retrievesMessage;
    bool linkHasChanged;
    bool errorMode;
    int idOfLastMessage;
    int numberOfMessageShowedFirstTime;
};

#endif
