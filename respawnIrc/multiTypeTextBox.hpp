#ifndef MULTITYPETEXTBOX_HPP
#define MULTITYPETEXTBOX_HPP

#include <QtWidgets>
#include <QtCore>

#include "spellTextEdit.hpp"
#include "highlighter.hpp"

class multiTypeTextBoxClass : public QWidget
{
    Q_OBJECT
public:
    multiTypeTextBoxClass(QWidget* parent = 0);
    void clear();
    QString text();
    void insertText(QString newText);
    void moveCursor(QTextCursor::MoveOperation operation, int numberOfTime = 1);
    void setFocus();
public slots:
    void setTextEditSelected(bool newVal);
    void returnIsPressed();
    void addBold();
    void addItalic();
    void addUnderLine();
    void addStrike();
    void addUList();
    void addOListe();
    void addQuote();
    void addCode();
    void addSpoil();
signals:
    void returnPressed();
private:
    QVBoxLayout layout;
    spellTextEditClass textEdit;
    highlighterClass* highlighter;
    QLineEdit lineEdit;
    bool textEditSelected;
};

#endif
