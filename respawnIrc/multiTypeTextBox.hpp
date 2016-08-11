#ifndef MULTITYPETEXTBOX_HPP
#define MULTITYPETEXTBOX_HPP

class QString;

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextCursor>

#include "spellTextEdit.hpp"
#include "highlighter.hpp"

class multiTypeTextBoxClass : public QWidget
{
    Q_OBJECT
public:
    multiTypeTextBoxClass(QWidget* parent = 0);
    void clear();
    QString text();
    void moveCursor(QTextCursor::MoveOperation operation, int numberOfTime = 1);
    QString getSelectedText();
    void setFocus();
    void setEditMode(bool newVal);
    void styleChanged();
    void settingsChanged();
public slots:
    void insertText(QString newText);
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
    bool textEditSelected = true;
    bool dicAreLoaded = false;
};

#endif
