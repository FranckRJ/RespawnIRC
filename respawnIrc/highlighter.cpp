#include "highlighter.hpp"
#include "styleTool.hpp"

highlighterClass::highlighterClass(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    spellCheckFormat.setUnderlineColor(QColor(styleToolClass::getColorInfo().underlineColor));
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
    bool old = spellCheckActive;

    if(spellerError == false)
    {
        spellCheckActive = state;
    }

    if(old != spellCheckActive)
    {
        rehighlight();
    }
}

bool highlighterClass::setDic(const QString newSpellDic)
{
    spellerError = false;
    spellDic = newSpellDic;

    if(spellChecker != nullptr)
    {
        delete spellChecker;
    }

    QFileInfo fileInfoForDic(QCoreApplication::applicationDirPath() + "/resources/" + spellDic + ".dic");
    if(fileInfoForDic.exists() == false || fileInfoForDic.isReadable() == false)
    {
        spellChecker = nullptr;
        spellCheckActive = false;
        spellerError = true;
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
    spellCheckFormat.setUnderlineColor(QColor(styleToolClass::getColorInfo().underlineColor));
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

void highlighterClass::spellCheck(const QString &text)
{
    if(spellCheckActive == true)
    {
        QString simplifiedText = text.simplified();
        if(simplifiedText.isEmpty() == false)
        {
            QStringList checkList = simplifiedText.split(QRegExp("[^\\w'-]+"), QString::SkipEmptyParts);
            for(const QString& thisString : checkList)
            {
                simplifiedText = thisString;
                if(simplifiedText.length() > 1)
                {
                    if(checkWord(simplifiedText) == false)
                    {
                        int wordCount;
                        int index = -1;
                        wordCount = text.count(QRegExp("\\b" + simplifiedText + "\\b"));
                        for(int j = 0; j < wordCount; ++j)
                        {
                            index = text.indexOf(QRegExp("\\b" + simplifiedText + "\\b"), index + 1);
                            if(index >= 0)
                            {
                                setFormat(index, simplifiedText.length(), spellCheckFormat);
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
