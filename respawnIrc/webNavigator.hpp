#ifndef WEBNAVIGATOR_HPP
#define WEBNAVIGATOR_HPP

#include <QWidget>
#include <QDialog>
#include <QNetworkCookie>
#include <QWebEngineView>
#include <QLineEdit>
#include <QList>
#include <QString>
#include <QUrl>

class webNavigatorClass : public QDialog
{
    Q_OBJECT
public:
    webNavigatorClass(QWidget* parent, QString startUrl = "", QList<QNetworkCookie> cookiesList = QList<QNetworkCookie>());
public slots:
    void changeUrl(QUrl newUrl);
    void goToUrl();
private:
    QWebEngineView webView;
    QLineEdit urlLine;
};

#endif
