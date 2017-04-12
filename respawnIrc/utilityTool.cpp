#include <QDesktopServices>
#include <QUrl>

#include "utilityTool.hpp"
#include "webNavigator.hpp"

void utilityTool::openLinkInBrowser(QWidget* parent, bool useInternalBrowser, const QString& linkToOpen, const QList<QNetworkCookie>& listOfCookies)
{
    if(useInternalBrowser == true)
    {
        webNavigatorClass* myWebNavigator = new webNavigatorClass(parent, linkToOpen, listOfCookies);
        myWebNavigator->exec();
    }
    else
    {
        QDesktopServices::openUrl(QUrl(linkToOpen));
    }
}
