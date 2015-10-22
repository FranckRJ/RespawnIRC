#ifndef SPELLTEXTEDIT_HPP
#define SPELLTEXTEDIT_HPP

#include <QtWidgets>
#include "hunspell/hunspell.hxx"

class spellTextEditClass : public QTextEdit
{
    Q_OBJECT
public:
    spellTextEditClass(QWidget* parent = 0);
    ~spellTextEditClass();
    QStringList getWordPropositions(const QString word);
    bool setDic(const QString newSpellDic);
    QString getWordUnderCursor(QPoint cursorPos);
signals:
    void addWord(QString word);
protected:
    void createActions();
    void contextMenuEvent(QContextMenuEvent* event);
private slots:
    void correctWord();
    void addWordToUserDic();
    void ignoreWord();
private:
    QVector<QAction*> wordPropositionsActions;
    QString spellDic;
    Hunspell* spellChecker;
    QTextCodec* codecUsed;
    QStringList addedWords;
    QPoint lastPos;
};

#endif
