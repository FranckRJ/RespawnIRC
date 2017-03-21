#ifndef MULTITYPETEXTBOX_HPP
#define MULTITYPETEXTBOX_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextCursor>
#include <QString>

#include "spellTextEdit.hpp"
#include "highlighter.hpp"

class multiTypeTextBoxClass : public QWidget
{
    Q_OBJECT
public:
    explicit multiTypeTextBoxClass(QWidget* parent = 0);
    void doStuffBeforeQuit();
    void clear();
    QString text() const;
    void setFocus();
    void setEditMode(bool newVal);
    void setTextEditSelected(bool newVal);
    void styleChanged();
    void settingsChanged();
public slots:
    void insertText(QString newText);
    void addBold();
    void addItalic();
    void addUnderLine();
    void addStrike();
    void addUList();
    void addOListe();
    void addQuote();
    void addCode();
    void addSpoil();
private:
    void moveCursor(QTextCursor::MoveOperation operation, int numberOfTime = 1);
    QString getSelectedText() const;
signals:
    void returnPressed();
private:
    QVBoxLayout layout;
    spellTextEditClass textEdit;
    highlighterClass* highlighter;
    QLineEdit lineEdit;
    bool textEditSelected = true;
    bool dicAreLoaded = false;
};

#endif
