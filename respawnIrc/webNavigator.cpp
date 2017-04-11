#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWebEngineProfile>
#include <QWebEngineCookieStore>

#include "webNavigator.hpp"

webNavigatorClass::webNavigatorClass(QWidget* parent, QString startUrl, QList<QNetworkCookie> cookiesList) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QWebEngineProfile* customProfile = new QWebEngineProfile(this);
    QWebEnginePage* customPage = new QWebEnginePage(customProfile, this);

    QPushButton* goButton = new QPushButton("Go", this);
    backwardButton = new QPushButton("←", this);
    forwardButton = new QPushButton("→", this);
    urlLine = new QLineEdit(this);
    webView = new customWebViewClass(this);
    webViewLoadBar = new QProgressBar(this);

    backwardButton->setAutoDefault(false);
    backwardButton->setMaximumWidth(backwardButton->fontMetrics().boundingRect(backwardButton->text()).width() + 15);
    backwardButton->setEnabled(false);
    forwardButton->setAutoDefault(false);
    forwardButton->setMaximumWidth(forwardButton->fontMetrics().boundingRect(forwardButton->text()).width() + 15);
    forwardButton->setEnabled(false);
    goButton->setAutoDefault(false);
    goButton->setMaximumWidth(goButton->fontMetrics().boundingRect(goButton->text()).width() + 20);

    webViewLoadBar->setMaximumHeight(1);
    webViewLoadBar->setTextVisible(false);
    webViewLoadBar->setStyleSheet("QProgressBar {border: 0px;} QProgressBar::chunk {background-color: blue;}");

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(backwardButton);
    topLayout->addWidget(forwardButton);
    topLayout->addWidget(urlLine, 1);
    topLayout->addWidget(goButton);
    topLayout->setMargin(2);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(webViewLoadBar);
    mainLayout->addWidget(webView);
    mainLayout->setSpacing(1);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setWindowTitle("RespawnIRC Navigator");

    webView->setPage(customPage);
    for(const QNetworkCookie& thisCookie : cookiesList)
    {
        webView->page()->profile()->cookieStore()->setCookie(thisCookie);
    }

    if(startUrl.isEmpty() == true)
    {
        startUrl = "http://www.jeuxvideo.com";
    }

    webView->setUrl(QUrl(startUrl));

    connect(webView, &customWebViewClass::urlChanged, this, &webNavigatorClass::changeUrl);
    connect(webView, &customWebViewClass::loadProgress, this, &webNavigatorClass::handleLoadProgress);
    connect(urlLine, &QLineEdit::returnPressed, goButton, &QPushButton::click);
    connect(goButton, &QPushButton::clicked, this, &webNavigatorClass::goToUrl);
    connect(backwardButton, &QPushButton::clicked, webView, &customWebViewClass::back);
    connect(forwardButton, &QPushButton::clicked, webView, &customWebViewClass::forward);
}

void webNavigatorClass::changeUrl(QUrl newUrl)
{
    urlLine->setText(newUrl.toString());
    backwardButton->setEnabled(webView->canGoBack());
    forwardButton->setEnabled(webView->canGoForward());
}

void webNavigatorClass::handleLoadProgress(int progress)
{
    webViewLoadBar->setValue(progress < 100 ? progress : 0);
}

void webNavigatorClass::goToUrl()
{
    QString currentUrlToGo = urlLine->text();

    if(currentUrlToGo.startsWith("http://") == false && currentUrlToGo.startsWith("https://") == false)
    {
        currentUrlToGo = "http://" + currentUrlToGo;
    }

    webView->setUrl(QUrl(currentUrlToGo));
}
