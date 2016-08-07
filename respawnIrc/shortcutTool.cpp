#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

#include "shortcutTool.hpp"

QMap<QString, QMap<QString, QString> > shortcutToolClass::listOfShortcutRules;

void shortcutToolClass::loadShortcutRule(QString ruleName, QString beforeBase, QString afterBase, QString beforeNew, QString afterNew)
{
    QFile thisFile(QCoreApplication::applicationDirPath() + "/resources/" + ruleName + ".txt");

    QMap<QString, QMap<QString, QString> >::iterator rulesIte = listOfShortcutRules.insert(ruleName, QMap<QString, QString>());

    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QTextStream textStream(&thisFile);

        while(textStream.atEnd() == false)
        {
            QString thisLine = textStream.readLine();
            int index = thisLine.indexOf(' ');

            if(index > 0 && index < (thisLine.size() - 1))
            {
                rulesIte.value().insert(beforeBase + thisLine.left(index) + afterBase, beforeNew + thisLine.right(thisLine.size() - index - 1) + afterNew);
            }
        }
    }
}

QString& shortcutToolClass::transformMessage(QString& thisMessage, QString ruleName)
{
    QMap<QString, QMap<QString, QString> >::iterator rulesIte = listOfShortcutRules.find(ruleName);

    if(rulesIte != listOfShortcutRules.end())
    {
        QMap<QString, QString>::const_iterator itShortcut = rulesIte.value().constBegin();
        while(itShortcut != rulesIte.value().constEnd())
        {
            thisMessage.replace(itShortcut.key(), itShortcut.value());
            ++itShortcut;
        }
    }

    return thisMessage;
}
