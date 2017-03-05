#ifndef SPELLTEXTEDIT_HPP
#define SPELLTEXTEDIT_HPP

#include <QWidget>
#include <QTextEdit>
#include <QString>
#include <QVector>
#include <QAction>
#include <QStringList>
#include <QPoint>
#include <QContextMenuEvent>
#include <QTextCodec>
#include "hunspell/hunspell.hxx"

class spellTextEditClass : public QTextEdit
{
    Q_OBJECT
public:
    explicit spellTextEditClass(QWidget* parent = 0);
    ~spellTextEditClass();
    void doStuffBeforeQuit();
    void enableSpellChecking(bool newVal);
    bool setDic(const QString newSpellDic);
private:
    void searchWordBoundaryPosition(QString textBlock, int checkPos, int& beginPos, int& endPos) const;
    QStringList getWordPropositions(const QString word) const;
    QString getWordUnderCursor(QPoint cursorPos) const;
    void createActions();
    void contextMenuEvent(QContextMenuEvent* event) override;
    bool checkWord(QString word) const;
private slots:
    void correctWord();
    void addWordToUserDic();
    void ignoreWord();
signals:
    void addWord(QString word);
private:
    QVector<QAction*> wordPropositionsActions;
    QString spellDic;
    Hunspell* spellChecker = nullptr;
    QTextCodec* codecUsed = nullptr;
    QStringList addedWords;
    QPoint lastPos;
    bool spellCheckingIsEnabled = false;
};

#endif
