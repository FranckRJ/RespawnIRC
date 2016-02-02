#include "spellTextEdit.hpp"

spellTextEditClass::spellTextEditClass(QWidget *parent) : QTextEdit(parent)
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
        if(file.open(QIODevice::WriteOnly | QIODevice::Text) == true && codecUsed != 0)
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

    if(spellChecker != 0)
    {
        delete spellChecker;
    }
}

QStringList spellTextEditClass::getWordPropositions(const QString word)
{
    QStringList wordList;
    if(spellChecker != 0 && codecUsed != 0)
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

    QFileInfo fileInfoForDic(QCoreApplication::applicationDirPath() + "/ressources/" + spellDic + ".dic");
    if(fileInfoForDic.exists() == false || fileInfoForDic.isReadable() == false)
    {
        spellChecker = 0;
        codecUsed = QTextCodec::codecForName("UTF-8");
        return false;
    }
    else
    {
        QFileInfo fileInfoForUserDic(QCoreApplication::applicationDirPath() + "/user_" + spellDic + ".dic");
        spellChecker = new Hunspell((QCoreApplication::applicationDirPath() + "/ressources/" + spellDic.toLatin1() + ".aff").toStdString().c_str(),
                                    (QCoreApplication::applicationDirPath() + "/ressources/" + spellDic.toLatin1() + ".dic").toStdString().c_str());

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
    int pos = cursor.positionInBlock();
    int end = textBlock.indexOf(QRegExp("[^\\w'-]"), pos);
    int begin = textBlock.lastIndexOf(QRegExp("[^\\w'-]"), pos);

    if(end == -1)
    {
        end = textBlock.size();
    }

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
    QFont thisFont;
    QMenu* menuRightClick = createStandardContextMenu();
    lastPos = event->pos();
    QStringList listOfWord = getWordPropositions(getWordUnderCursor(lastPos));

    thisFont.setBold(true);

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
            wordPropositionsActions[i]->setFont(thisFont);
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
        QString textBlock = cursor.block().text();
        int pos = cursor.positionInBlock();
        int end = textBlock.indexOf(QRegExp("[^\\w'-]"), pos);
        int begin = textBlock.lastIndexOf(QRegExp("[^\\w'-]"), pos);

        if(end == -1)
        {
            end = textBlock.size();
        }

        cursor.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, pos - begin - 1);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, end - begin - 1);

        cursor.deleteChar();
        cursor.insertText(replacement);
    }
}

void spellTextEditClass::addWordToUserDic()
{
    if(spellChecker != 0 && codecUsed != 0)
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
    if(spellChecker != 0 && codecUsed != 0)
    {
        QString wordUnderCursor = getWordUnderCursor(lastPos);
        QByteArray encodedString;

        encodedString = codecUsed->fromUnicode(wordUnderCursor);
        spellChecker->add(encodedString.data());

        emit addWord(wordUnderCursor);
    }
}
