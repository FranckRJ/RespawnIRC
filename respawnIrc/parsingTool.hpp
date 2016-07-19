#ifndef PARSINGTOOL_HPP
#define PARSINGTOOL_HPP

#include <QtCore>
#include <QtNetwork>

struct pseudoInfoStruct
{
    QString pseudoName;
    QString pseudoType;
};

struct messageStruct
{
    int idOfMessage;
    bool isAnEdit = false;
    bool isFirstMessage = false;
    pseudoInfoStruct pseudoInfo;
    QString date;
    QString wholeDate;
    QString message;
    QString lastTimeEdit;
    bool operator<(const messageStruct& otherStruct) const
    {
        return (idOfMessage < otherStruct.idOfMessage);
    }
};

struct topicStruct
{
    QString name;
    QString link;
    QString numberOfMessage;
};

struct ajaxInfoStruct
{
    QString list;
    QString mod;
};

class parsingToolClass
{
public:
    static ajaxInfoStruct getAjaxInfo(const QString& source);
    static QString getMessageEdit(QString& source);
    static QString getMessageQuote(QString& source);
    static QString getVersionName(const QString& source);
    static QString getVersionChangelog(const QString& source);
    static void getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput);
    static QString getCaptchaLink(const QString& source);
    static QString getErrorMessage(const QString& source);
    static QString getLastPageOfTopic(const QString& source);
    static QString getFirstPageOfTopic(const QString& source);
    static QString getBeforeLastPageOfTopic(const QString& source);
    static QString getNameOfTopic(const QString& source);
    static QString getNumberOfConnected(const QString& source);
    static QString getNumberOfMp(const QString& source);
    static QList<messageStruct> getListOfEntireMessagesWithoutMessagePars(const QString& source);
    static QList<topicStruct> getListOfTopic(const QString& source);
    static QString getForumOfTopic(const QString& source);
    static QString getForumName(const QString& source);
    static QString jvfLinkToJvcLink(const QString& source);
    static QString parsingMessages(QString thisMessage, bool showStickers, bool stickerToSmiley, int stickersSize,
                                   int nbMaxQuote, bool betterQuote, QStringList* listOfStickersUsed = nullptr, QStringList* listOfNoelshackImageUsed = nullptr,
                                   int noelshackImageWidth = 68, int noelshackImageHeight = 51);
    static QString parsingAjaxMessages(QString thisMessage);
    static QNetworkRequest buildRequestWithThisUrl(QString url);
    static QList<QString> getListOfThisCapNumber(const QString &source, const QRegularExpression& exp, int capNumber, bool globalMatch = true);
    static void removeAllOverlyQuote(QString& source, int maxNumberQuote);
    static bool replaceWithCapNumber(QString& source, const QRegularExpression& exp, int capNumber, QString stringBefore = "",
                                     QString stringAfter = "", int secondCapNumber = -1, QString stringAfterAfter = "",
                                     bool replaceReturnByBr = false, bool makeLinkIfPossible = false, bool replacePByBr = false, int additionnalOffset = -1);
private:
    static const QRegularExpression expForNormalLink;
    static const QRegularExpression expForAjaxTimestamp;
    static const QRegularExpression expForAjaxHash;
    static const QRegularExpression expForAjaxModTimestamp;
    static const QRegularExpression expForAjaxModHash;
    static const QRegularExpression expForMessageEdit;
    static const QRegularExpression expForMessageQuote;
    static const QRegularExpression expForVersionName;
    static const QRegularExpression expForVersionChangelog;
    static const QRegularExpression expForFormTopic;
    static const QRegularExpression expForFormConnect;
    static const QRegularExpression expForInput;
    static const QRegularExpression expForCaptcha;
    static const QRegularExpression expForError;
    static const QRegularExpression expForCurrentPage;
    static const QRegularExpression expForPageLink;
    static const QRegularExpression expForBeforeLastPage;
    static const QRegularExpression expForNameOfTopic;
    static const QRegularExpression expForNumberOfConnected;
    static const QRegularExpression expForMpJvc;
    static const QRegularExpression expForEntireMessage;
    static const QRegularExpression expForEntireTopic;
    static const QRegularExpression expForTopicNameAndLink;
    static const QRegularExpression expForTopicNumberMessage;
    static const QRegularExpression expForMessageID;
    static const QRegularExpression expForPseudo;
    static const QRegularExpression expForDate;
    static const QRegularExpression expForMessage;
    static const QRegularExpression expForEdit;
    static const QRegularExpression expForForum;
    static const QRegularExpression expForForumName;
    static const QRegularExpression expForJvfLink;
    static const QRegularExpression expForSmiley;
    static const QRegularExpression expForStickers;
    static const QRegularExpression expForLongLink;
    static const QRegularExpression expForShortLink;
    static const QRegularExpression expForJvcLink;
    static const QRegularExpression expForNoelshack;
    static const QRegularExpression expForSpoilLine;
    static const QRegularExpression expForSpoilBlock;
    static const QRegularExpression expForCodeLine;
    static const QRegularExpression expForCodeBlock;
    static const QRegularExpression expForAllJVCare;
    static const QRegularExpression expForUnicodeInText;
    static const QRegularExpression expForHtmlTag;
};

#endif
