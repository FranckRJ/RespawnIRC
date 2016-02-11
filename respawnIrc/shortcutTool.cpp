#include "shortcutTool.hpp"

QMap<QString, QString> shortcutToolClass::listOfShortcut;

void shortcutToolClass::loadShortcuts()
{
    QFile thisFile(QCoreApplication::applicationDirPath() + "/ressources/shortcut.txt");
    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QTextStream textStream(&thisFile);

        while(textStream.atEnd() == false)
        {
            QString thisLine = textStream.readLine();
            int index = thisLine.indexOf(' ');

            if(index > 0 && index < (thisLine.size() - 1))
            {
                listOfShortcut[thisLine.left(index)] = thisLine.right(thisLine.size() - index - 1);
            }
        }
    }
}

QString shortcutToolClass::transformMessage(QString thisMessage)
{
    QMap<QString, QString>::const_iterator itShortcut = listOfShortcut.constBegin();
    while(itShortcut != listOfShortcut.constEnd())
    {
        thisMessage.replace(itShortcut.key(), itShortcut.value());
        ++itShortcut;
    }

    return thisMessage;
}
