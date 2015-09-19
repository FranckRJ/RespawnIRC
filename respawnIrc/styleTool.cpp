#include "styleTool.hpp"

QMap<QString, QString> styleToolClass::listOfStyle;

void styleToolClass::loadStyles()
{
    QDir themeDir("theme");
    if(themeDir.exists() == true)
    {
        QList<QString> listOfFiles = themeDir.entryList(QDir::Files);

        for(int i = 0; i < listOfFiles.size(); ++i)
        {
            if(listOfFiles.at(i).right(4) == ".css")
            {
                QFile thisFile(themeDir.dirName() + "/" + listOfFiles.at(i));
                if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
                {
                    QTextStream textStream(&thisFile);
                    listOfStyle[listOfFiles.at(i)] = textStream.readAll();
                }
            }
        }
    }
}

QString styleToolClass::getStyle(QString styleName)
{
    QMap<QString, QString>::iterator iteForList = listOfStyle.find(styleName);

    if(iteForList != listOfStyle.end())
    {
        return iteForList.value();
    }
    else
    {
        return "";
    }
}
