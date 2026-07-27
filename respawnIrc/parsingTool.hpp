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
    /* Le texte du message tel qu'il a été écrit, avant rendu HTML. Sert à citer sans
     * avoir à le redemander au site. */
    QString messageRaw;
    QString lastTimeEdit;
    QString lastTimeEditHourOnly;
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

struct pagerInfoStruct
{
    int currentPage = 0;
    int numberOfPages = 0;
    /* Lien de n'importe quelle page du topic, pour pouvoir en reconstruire d'autres :
     * le payload ne liste pas toutes les pages. */
    QString linkOfAPage;
};

struct infoForMessageParsingStruct
{
    bool showStickers;
    bool hideUglyImages;
    bool stickerToSmiley;
    bool smileyToText;
    int stickersSize;
    int nbMaxQuote;
    bool betterQuote;
    bool betterCodeTag;
    int noelshackImageWidth;
    int noelshackImageHeight;
    QStringList* listOfStickersUsed = nullptr;
    QStringList* listOfNoelshackImageUsed = nullptr;
};

namespace parsingTool
{
    void generateNewUserAgent();
    bool checkIfTopicAreSame(const QString& firstTopic, const QString& secondTopic);
    ajaxInfoStruct getAjaxInfo(const QString& source);
    QString quoteThisRawMessage(QString rawMessage);
    QString getWebsite(const QString& topicLink);
    QString getVersionName(const QString& source);
    QString getVersionChangelog(const QString& source);
    void getListOfHiddenInputFromThisForm(const QString& source, QString formName, QList<QPair<QString, QString>>& listOfInput);
    bool getTopicLocked(const QString& source);
    QString getErrorMessage(const QString& source, QString defaultError = "Le message n'a pas été envoyé.");
    QString getErrorMessageInJSON(const QString& source, bool needToParseAsAjaxMessage = true, QString defaultError = "Le message n'a pas été envoyé.");
    QString getNextPageOfTopic(const QString& source, const QString& website);
    QString getLastPageOfTopic(const QString& source, const QString& website);
    pagerInfoStruct getPagerInfo(const QString& source, const QString& website);
    QString buildLinkForThisPage(const QString& linkOfAnyPage, int wantedPage);
    QString getFirstPageOfTopic(const QString& topicLink);
    QString getBeforeLastPageOfTopic(const QString& topicLink);
    QString getNameOfTopic(const QString& source);
    QString getNumberOfConnected(const QString& source);
    int getNumberOfMp(const QString& source);
    QList<messageStruct> getListOfEntireMessagesWithoutMessagePars(const QString& source);
    QList<topicStruct> getListOfTopic(const QString& source, const QString& website);
    QString getForumOfTopic(const QString& topicLink);
    QString getForumIdOfThisTopic(const QString& topicLink);
    QString getTopicIdOfThisTopic(const QString& topicLink);
    QByteArray buildMultipartFormData(const QList<QPair<QString, QString>>& listOfField, QByteArray& boundaryUsed);
    QString getForumName(const QString& source);
    QString jvfLinkToJvcLink(const QString& jvfTopicLink);
    QString normalAvatarLinkToHDLink(const QString& avatarLink);
    QString roleToPseudoType(const QString& role);
    QString stateOfTopicToTopicType(const QString& stateTopic, const QString& stateIcon);
    QString removeSchemeOfUrl(const QString& url);
    QString getHourOfDate(const QString& wholeDate);
    QString parsingMessages(QString thisMessage, infoForMessageParsingStruct infoForParsing, bool reallyDownloadStickers = true);
    QString makeBasicPreParseOfMessage(QString thisMessage);
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
