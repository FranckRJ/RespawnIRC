#ifndef HIGHLIGHTER_HPP
#define HIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QString>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextDocument>
#include "hunspell/hunspell.hxx"

class highlighterClass : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit highlighterClass(QTextDocument* parent = 0);
    ~highlighterClass();
    void enableSpellChecking(const bool state);
    bool setDic(const QString newSpellDic);
    void styleChanged();
public slots:
    void addWordToDic(QString word);
private:
    void highlightBlock(const QString& text) override;
    void spellCheck(const QString& text);
    bool checkWord(QString word);
private:
    QString spellDic;
    QString spellEncoding;
    Hunspell* spellChecker = nullptr;
    bool spellCheckingIsEnabled = false;
    QTextCharFormat spellCheckFormat;
    QTextCodec* codec = nullptr;
};

#endif
