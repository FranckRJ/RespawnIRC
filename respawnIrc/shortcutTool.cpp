#include <QFile>
#include <QMap>
#include <QTextStream>
#include <QCoreApplication>

#include "shortcutTool.hpp"
#include "configDependentVar.hpp"

namespace
{
    QMap<QString, shortcutRuleStruct> listOfShortcutRules;
}

void shortcutToolClass::loadShortcutRule(QString ruleName, QString beforeBase, QString afterBase, QString beforeNew, QString afterNew, bool useRegex)
{
    QFile thisFile(QCoreApplication::applicationDirPath() + "/resources/" + ruleName + ".txt");

    QMap<QString, shortcutRuleStruct>::iterator rulesIte = listOfShortcutRules.insert(ruleName, shortcutRuleStruct());

    rulesIte.value().needToUseRegex = useRegex;

    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QTextStream textStream(&thisFile);

        while(textStream.atEnd() == false)
        {
            QString thisLine = textStream.readLine();
            int index = thisLine.indexOf(' ');

            if(index > 0 && index < (thisLine.size() - 1))
            {
                shortcutInfosStruct newShortcutInfos;

                if(rulesIte.value().needToUseRegex == false)
                {
                    newShortcutInfos.base = beforeBase + thisLine.left(index) + afterBase;
                }
                else
                {
                    newShortcutInfos.regexBase = QRegularExpression(beforeBase + thisLine.left(index) + afterBase, configDependentVar::regexpBaseOptions);
                }

                newShortcutInfos.replacement = beforeNew + thisLine.right(thisLine.size() - index - 1) + afterNew;
                rulesIte.value().listOfShortcuts.push_back(newShortcutInfos);
            }
        }
    }
}

void shortcutToolClass::transformMessage(QString* thisMessage, QString ruleName)
{
    if(thisMessage != nullptr)
    {
        *thisMessage = transformMessage(*thisMessage, ruleName);
    }
}

QString shortcutToolClass::transformMessage(QString thisMessage, QString ruleName)
{
    QMap<QString, shortcutRuleStruct>::iterator rulesIte = listOfShortcutRules.find(ruleName);

    if(rulesIte != listOfShortcutRules.end())
    {
        QList<shortcutInfosStruct>::const_iterator itShortcut = rulesIte.value().listOfShortcuts.constBegin();
        while(itShortcut != rulesIte.value().listOfShortcuts.constEnd())
        {
            if(rulesIte->needToUseRegex == false)
            {
                thisMessage.replace(itShortcut->base, itShortcut->replacement);
            }
            else
            {
                thisMessage.replace(itShortcut->regexBase, itShortcut->replacement);
            }
            ++itShortcut;
        }
    }

    return thisMessage;
}
