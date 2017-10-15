#ifndef UTILITYTOOL_HPP
#define UTILITYTOOL_HPP

#include <QWidget>
#include <QString>
#include <QList>
#include <QNetworkCookie>

namespace utilityTool
{
    void openLinkInBrowser(QWidget* parent, bool useInternalBrowser, const QString& linkToOpen, const QNetworkCookie& connectCookie);
    QList<QNetworkCookie> cookieToCookieList(const QNetworkCookie& connectCookie);
    int roundToInt(double number);
}

#endif
