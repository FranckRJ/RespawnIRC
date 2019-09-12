#ifndef WEBNAVIGATOR_HPP
#define WEBNAVIGATOR_HPP

#include <QWidget>
#include <QNetworkCookie>
#include <QLineEdit>
#include <QList>
#include <QString>
#include <QUrl>
#include <QProgressBar>
#include <QPushButton>

#include "baseDialog.hpp"
#include "customWebPage.hpp"
#include "customWebView.hpp"

class webNavigatorClass : public baseDialogClass
{
    Q_OBJECT
public:
    explicit webNavigatorClass(QWidget* parent, QString startUrl = "", QList<QNetworkCookie> jvcCookiesList = QList<QNetworkCookie>());
    ~webNavigatorClass();

private:
    void newPageAvailableCallback(customWebPageClass* newPage);

private slots:
    void changeUrl(QUrl newUrl);
    void handleLoadProgress(int progress);
    void goToUrl();
    void openCurrentPageInExternalNavigator() const;

private:
    customWebViewClass* webView;
    QProgressBar* webViewLoadBar;
    QPushButton* backwardButton;
    QPushButton* forwardButton;
    QLineEdit* urlLine;
};

#endif
