#ifndef PARSINGTOOL_HPP
#define PARSINGTOOL_HPP

#include <QtCore>
#include <QtNetwork>

class parsingToolClass
{
public:
    static void getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput);
    static QString getCaptchaLink(const QString& source);
    static QString getLastPageOfTopic(const QString& source);
    static QString getNameOfTopic(const QString& source);
    static QString getNumberOfConnected(const QString& source);
    static QList<int> getListOfMessageID(const QString& source);
    static QList<QString> getListOfPseudo(const QString& source);
    static QList<QString> getListOfDate(const QString& source);
    static QList<QString> getListOfMessage(const QString& source);
    static QString getForumOfTopic(const QString& source);
    static QString parsingMessages(QString thisMessage);
    static QNetworkRequest buildRequestWithThisUrl(QString url);
    static QList<QString> getListOfThisCapNumber(const QString &source, QRegExp exp, int capNumber);
    static void replaceWithCapNumber(QString& source, QRegExp exp, int capNumber, bool createLink = false, QString stringBefore = "",
                                     QString stringAfter = "", int secondCapNumber = 0, QString stringAfterAfter = "");
};

#endif
