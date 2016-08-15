#ifndef CONNECTWINDOW_HPP
#define CONNECTWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QNetworkCookie>
#include <QWebEngineView>
#include <QLineEdit>
#include <QCheckBox>
#include <QList>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>

class connectWindowClass : public QDialog
{
    Q_OBJECT
public:
    connectWindowClass(QWidget* parent, bool showRemeberBox = true);
public slots:
    void addWebView();
    void checkThisCookie(QNetworkCookie cookie);
    void showAddCookiesWindow();
    void addCookiesManually(QString newHelloCookie, QString newConnectCookie);
    void valideConnect();
    void showHelpConnect();
signals:
    void newCookiesAvailable(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
private:
    QList<QNetworkCookie> cookieList;
    QWebEngineView* webView = nullptr;
    QPushButton* buttonShowWebView;
    QVBoxLayout* mainLayout;
    QLineEdit pseudoLine;
    QCheckBox rememberBox;
};

#endif
