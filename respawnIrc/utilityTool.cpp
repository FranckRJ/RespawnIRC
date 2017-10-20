#include <QDesktopServices>
#include <QUrl>

#include "utilityTool.hpp"
#include "webNavigator.hpp"

void utilityTool::openLinkInBrowser(QWidget* parent, bool useInternalBrowser, const QString& linkToOpen, const QNetworkCookie& connectCookie)
{
    if(useInternalBrowser == true)
    {
        webNavigatorClass* myWebNavigator = new webNavigatorClass(parent, linkToOpen, cookieToCookieList(connectCookie));
        myWebNavigator->exec();
    }
    else
    {
        QDesktopServices::openUrl(QUrl(linkToOpen));
    }
}

QList<QNetworkCookie> utilityTool::cookieToCookieList(const QNetworkCookie& connectCookie)
{
    if(connectCookie.value().isEmpty() == true)
    {
        return {};
    }
    else
    {
        return {connectCookie};
    }
}

//blabla inutile je sais mais c'est plus beau
int utilityTool::roundToInt(double number)
{
    return (number + 0.5);
}
