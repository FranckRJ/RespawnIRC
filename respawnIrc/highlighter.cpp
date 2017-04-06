#include <QFileInfo>
#include <QCoreApplication>
#include <QByteArray>
#include <QColor>
#include <QStringList>
#include <QRegExp>

#include "highlighter.hpp"
#include "styleTool.hpp"

highlighterClass::highlighterClass(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    spellCheckFormat.setUnderlineColor(QColor(styleTool::getColorInfo().underlineColor));
    spellCheckFormat.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    setDic("");
}

highlighterClass::~highlighterClass()
{
    if(spellChecker != nullptr)
    {
        delete spellChecker;
    }
}

void highlighterClass::enableSpellChecking(const bool state)
{
    bool old = spellCheckingIsEnabled;

    spellCheckingIsEnabled = state;

    if(old != spellCheckingIsEnabled)
    {
        rehighlight();
    }
}

bool highlighterClass::setDic(const QString newSpellDic)
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
        codec = QTextCodec::codecForName("UTF-8");
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
        codec = QTextCodec::codecForName(QString(spellChecker->get_dic_encoding()).toLatin1());
    }

    rehighlight();

    if(spellChecker == nullptr)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void highlighterClass::styleChanged()
{
    spellCheckFormat.setUnderlineColor(QColor(styleTool::getColorInfo().underlineColor));
    rehighlight();
}

void highlighterClass::addWordToDic(QString word)
{
    if(spellChecker != nullptr && codec != nullptr)
    {
        QByteArray encodedString;
        encodedString = codec->fromUnicode(word);
        spellChecker->add(encodedString.data());
        rehighlight();
    }
}

void highlighterClass::highlightBlock(const QString& text)
{
    spellCheck(text);
}

void highlighterClass::spellCheck(const QString& text)
{
    if(spellChecker != nullptr && codec != nullptr && spellCheckingIsEnabled == true)
    {
        QString simplifiedText = text.simplified();
        if(simplifiedText.isEmpty() == false)
        {
            QStringList checkList = simplifiedText.split(QRegExp(R"rgx([^\w'-]+)rgx"), QString::SkipEmptyParts);
            for(QString thisString : checkList)
            {
                while(thisString.startsWith('\'') == true || thisString.startsWith('-') == true)
                {
                    thisString.remove(0, 1);
                }
                while(thisString.endsWith('\'') == true || thisString.endsWith('-') == true)
                {
                    thisString.remove(thisString.size() - 1, 1);
                }

                if(thisString.size() > 1)
                {
                    if(checkWord(thisString) == false)
                    {
                        int wordCount;
                        int index = -1;
                        wordCount = text.count(QRegExp(R"rgx(\b)rgx" + thisString + R"rgx(\b)rgx"));
                        for(int j = 0; j < wordCount; ++j)
                        {
                            index = text.indexOf(QRegExp(R"rgx(\b)rgx" + thisString + R"rgx(\b)rgx"), index + 1);
                            if(index >= 0)
                            {
                                setFormat(index, thisString.size(), spellCheckFormat);
                            }
                        }
                    }
                }
            }
        }
    }
}

bool highlighterClass::checkWord(QString word)
{
    if(spellChecker != nullptr && codec != nullptr)
    {
        return spellChecker->spell(codec->fromUnicode(word).data());
    }
    else
    {
        return false;
    }
}
