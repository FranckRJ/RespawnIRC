#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QByteArray>
#include <QCoreApplication>
#include <QTextCursor>
#include <QFileInfo>
#include <QTextBlock>
#include <QIODevice>
#include <QMenu>
#include <QRegExp>

#include "spellTextEdit.hpp"

spellTextEditClass::spellTextEditClass(QWidget* parent) : QTextEdit(parent)
{
    wordPropositionsActions.fill(new QAction(this), 5);
    createActions();
    setDic("");
}

spellTextEditClass::~spellTextEditClass()
{
    if(addedWords.isEmpty() == false)
    {
        QFile file(QCoreApplication::applicationDirPath() + "/user_" + spellDic + ".dic");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text) == true)
        {
            QTextStream readStream(&file);

            readStream.readLine();
            while(readStream.atEnd() == false)
            {
                QString line = readStream.readLine();
                if(addedWords.contains(line) == false)
                {
                    addedWords << line;
                }
            }

            file.close();
        }
        if(file.open(QIODevice::WriteOnly | QIODevice::Text) == true && codecUsed != nullptr)
        {
            QTextStream writeStream(&file);
            QByteArray encodedString;

            writeStream << addedWords.count() << "\n";

            for(const QString& thisWord : addedWords)
            {
                encodedString = codecUsed->fromUnicode(thisWord);
                writeStream << encodedString.data() << "\n";
            }

            file.close();
        }
    }

    if(spellChecker != nullptr)
    {
        delete spellChecker;
    }
}

void spellTextEditClass::enableSpellChecking(bool newVal)
{
    spellCheckingIsEnabled = newVal;
}

void spellTextEditClass::searchWordBoundaryPosition(QString textBlock, int checkPos, int& beginPos, int& endPos)
{
    endPos = textBlock.indexOf(QRegExp("[^\\w'-]"), checkPos);
    beginPos = textBlock.lastIndexOf(QRegExp("[^\\w'-]"), checkPos);

    if(endPos == -1)
    {
        endPos = textBlock.size();
    }

    while((textBlock.at(beginPos + 1) == '\'' || textBlock.at(beginPos + 1) == '-') &&
          (beginPos + 1) <= endPos && (beginPos + 2) < textBlock.size())
    {
        beginPos += 1;
    }
    while((textBlock.at(endPos - 1) == '\'' || textBlock.at(endPos - 1) == '-') &&
          endPos >= (beginPos + 1) && (endPos - 2) >= 0)
    {
        endPos -= 1;
    }

    if(beginPos == endPos)
    {
        beginPos -= 1;
    }

    if((beginPos + 1) > checkPos || (endPos - 1) < checkPos)
    {
        endPos = checkPos;
        beginPos = endPos - 1;
    }
}

QStringList spellTextEditClass::getWordPropositions(const QString word)
{
    QStringList wordList;
    if(spellChecker != nullptr && codecUsed != nullptr)
    {
        QByteArray encodedString;
        encodedString = codecUsed->fromUnicode(word);
        bool check = spellChecker->spell(encodedString.data());

        if(check == false)
        {
            char** listOfWord;
            int numberOfSuggest = spellChecker->suggest(&listOfWord, encodedString.data());
            if(numberOfSuggest > 0)
            {
                for(int i = 0; i < numberOfSuggest; ++i)
                {
                    wordList.append(codecUsed->toUnicode(listOfWord[i]));
                }

                spellChecker->free_list(&listOfWord, numberOfSuggest);
            }
        }
    }

    return wordList;
}

bool spellTextEditClass::setDic(const QString newSpellDic)
{
    spellDic = newSpellDic;

    if(spellChecker != nullptr)
    {
        delete spellChecker;
    }

    QFileInfo fileInfoForDic(QCoreApplication::applicationDirPath() + "/resources/" + spellDic + ".dic");
    if(fileInfoForDic.exists() == false || fileInfoForDic.isReadable() == false)
    {
        spellChecker = nullptr;
        codecUsed = QTextCodec::codecForName("UTF-8");
        return false;
    }
    else
    {
        QFileInfo fileInfoForUserDic(QCoreApplication::applicationDirPath() + "/user_" + spellDic + ".dic");
        spellChecker = new Hunspell((QCoreApplication::applicationDirPath() + "/resources/" + spellDic.toLatin1() + ".aff").toStdString().c_str(),
                                    (QCoreApplication::applicationDirPath() + "/resources/" + spellDic.toLatin1() + ".dic").toStdString().c_str());

        if(fileInfoForUserDic.exists() == true && fileInfoForUserDic.isReadable() == true)
        {
            spellChecker->add_dic(fileInfoForUserDic.filePath().toLatin1());
        }

        codecUsed = QTextCodec::codecForName(QString(spellChecker->get_dic_encoding()).toLatin1());
    }

    return true;
}

QString spellTextEditClass::getWordUnderCursor(QPoint cursorPos)
{
    QTextCursor cursor = cursorForPosition(cursorPos);
    QString textBlock = cursor.block().text();

    if(textBlock.isEmpty() == false)
    {
        int pos = cursor.positionInBlock();
        int end;
        int begin;

        searchWordBoundaryPosition(textBlock, pos, begin, end);

        textBlock = textBlock.mid(begin + 1, end - begin - 1);
    }

    return textBlock;
}

void spellTextEditClass::createActions()
{
    for(QAction*& thisAction : wordPropositionsActions)
    {
        thisAction = new QAction(this);
        thisAction->setVisible(false);
        connect(thisAction, &QAction::triggered, this, &spellTextEditClass::correctWord);
    }
}

void spellTextEditClass::contextMenuEvent(QContextMenuEvent* event)
{
    if(spellChecker != nullptr && codecUsed != nullptr && spellCheckingIsEnabled == true)
    {
        QFont thisFont;
        lastPos = event->pos();
        QString wordUnderCursor = getWordUnderCursor(lastPos);
        QMenu* menuRightClick = createStandardContextMenu();
        QStringList listOfWord = getWordPropositions(wordUnderCursor);

        thisFont.setBold(true);

        if(wordUnderCursor.size() > 1 && checkWord(wordUnderCursor) == false)
        {
            menuRightClick->addSeparator();
            menuRightClick->addAction("Add...", this, SLOT(addWordToUserDic()));
            menuRightClick->addAction("Ignore...", this, SLOT(ignoreWord()));

            if(listOfWord.isEmpty() == false)
            {
                menuRightClick->addSeparator();

                for(int i = 0; i < qMin(wordPropositionsActions.size(), listOfWord.size()); ++i)
                {
                    wordPropositionsActions[i]->setText(listOfWord.at(i).trimmed().replace("’", "'"));
                    wordPropositionsActions[i]->setVisible(true);
                    wordPropositionsActions[i]->setFont(thisFont);
                    menuRightClick->addAction(wordPropositionsActions[i]);
                }

            }
        }

        menuRightClick->exec(event->globalPos());
        delete menuRightClick;
    }
    else
    {
        QTextEdit::contextMenuEvent(event);
    }
}

bool spellTextEditClass::checkWord(QString word)
{
    if(spellChecker != nullptr && codecUsed != nullptr)
    {
        return spellChecker->spell(codecUsed->fromUnicode(word).data());
    }
    else
    {
        return false;
    }
}

void spellTextEditClass::correctWord()
{
    QAction* thisAction = qobject_cast<QAction*>(sender());

    if(thisAction != nullptr)
    {
        QString replacement = thisAction->text();
        QTextCursor cursor = cursorForPosition(lastPos);
        QString textBlock = cursor.block().text();

        if(textBlock.isEmpty() == false)
        {
            int pos = cursor.positionInBlock();
            int end;
            int begin;

            searchWordBoundaryPosition(textBlock, pos, begin, end);

            cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, pos - begin - 1);
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, end - begin - 1);

            cursor.removeSelectedText();
        }

        cursor.insertText(replacement);
    }
}

void spellTextEditClass::addWordToUserDic()
{
    if(spellChecker != nullptr && codecUsed != nullptr)
    {
        QString wordUnderCursor = getWordUnderCursor(lastPos);
        QByteArray encodedString;

        encodedString = codecUsed->fromUnicode(wordUnderCursor);
        spellChecker->add(encodedString.data());
        addedWords.append(wordUnderCursor);

        emit addWord(wordUnderCursor);
    }
}

void spellTextEditClass::ignoreWord()
{
    if(spellChecker != nullptr && codecUsed != nullptr)
    {
        QString wordUnderCursor = getWordUnderCursor(lastPos);
        QByteArray encodedString;

        encodedString = codecUsed->fromUnicode(wordUnderCursor);
        spellChecker->add(encodedString.data());

        emit addWord(wordUnderCursor);
    }
}
