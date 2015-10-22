#include "multiTypeTextBox.hpp"

multiTypeTextBoxClass::multiTypeTextBoxClass(QWidget *parent) : QWidget(parent)
{
    highlighter = new highlighterClass(textEdit.document());

    textEdit.setTabChangesFocus(true);
    textEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    textEdit.setMaximumHeight(65);
    textEdit.setAcceptRichText(false);
    textEdit.setDic("ressources/fr");
    highlighter->setDic("ressources/fr");
    highlighter->enableSpellChecking(true);
    textEditSelected = true;

    layout.addWidget(&textEdit);
    layout.setMargin(0);

    setLayout(&layout);

    QObject::connect(&lineEdit, &QLineEdit::returnPressed, this, &multiTypeTextBoxClass::returnIsPressed);
    QObject::connect(&textEdit, &spellTextEditClass::addWord, highlighter, &highlighterClass::addWordToDic);
}

void multiTypeTextBoxClass::clear()
{
    textEdit.clear();
    lineEdit.clear();
}

QString multiTypeTextBoxClass::text()
{
    if(textEditSelected == true)
    {
        return textEdit.toPlainText();
    }
    else
    {
        return lineEdit.text();
    }
}

void multiTypeTextBoxClass::insertText(QString newText)
{
    if(textEditSelected == true)
    {
        textEdit.insertPlainText(newText);
        textEdit.verticalScrollBar()->updateGeometry();
        textEdit.verticalScrollBar()->setValue(textEdit.verticalScrollBar()->maximum());
    }
    else
    {
        lineEdit.insert(newText);
    }
}

void multiTypeTextBoxClass::moveCursor(QTextCursor::MoveOperation operation, int numberOfTime)
{
    if(textEditSelected == true)
    {
        for(int i = 0; i < numberOfTime; ++i)
        {
            textEdit.moveCursor(operation);
        }
    }
    else
    {
        if(operation == QTextCursor::Left || operation == QTextCursor::Right)
        {
            if(operation == QTextCursor::Left)
            {
                numberOfTime = -numberOfTime;
            }
            lineEdit.setCursorPosition(lineEdit.cursorPosition() + numberOfTime);
        }
        else if(operation == QTextCursor::StartOfLine)
        {
            lineEdit.setCursorPosition(0);
        }
        else if(operation == QTextCursor::EndOfLine)
        {
            lineEdit.setCursorPosition(lineEdit.text().size());
        }
    }
}

void multiTypeTextBoxClass::setFocus()
{
    QWidget::setFocus();

    if(textEditSelected == true)
    {
        textEdit.setFocus();
    }
    else
    {
        lineEdit.setFocus();
    }
}

void multiTypeTextBoxClass::setTextEditSelected(bool newVal)
{
    clear();
    layout.removeWidget(layout.itemAt(0)->widget());
    textEditSelected = newVal;

    if(textEditSelected == true)
    {
        lineEdit.setVisible(false);
        layout.addWidget(&textEdit);
        textEdit.setVisible(true);
    }
    else
    {
        textEdit.setVisible(false);
        layout.addWidget(&lineEdit);
        lineEdit.setVisible(true);
    }
}

void multiTypeTextBoxClass::returnIsPressed()
{
    emit returnPressed();
}

void multiTypeTextBoxClass::addBold()
{
    insertText("''''''");
    moveCursor(QTextCursor::Left, 3);
    setFocus();
}

void multiTypeTextBoxClass::addItalic()
{
    insertText("''''");
    moveCursor(QTextCursor::Left, 2);
    setFocus();
}

void multiTypeTextBoxClass::addUnderLine()
{
    insertText("<u></u>");
    moveCursor(QTextCursor::Left, 4);
    setFocus();
}

void multiTypeTextBoxClass::addStrike()
{
    insertText("<s></s>");
    moveCursor(QTextCursor::Left, 4);
    setFocus();
}

void multiTypeTextBoxClass::addUList()
{
    moveCursor(QTextCursor::StartOfLine);
    insertText("* ");
    moveCursor(QTextCursor::EndOfLine);
    setFocus();
}

void multiTypeTextBoxClass::addOListe()
{
    moveCursor(QTextCursor::StartOfLine);
    insertText("# ");
    moveCursor(QTextCursor::EndOfLine);
    setFocus();
}

void multiTypeTextBoxClass::addQuote()
{
    moveCursor(QTextCursor::StartOfLine);
    insertText("> ");
    moveCursor(QTextCursor::EndOfLine);
    setFocus();
}

void multiTypeTextBoxClass::addCode()
{
    insertText("<code></code>");
    moveCursor(QTextCursor::Left, 7);
    setFocus();
}

void multiTypeTextBoxClass::addSpoil()
{
    insertText("<spoil></spoil>");
    moveCursor(QTextCursor::Left, 8);
    setFocus();
}
