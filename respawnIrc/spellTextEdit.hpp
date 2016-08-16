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
    QStringList getWordPropositions(const QString word);
    bool setDic(const QString newSpellDic);
    QString getWordUnderCursor(QPoint cursorPos);
signals:
    void addWord(QString word);
protected:
    void createActions();
    void contextMenuEvent(QContextMenuEvent* event);
    bool checkWord(QString word);
private slots:
    void correctWord();
    void addWordToUserDic();
    void ignoreWord();
private:
    QVector<QAction*> wordPropositionsActions;
    QString spellDic;
    Hunspell* spellChecker = nullptr;
    QTextCodec* codecUsed = nullptr;
    QStringList addedWords;
    QPoint lastPos;
};

#endif
