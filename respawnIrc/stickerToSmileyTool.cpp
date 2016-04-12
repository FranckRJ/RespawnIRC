#include "stickerToSmileyTool.hpp"

QMap<QString, QString> stickerToSmileyToolClass::listOfTransform;

void stickerToSmileyToolClass::loadTransformInfo()
{
    QFile thisFile(QCoreApplication::applicationDirPath() + "/resources/stickerToSmiley.txt");
    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QTextStream textStream(&thisFile);

        while(textStream.atEnd() == false)
        {
            QString thisLine = textStream.readLine();
            int index = thisLine.indexOf(' ');

            if(index > 0 && index < (thisLine.size() - 1))
            {
                listOfTransform[thisLine.left(index)] = thisLine.right(thisLine.size() - index - 1);
            }
        }
    }
}

void stickerToSmileyToolClass::transformMessage(QString& thisMessage)
{
    QMap<QString, QString>::const_iterator itTransform = listOfTransform.constBegin();
    while(itTransform != listOfTransform.constEnd())
    {
        thisMessage.replace("<img class=\"img-stickers\" src=\"http://jv.stkr.fr/p/" +
                            itTransform.key() + "\"/>", "<img src=\"resources/smileys/" + itTransform.value() + "\"/>");
        ++itTransform;
    }
}
