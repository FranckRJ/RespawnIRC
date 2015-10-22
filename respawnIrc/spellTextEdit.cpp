#include "spellTextEdit.hpp"

spellTextEditClass::spellTextEditClass(QWidget *parent) : QTextEdit(parent)
{
    spellChecker = 0;

    wordPropositionsActions.fill(new QAction(this), 5);
    createActions();
    setDic("");
    addedWords.clear();
}


spellTextEditClass::~spellTextEditClass()
{
    QFile file("ressources/user_" + spellDic + ".dic");
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
    if(file.open(QIODevice::WriteOnly | QIODevice::Text) == true)
    {
        QTextStream writeStream(&file);
        QByteArray encodedString;

        writeStream << addedWords.count() << "\n";

        for(int i = 0; i < addedWords.size(); ++i)
        {
            encodedString = codecUsed->fromUnicode(addedWords.at(i));
            writeStream << encodedString.data() << "\n";
        }

        file.close();
    }
}

QStringList spellTextEditClass::getWordPropositions(const QString word)
{
    QStringList wordList;
    if(spellChecker != 0)
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

    if(spellChecker != 0)
    {
        delete spellChecker;
    }
    spellChecker = new Hunspell(spellDic.toLatin1() + ".aff", spellDic.toLatin1() + ".dic");

    QFileInfo fileInfoForDic(spellDic + ".dic");
    if(fileInfoForDic.exists() == false || fileInfoForDic.isReadable() == false)
    {
        delete spellChecker;
        spellChecker = 0;
        codecUsed = QTextCodec::codecForName("UTF-8");
        return false;
    }
    else
    {
        codecUsed = QTextCodec::codecForName(QString(spellChecker->get_dic_encoding()).toLatin1());
    }

    QFileInfo fileInfoForUserDic("ressources/user_" + spellDic + ".dic");
    if(fileInfoForUserDic.exists() == true && fileInfoForUserDic.isReadable() == true)
    {
        spellChecker->add_dic(fileInfoForUserDic.filePath().toLatin1());
    }

    return true;
}

QString spellTextEditClass::getWordUnderCursor(QPoint cursorPos)
{
    QTextCursor cursor = cursorForPosition(cursorPos);
    QString textBlock = cursor.block().text();
    int pos = cursor.columnNumber();
    int end = textBlock.indexOf(QRegExp("\\W+"), pos);
    int begin = textBlock.lastIndexOf(QRegExp("\\W+"), pos);

    textBlock = textBlock.mid(begin + 1, end - begin - 1);

    return textBlock;
}

void spellTextEditClass::createActions()
{
    for(int i = 0; i < wordPropositionsActions.size(); ++i)
    {
        wordPropositionsActions[i] = new QAction(this);
        wordPropositionsActions[i]->setVisible(false);
        connect(wordPropositionsActions[i], &QAction::triggered, this, &spellTextEditClass::correctWord);
    }
}

void spellTextEditClass::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menuRightClick = createStandardContextMenu();
    lastPos = event->pos();
    QStringList listOfWord = getWordPropositions(getWordUnderCursor(lastPos));

    if(listOfWord.isEmpty() == false)
    {
        menuRightClick->addSeparator();
        menuRightClick->addAction("Add...", this, SLOT(addWordToUserDic()));
        menuRightClick->addAction("Ignore...", this, SLOT(ignoreWord()));
        menuRightClick->addSeparator();

        for(int i = 0; i < qMin(wordPropositionsActions.size(), listOfWord.size()); ++i)
        {
            wordPropositionsActions[i]->setText(listOfWord.at(i).trimmed());
            wordPropositionsActions[i]->setVisible(true);
            menuRightClick->addAction(wordPropositionsActions[i]);
        }

    }

    menuRightClick->exec(event->globalPos());
    delete menuRightClick;
}

void spellTextEditClass::correctWord()
{
    QAction* thisAction = qobject_cast<QAction*>(sender());
    if(thisAction != 0)
    {
        QString replacement = thisAction->text();
        QTextCursor cursor = cursorForPosition(lastPos);

        cursor.select(QTextCursor::WordUnderCursor);
        cursor.deleteChar();
        cursor.insertText(replacement);
    }
}

void spellTextEditClass::addWordToUserDic()
{
    QString wordUnderCursor = getWordUnderCursor(lastPos);
    QByteArray encodedString;

    encodedString = codecUsed->fromUnicode(wordUnderCursor);
    spellChecker->add(encodedString.data());
    addedWords.append(wordUnderCursor);

    emit addWord(wordUnderCursor);
}

void spellTextEditClass::ignoreWord()
{
    QString wordUnderCursor = getWordUnderCursor(lastPos);
    QByteArray encodedString;

    encodedString = codecUsed->fromUnicode(wordUnderCursor);
    spellChecker->add(encodedString.data());

    emit addWord(wordUnderCursor);
}
