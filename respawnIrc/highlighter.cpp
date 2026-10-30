#include <QFileInfo>
#include <QByteArray>
#include <QColor>
#include <QStringList>
#include <QRegularExpression>

#include "highlighter.hpp"
#include "styleTool.hpp"
#include "pathTool.hpp"
#include "configDependentVar.hpp"

namespace
{
    const QRegularExpression expForWordSeparators(configDependentVar::expForWordSeparatorPattern + "+",
                                                  configDependentVar::expForWordSeparatorOptions);
}

highlighterClass::highlighterClass(QTextDocument* parent) : QSyntaxHighlighter(parent)
{
    spellCheckFormat.setUnderlineColor(QColor(styleTool::getColorInfo().underlineColor));
    spellCheckFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
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

    QFileInfo fileInfoForDic(pathTool::pathForReading("resources/" + spellDic + ".dic"));
    if(fileInfoForDic.exists() == false || fileInfoForDic.isReadable() == false)
    {
        spellChecker = nullptr;
        encoderUsed = QStringEncoder(QStringConverter::Utf8);
    }
    else
    {
        QFileInfo fileInfoForUserDic(pathTool::pathForReading("user_" + spellDic + ".dic"));
        spellChecker = new Hunspell(pathTool::pathForReading("resources/" + spellDic + ".aff").toStdString().c_str(),
                                    pathTool::pathForReading("resources/" + spellDic + ".dic").toStdString().c_str());

        if(fileInfoForUserDic.exists() == true && fileInfoForUserDic.isReadable() == true)
        {
            spellChecker->add_dic(fileInfoForUserDic.filePath().toLatin1());
        }
        encoderUsed = QStringEncoder(spellChecker->get_dic_encoding());
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
    if(spellChecker != nullptr && encoderUsed.isValid() == true)
    {
        spellChecker->add(QByteArray(encoderUsed(word)).toStdString());
        rehighlight();
    }
}

void highlighterClass::highlightBlock(const QString& text)
{
    spellCheck(text);
}

void highlighterClass::spellCheck(const QString& text)
{
    if(spellChecker != nullptr && encoderUsed.isValid() == true && spellCheckingIsEnabled == true)
    {
        QString simplifiedText = text.simplified();
        if(simplifiedText.isEmpty() == false)
        {
            QStringList checkList = simplifiedText.split(expForWordSeparators);
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
                        const QRegularExpression expForThisWord(R"rgx(\b)rgx" + QRegularExpression::escape(thisString) + R"rgx(\b)rgx",
                                                                configDependentVar::expForWordSeparatorOptions);
                        int wordCount = text.count(expForThisWord);
                        int index = -1;
                        for(int j = 0; j < wordCount; ++j)
                        {
                            index = text.indexOf(expForThisWord, index + 1);
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
    if(spellChecker != nullptr && encoderUsed.isValid() == true)
    {
        return spellChecker->spell(QByteArray(encoderUsed(word)).toStdString());
    }
    else
    {
        return false;
    }
}
