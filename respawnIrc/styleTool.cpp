#include "styleTool.hpp"

colorInfoForMessageAndOtherStruct styleToolClass::colorInfoForMessageAndOther;

QString styleToolClass::getStyle(QString themeName)
{
    QFile thisFile("theme/" + themeName + "/style.css");
    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QTextStream textStream(&thisFile);
        return textStream.readAll();
    }
    else
    {
        return "";
    }
}

QString styleToolClass::getModel(QString themeName)
{
    QFile thisFile("theme/" + themeName + "/model.txt");
    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QTextStream textStream(&thisFile);
        return textStream.readAll();
    }
    else
    {
        return "<table><tr><td><%BUTTON_QUOTE%><%BUTTON_BLACKLIST%><%BUTTON_EDIT%>[<a style=\"color: <%DATE_COLOR%>;text-decoration: none\" href=\"http://www.jeuxvideo.com/"
                "<%PSEUDO_LOWER%>/forums/message/<%ID_MESSAGE%>\"><%DATE_MESSAGE%></a>] &lt;<a href=\"http://www.jeuxvideo.com/profil/"
                "<%PSEUDO_LOWER%>?mode=infos\"><span style=\"color: <%PSEUDO_COLOR%>;text-decoration: none\"><%PSEUDO_PSEUDO%>"
                "</span></a>&gt;</td><td><%MESSAGE_MESSAGE%></td></tr></table>";
    }
}

modelInfoStruct styleToolClass::getModelInfo(QString themeName)
{
    QStringList listOfInfos;
    QVector<QString> listOfLine;
    modelInfoStruct modelInfo;
    QFile thisFile("theme/" + themeName + "/modelInfo.cfg");

    listOfInfos.push_back("<a style=\"color: black;text-decoration: none\" href=\"quote:<%ID_MESSAGE%>:[<%DATE_MESSAGE%>] <<%PSEUDO_PSEUDO%>>\">[C]</a> ");
    listOfInfos.push_back("<a style=\"color: black;text-decoration: none\" href=\"blacklist:<%PSEUDO_LOWER%>\">[B]</a> ");
    listOfInfos.push_back("<a style=\"color: black;text-decoration: none\" href=\"edit:<%ID_MESSAGE%>\">[E]</a> ");
    listOfInfos.push_back("black");
    listOfInfos.push_back("green");
    listOfInfos.push_back("dimgrey");
    listOfInfos.push_back("blue");

    listOfInfos.push_back("blue");
    listOfInfos.push_back("black");
    listOfInfos.push_back("grey");
    listOfInfos.push_back("red");

    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QTextStream textStream(&thisFile);
        while(textStream.atEnd() == false)
        {
            listOfLine.push_back(textStream.readLine());
        }
    }

    while(listOfLine.size() < 11)
    {
        listOfLine.push_back(listOfInfos.at(listOfLine.size()));
    }

    modelInfo.quoteModel = listOfLine.at(0);
    modelInfo.blacklistModel = listOfLine.at(1);
    modelInfo.editModel = listOfLine.at(2);
    modelInfo.normalDateColor = listOfLine.at(3);
    modelInfo.editDateColor = listOfLine.at(4);
    modelInfo.normalPseudoColor = listOfLine.at(5);
    modelInfo.userPseudoColor = listOfLine.at(6);

    colorInfoForMessageAndOther.linkColor = listOfLine.at(7);
    colorInfoForMessageAndOther.spoilColor = listOfLine.at(8);
    colorInfoForMessageAndOther.tableBorderColor = listOfLine.at(9);
    colorInfoForMessageAndOther.underlineColor = listOfLine.at(10);

    return modelInfo;
}

const colorInfoForMessageAndOtherStruct &styleToolClass::getColorInfo()
{
    return colorInfoForMessageAndOther;
}
