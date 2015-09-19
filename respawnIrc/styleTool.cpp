#include "styleTool.hpp"

QString styleToolClass::getStyle(QString themeName)
{
    QFile thisFile("theme/" + themeName + ".css");
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
    QFile thisFile("theme/" + themeName + ".txt");
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
    modelInfoStruct modelInfo;
    QFile thisFile("theme/" + themeName + ".cfg");
    if(thisFile.open(QFile::ReadOnly | QFile::Text) == true)
    {
        QVector<QString> listOfLine;
        QTextStream textStream(&thisFile);
        while(textStream.atEnd() == false)
        {
            listOfLine.push_back(textStream.readLine());
        }

        if(listOfLine.size() >= 7)
        {
            modelInfo.quoteModel = listOfLine.at(0);
            modelInfo.blacklistModel = listOfLine.at(1);
            modelInfo.editModel = listOfLine.at(2);
            modelInfo.normalDateColor = listOfLine.at(3);
            modelInfo.editDateColor = listOfLine.at(4);
            modelInfo.normalPseudoColor = listOfLine.at(5);
            modelInfo.userPseudoColor = listOfLine.at(6);
            return modelInfo;
        }
    }

    modelInfo.quoteModel = "<a style=\"color: black;text-decoration: none\" href=\"quote:<%ID_MESSAGE%>:[<%DATE_MESSAGE%>] <<%PSEUDO_PSEUDO%>>\">[C]</a> ";
    modelInfo.blacklistModel = "<a style=\"color: black;text-decoration: none\" href=\"blacklist:<%PSEUDO_LOWER%>\">[B]</a> ";
    modelInfo.editModel = "<a style=\"color: black;text-decoration: none\" href=\"edit:<%ID_MESSAGE%>\">[E]</a> ";
    modelInfo.normalDateColor = "black";
    modelInfo.editDateColor = "green";
    modelInfo.normalPseudoColor = "dimgrey";
    modelInfo.userPseudoColor = "blue";

    return modelInfo;
}
