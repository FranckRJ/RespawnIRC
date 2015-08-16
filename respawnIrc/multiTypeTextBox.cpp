#include "multiTypeTextBox.hpp"

multiTypeTextBoxClass::multiTypeTextBoxClass(QWidget *parent) : QWidget(parent)
{
    textEdit.setTabChangesFocus(true);
    textEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    textEdit.setMaximumHeight(65);
    textEdit.setAcceptRichText(false);
    textEditSelected = true;

    layout.addWidget(&textEdit);
    layout.setMargin(0);

    setLayout(&layout);

    connect(&lineEdit, SIGNAL(returnPressed()), this, SLOT(returnIsPressed()));
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
