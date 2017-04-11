#ifndef WEBNAVIGATOR_HPP
#define WEBNAVIGATOR_HPP

#include <QWidget>
#include <QDialog>
#include <QNetworkCookie>
#include <QLineEdit>
#include <QList>
#include <QString>
#include <QUrl>
#include <QProgressBar>

#include "customWebView.hpp"

class webNavigatorClass : public QDialog
{
    Q_OBJECT
public:
    explicit webNavigatorClass(QWidget* parent, QString startUrl = "", QList<QNetworkCookie> cookiesList = QList<QNetworkCookie>());
private slots:
    void changeUrl(QUrl newUrl);
    void handleLoadProgress(int progress);
    void goToUrl();
private:
    customWebViewClass* webView;
    QProgressBar* webViewLoadBar;
    QPushButton* backwardButton;
    QPushButton* forwardButton;
    QLineEdit* urlLine;
};

#endif
