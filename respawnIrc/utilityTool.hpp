#ifndef UTILITYTOOL_HPP
#define UTILITYTOOL_HPP

#include <QWidget>
#include <QString>
#include <QList>
#include <QNetworkCookie>
#include <QWebEngineCookieStore>

namespace utilityTool
{
    void openLinkInBrowser(QWidget* parent, bool useInternalBrowser, const QString& linkToOpen, const QNetworkCookie& connectCookie);
    QList<QNetworkCookie> cookieToCookieList(const QNetworkCookie& connectCookie);
    void setupWebNotifierCookie(QWebEngineCookieStore& webEngineCookieStore);
    int roundToInt(double number);
}

#endif
