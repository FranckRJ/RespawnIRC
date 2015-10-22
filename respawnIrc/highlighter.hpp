#ifndef HIGHLIGHTER_HPP
#define HIGHLIGHTER_HPP

#include <QtGui>
#include <QtCore>
#include "hunspell/hunspell.hxx"

class highlighterClass : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    highlighterClass(QTextDocument* parent = 0);
    ~highlighterClass();
    void enableSpellChecking(const bool state);
    bool setDic(const QString newSpellDic);
public slots:
    void addWordToDic(QString word);
protected:
    void highlightBlock(const QString& text);
    void spellCheck(const QString& text);
    bool checkWord(QString word);
private:
    QString spellDic;
    QString spellEncoding;
    Hunspell* spellChecker;
    bool spellCheckActive;
    bool spellerError;
    QTextCharFormat spellCheckFormat;
    QTextCodec* codec;

};

#endif
