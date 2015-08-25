#ifndef PARSINGTOOL_HPP
#define PARSINGTOOL_HPP

#include <QtCore>
#include <QtNetwork>

struct messageStruct
{
    int idOfMessage;
    QString pseudo;
    QString date;
    QString message;
    QString lastTimeEdit;
};

class parsingToolClass
{
public:
    static QString getVersionName(const QString& source);
    static QString getVersionChangelog(const QString& source);
    static void getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput);
    static QString getCaptchaLink(const QString& source);
    static QString getLastPageOfTopic(const QString& source);
    static QString getBeforeLastPageOfTopic(const QString& source);
    static QString getNameOfTopic(const QString& source);
    static QString getNumberOfConnected(const QString& source);
    static QList<messageStruct> getListOfEntireMessages(const QString& source);
    static QString getForumOfTopic(const QString& source);
    static QString jvfLinkToJvcLink(const QString& source);
    static QString parsingMessages(QString thisMessage);
    static QNetworkRequest buildRequestWithThisUrl(QString url);
    static QList<QString> getListOfThisCapNumber(const QString &source, QRegularExpression& exp, int capNumber, bool globalMatch = true);
    static void replaceWithCapNumber(QString& source, QRegularExpression& exp, int capNumber, bool createLink = false, QString stringBefore = "",
                                     QString stringAfter = "", int secondCapNumber = 0, QString stringAfterAfter = "");
private:
    static QRegularExpression expForVersionName;
    static QRegularExpression expForVersionChangelog;
    static QRegularExpression expForFormTopic;
    static QRegularExpression expForFormConnect;
    static QRegularExpression expForInput;
    static QRegularExpression expForCaptcha;
    static QRegularExpression expForCurrentPage;
    static QRegularExpression expForPageLink;
    static QRegularExpression expForBeforeLastPage;
    static QRegularExpression expForNameOfTopic;
    static QRegularExpression expForNumberOfConnected;
    static QRegularExpression expForEntireMessage;
    static QRegularExpression expForMessageID;
    static QRegularExpression expForPseudo;
    static QRegularExpression expForDate;
    static QRegularExpression expForMessage;
    static QRegularExpression expForEdit;
    static QRegularExpression expForForum;
    static QRegularExpression expForJvfLink;
    static QRegularExpression expForSmiley;
    static QRegularExpression expForStickers;
    static QRegularExpression expForLongLink;
    static QRegularExpression expForShortLink;
    static QRegularExpression expForNoelshack;
    static QRegularExpression expForSpoilLine;
    static QRegularExpression expForSpoilBlock;
    static QRegularExpression expForAllJVCare;
};

#endif
