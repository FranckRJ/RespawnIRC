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

    QPushButton* backwardButton = new QPushButton("←", this);
    QPushButton* forwardButton = new QPushButton("→", this);
    QPushButton* goButton = new QPushButton("Go", this);
    urlLine = new QLineEdit(this);
    webView = new QWebEngineView(this);

    backwardButton->setAutoDefault(false);
    backwardButton->setMaximumWidth(backwardButton->fontMetrics().boundingRect(backwardButton->text()).width() + 15);
    forwardButton->setAutoDefault(false);
    forwardButton->setMaximumWidth(forwardButton->fontMetrics().boundingRect(forwardButton->text()).width() + 15);
    goButton->setAutoDefault(false);
    goButton->setMaximumWidth(goButton->fontMetrics().boundingRect(goButton->text()).width() + 20);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(backwardButton);
    topLayout->addWidget(forwardButton);
    topLayout->addWidget(urlLine, 1);
    topLayout->addWidget(goButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(webView);

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

    connect(webView, &QWebEngineView::urlChanged, this, &webNavigatorClass::changeUrl);
    connect(urlLine, &QLineEdit::returnPressed, goButton, &QPushButton::click);
    connect(goButton, &QPushButton::clicked, this, &webNavigatorClass::goToUrl);
    connect(backwardButton, &QPushButton::clicked, webView, &QWebEngineView::back);
    connect(forwardButton, &QPushButton::clicked, webView, &QWebEngineView::forward);
}

void webNavigatorClass::changeUrl(QUrl newUrl)
{
    urlLine->setText(newUrl.toString());
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
