#ifndef PARSINGTOOL_HPP
#define PARSINGTOOL_HPP

#include <QtCore>
#include <QtNetwork>

class parsingToolClass
{
public:
    static void getListOfHiddenInputFromThisForm(QString& source, QString formName, QList<QPair<QString, QString> >& listOfInput);
    static QString getLastPageOfTopic(QString source);
    static QList<int> getListOfMessageID(QString source);
    static QList<QString> getListOfPseudo(QString source);
    static QList<QString> getListOfDate(QString source);
    static QList<QString> getListOfMessage(QString source);
    static QString parsingMessages(QString thisMessage);
    static QNetworkRequest buildRequestWithThisUrl(QString url);
    static QList<QString> getListOfThisCapNumber(QString& source, QRegExp exp, int capNumber);
    static void replaceWithCapNumber(QString& source, QRegExp exp, int capNumber, bool createLink = false, QString stringBefore = "", QString stringAfter = "");
};

#endif
