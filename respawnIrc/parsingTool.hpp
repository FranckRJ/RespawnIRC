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

struct topicStruct
{
    QString name;
    QString link;
};

class parsingToolClass
{
public:
    static QString getAjaxInfo(const QString& source);
    static QString getMessageEdit(QString& source);
    static QString getMessageQuote(QString& source);
    static QString getVersionName(const QString& source);
    static QString getVersionChangelog(const QString& source);
    static void getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput);
    static QString getCaptchaLink(const QString& source);
    static QString getErrorMessage(const QString& source);
    static QString getLastPageOfTopic(const QString& source);
    static QString getBeforeLastPageOfTopic(const QString& source);
    static QString getNameOfTopic(const QString& source);
    static QString getNumberOfConnected(const QString& source);
    static QString getNumberOfMp(const QString& source);
    static QList<messageStruct> getListOfEntireMessages(const QString& source, bool showStickers = true, int stickersSize = 70);
    static QList<topicStruct> getListOfTopic(const QString& source);
    static QString getForumOfTopic(const QString& source);
    static QString jvfLinkToJvcLink(const QString& source);
    static QString parsingMessages(QString thisMessage, bool showStickers, int stickersSize);
    static QString parsingAjaxMessages(QString thisMessage);
    static QNetworkRequest buildRequestWithThisUrl(QString url);
    static QList<QString> getListOfThisCapNumber(const QString &source, QRegularExpression& exp, int capNumber, bool globalMatch = true);
    static void replaceWithCapNumber(QString& source, QRegularExpression& exp, int capNumber, QString stringBefore = "",
                                     QString stringAfter = "", int secondCapNumber = -1, QString stringAfterAfter = "", bool replaceReturnByBr = false);
private:
    static QRegularExpression expForNormalLink;
    static QRegularExpression expForAjaxTimestamp;
    static QRegularExpression expForAjaxHash;
    static QRegularExpression expForMessageEdit;
    static QRegularExpression expForMessageQuote;
    static QRegularExpression expForVersionName;
    static QRegularExpression expForVersionChangelog;
    static QRegularExpression expForFormTopic;
    static QRegularExpression expForFormConnect;
    static QRegularExpression expForInput;
    static QRegularExpression expForCaptcha;
    static QRegularExpression expForError;
    static QRegularExpression expForCurrentPage;
    static QRegularExpression expForPageLink;
    static QRegularExpression expForBeforeLastPage;
    static QRegularExpression expForNameOfTopic;
    static QRegularExpression expForNumberOfConnected;
    static QRegularExpression expForMpJvc;
    static QRegularExpression expForEntireMessage;
    static QRegularExpression expForListOfTopic;
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
    static QRegularExpression expForJvcLink;
    static QRegularExpression expForNoelshack;
    static QRegularExpression expForSpoilLine;
    static QRegularExpression expForSpoilBlock;
    static QRegularExpression expForCodeLine;
    static QRegularExpression expForCodeBlock;
    static QRegularExpression expForAllJVCare;
    static QRegularExpression expForUnicodeInText;
};

#endif
