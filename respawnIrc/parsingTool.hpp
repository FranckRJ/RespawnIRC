#ifndef PARSINGTOOL_HPP
#define PARSINGTOOL_HPP

#include <QString>
#include <QList>
#include <QPair>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QStringList>

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

struct infoForMessageParsingStruct
{
    bool showStickers;
    bool stickerToSmiley;
    int stickersSize;
    int nbMaxQuote;
    bool betterQuote;
    int noelshackImageWidth;
    int noelshackImageHeight;
};

namespace parsingToolClass
{
    ajaxInfoStruct getAjaxInfo(const QString& source);
    QString getMessageEdit(QString& source);
    QString getMessageQuote(QString& source);
    QString getVersionName(const QString& source);
    QString getVersionChangelog(const QString& source);
    void getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput);
    QString getCaptchaLink(const QString& source);
    QString getErrorMessage(const QString& source);
    QString getLastPageOfTopic(const QString& source);
    QString getFirstPageOfTopic(const QString& source);
    QString getBeforeLastPageOfTopic(const QString& source);
    QString getNameOfTopic(const QString& source);
    QString getNumberOfConnected(const QString& source);
    QString getNumberOfMp(const QString& source);
    QList<messageStruct> getListOfEntireMessagesWithoutMessagePars(const QString& source);
    QList<topicStruct> getListOfTopic(const QString& source);
    QString getForumOfTopic(const QString& source);
    QString getForumName(const QString& source);
    QString jvfLinkToJvcLink(const QString& source);
    QString parsingMessages(QString thisMessage, infoForMessageParsingStruct infoForParsing, QStringList* listOfStickersUsed = nullptr, QStringList* listOfNoelshackImageUsed = nullptr);
    QString parsingAjaxMessages(QString thisMessage);
    QNetworkRequest buildRequestWithThisUrl(QString url);
    QList<QString> getListOfThisCapNumber(const QString &source, const QRegularExpression& exp, int capNumber, bool globalMatch = true);
    void removeAllOverlyQuote(QString& source, int maxNumberQuote);
    bool replaceWithCapNumber(QString& source, const QRegularExpression& exp, int capNumber, QString stringBefore = "",
                                     QString stringAfter = "", int secondCapNumber = -1, QString stringAfterAfter = "",
                                     bool replaceReturnByBr = false, bool makeLinkIfPossible = false, bool replacePByBr = false, int additionnalOffset = -1);
}

#endif
