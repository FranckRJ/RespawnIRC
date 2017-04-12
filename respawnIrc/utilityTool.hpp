#ifndef UTILITYTOOL_HPP
#define UTILITYTOOL_HPP

#include <QWidget>
#include <QString>
#include <QList>
#include <QNetworkCookie>

namespace utilityTool
{
    void openLinkInBrowser(QWidget* parent, bool useInternalBrowser, const QString& linkToOpen, const QList<QNetworkCookie>& listOfCookies);
}

#endif
