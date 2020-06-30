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

void utilityTool::setupWebNotifierCookie(QWebEngineCookieStore& webEngineCookieStore)
{
    {
        QNetworkCookie cookie("_cmpQcif3pcsupported", "1");
        cookie.setDomain("www.jeuxvideo.com");
        cookie.setPath("/");
        cookie.setHttpOnly(false);
        cookie.setSecure(true);
        cookie.setExpirationDate(QDateTime());
        webEngineCookieStore.setCookie(cookie);
    }
    {
        QNetworkCookie cookie("_gcl_au", "1.1.68642818.1593516826");
        cookie.setDomain("jeuxvideo.com");
        cookie.setPath("/");
        cookie.setHttpOnly(false);
        cookie.setSecure(false);
        cookie.setExpirationDate(QDateTime::currentDateTime().addMonths(3));
        webEngineCookieStore.setCookie(cookie);
    }
    {
        QNetworkCookie cookie("euconsent", "BO1z189O1z189AKAiCENDQAAAAAweAAA");
        cookie.setDomain("www.jeuxvideo.com");
        cookie.setPath("/");
        cookie.setHttpOnly(false);
        cookie.setSecure(true);
        cookie.setExpirationDate(QDateTime::currentDateTime().addYears(1).addMonths(1));
        webEngineCookieStore.setCookie(cookie);
    }
    {
        QNetworkCookie cookie("googlepersonalization", "O1z189O1z189AA");
        cookie.setDomain("www.jeuxvideo.com");
        cookie.setPath("/");
        cookie.setHttpOnly(false);
        cookie.setSecure(true);
        cookie.setExpirationDate(QDateTime::currentDateTime().addYears(1).addMonths(1));
        webEngineCookieStore.setCookie(cookie);
    }
    {
        QNetworkCookie cookie("noniabvendorconsent", "O1z189O1z189AKAiAA8AAA");
        cookie.setDomain("www.jeuxvideo.com");
        cookie.setPath("/");
        cookie.setHttpOnly(false);
        cookie.setSecure(true);
        cookie.setExpirationDate(QDateTime::currentDateTime().addYears(1).addMonths(1));
        webEngineCookieStore.setCookie(cookie);
    }
    {
        QNetworkCookie cookie("visitor_country", "FR");
        cookie.setDomain("www.jeuxvideo.com");
        cookie.setPath("/");
        cookie.setHttpOnly(false);
        cookie.setSecure(false);
        cookie.setExpirationDate(QDateTime::currentDateTime().addYears(1));
        webEngineCookieStore.setCookie(cookie);
    }
}

//blabla inutile je sais mais c'est plus beau
int utilityTool::roundToInt(double number)
{
    return static_cast<int>(number + 0.5);
}
