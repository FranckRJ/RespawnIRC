#ifndef PARSINGTOOL_HPP
#define PARSINGTOOL_HPP

#include <functional>
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
    long idOfMessage;
    bool isAnEdit = false;
    bool isFirstMessage = false;
    pseudoInfoStruct pseudoInfo;
    QString date;
    QString wholeDate;
    QString message;
    QString lastTimeEdit;
    QString signature;
    QString avatarLink;
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
    pseudoInfoStruct pseudoInfo;
    QString topicType;
};

struct ajaxInfoStruct
{
    QString list;
    QString mod;
};

struct infoForMessageParsingStruct
{
    bool showStickers;
    bool hideUglyImages;
    bool stickerToSmiley;
    int stickersSize;
    int nbMaxQuote;
    bool betterQuote;
    int noelshackImageWidth;
    int noelshackImageHeight;
    QStringList* listOfStickersUsed = nullptr;
    QStringList* listOfNoelshackImageUsed = nullptr;
};

namespace parsingToolClass
{
    void generateNewUserAgent();
    bool checkIfTopicAreSame(const QString& firstTopic, const QString& secondTopic);
    ajaxInfoStruct getAjaxInfo(const QString& source);
    QString getMessageEdit(const QString& source);
    QString getMessageQuote(const QString& source);
    QString getWebsite(const QString& topicLink);
    QString getVersionName(const QString& source);
    QString getVersionChangelog(const QString& source);
    void getListOfHiddenInputFromThisForm(const QString& source, QString formName, QList<QPair<QString, QString>>& listOfInput);
    bool getTopicLocked(const QString& source);
    QString getCaptchaLink(const QString& source);
    QString getErrorMessage(const QString& source);
    QString getNextPageOfTopic(const QString& source, const QString& website);
    QString getLastPageOfTopic(const QString& source, const QString& website);
    QString getFirstPageOfTopic(const QString& topicLink);
    QString getBeforeLastPageOfTopic(const QString& topicLink);
    QString getNameOfTopic(const QString& source);
    QString getNumberOfConnected(const QString& source);
    int getNumberOfMp(const QString& source);
    QList<messageStruct> getListOfEntireMessagesWithoutMessagePars(const QString& source);
    QList<topicStruct> getListOfTopic(const QString& source, const QString& website);
    QString getForumOfTopic(const QString& topicLink);
    QString getForumName(const QString& source);
    QString jvfLinkToJvcLink(const QString& jvfTopicLink);
    QString parsingMessages(QString thisMessage, infoForMessageParsingStruct infoForParsing, bool reallyDownloadStickers = true);
    QString parsingAjaxMessages(QString thisMessage);
    QString specialCharToNormalChar(QString thisMessage);
    QNetworkRequest buildRequestWithThisUrl(QString url);
    QList<QString> getListOfThisCapNumber(const QString& source, const QRegularExpression& exp, int capNumber, bool globalMatch = true);
    void removeAllOverlyQuote(QString& source, int maxNumberQuote);
    void removeAllOverlySpoils(QString& source);
    void replaceWithCapNumber(QString& source, const QRegularExpression& exp, int capNumber, QString stringBefore = "",
                                     QString stringAfter = "", int secondCapNumber = -1, QString stringAfterAfter = "",
                                     std::function<QString(QString)> capModificator = nullptr);
}

#endif
