#ifndef CONNECTWINDOW_HPP
#define CONNECTWINDOW_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>
#include <QWebView>

class connectWindowClass : public QDialog
{
    Q_OBJECT
public:
    connectWindowClass(QWidget* parent, bool showRemeberBox = true);
public slots:
    void newPageLoaded(QNetworkReply* reply);
    void valideConnect();
signals:
    void newCookiesAvailable(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
private:
    QList<QNetworkCookie> cookieList;
    QWebView* webView;
    QLineEdit pseudoLine;
    bool saveThisAccount;
};
#endif
