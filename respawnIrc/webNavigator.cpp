#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEnginePage>
#include <QWebEngineCookieStore>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>

#include "webNavigator.hpp"
#include "styleTool.hpp"
#include "utilityTool.hpp"

webNavigatorClass::webNavigatorClass(QWidget* parent, QString startUrl, QList<QNetworkCookie> jvcCookiesList) :
    baseDialogClass(parent, Qt::WindowMaximizeButtonHint)
{
    QWebEngineProfile* customProfile = new QWebEngineProfile(this);

    QMenuBar* mainMenuBar = new QMenuBar(this);
    QPushButton* goButton = new QPushButton("Go", this);
    backwardButton = new QPushButton("←", this);
    forwardButton = new QPushButton("→", this);
    urlLine = new QLineEdit(this);
    webView = new customWebViewClass(this);
    webViewLoadBar = new QProgressBar(this);

    QMenu* menuFile = mainMenuBar->addMenu("&Fichier");
    QAction* actionOpenInExternalNavigator = menuFile->addAction("Ouvrir dans le navigateur externe");

    mainMenuBar->setStyleSheet("QMenuBar {padding-top: 1px;}"); //sinon les qmenu se dessinent par-dessus la ligne du bas de la qmenubar sous W7
    backwardButton->setAutoDefault(false);
    backwardButton->setMaximumWidth(backwardButton->fontMetrics().boundingRect(backwardButton->text()).width() + 15);
    backwardButton->setEnabled(false);
    forwardButton->setAutoDefault(false);
    forwardButton->setMaximumWidth(forwardButton->fontMetrics().boundingRect(forwardButton->text()).width() + 15);
    forwardButton->setEnabled(false);
    goButton->setAutoDefault(false);
    goButton->setMaximumWidth(goButton->fontMetrics().boundingRect(goButton->text()).width() + 20);

    webView->resize(800, 600);
    webViewLoadBar->setMaximumHeight(1);
    webViewLoadBar->setTextVisible(false);
    webViewLoadBar->setStyleSheet("QProgressBar {border: 0px; background-color: transparent;} QProgressBar::chunk {background-color: " + styleTool::getColorInfo().navigatorProgressBarColor + ";}");

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(backwardButton);
    topLayout->addWidget(forwardButton);
    topLayout->addWidget(urlLine, 1);
    topLayout->addWidget(goButton);
    topLayout->setSpacing(2);
    topLayout->setMargin(2);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(mainMenuBar);
    mainLayout->addWidget(webViewLoadBar);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(webView, 1);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setWindowTitle("RespawnIRC Navigator");

    QWebEnginePage* customPage = new QWebEnginePage(customProfile, webView);
    webView->setPage(customPage);
    for(QNetworkCookie thisCookie : jvcCookiesList)
    {
        thisCookie.setDomain("www.jeuxvideo.com");
        webView->page()->profile()->cookieStore()->setCookie(thisCookie);
    }
    webView->page()->profile()->cookieStore()->setCookie(utilityTool::createWebNotifierCookie());

    if(startUrl.isEmpty() == true)
    {
        startUrl = "http://www.jeuxvideo.com";
    }

    webView->setUrl(QUrl(startUrl));

    connect(actionOpenInExternalNavigator, &QAction::triggered, this, &webNavigatorClass::openCurrentPageInExternalNavigator);
    connect(webView, &customWebViewClass::urlChanged, this, &webNavigatorClass::changeUrl);
    connect(webView, &customWebViewClass::loadProgress, this, &webNavigatorClass::handleLoadProgress);
    connect(urlLine, &QLineEdit::returnPressed, goButton, &QPushButton::click);
    connect(goButton, &QPushButton::clicked, this, &webNavigatorClass::goToUrl);
    connect(backwardButton, &QPushButton::clicked, webView, &customWebViewClass::back);
    connect(forwardButton, &QPushButton::clicked, webView, &customWebViewClass::forward);
}

webNavigatorClass::~webNavigatorClass()
{
    delete webView->page();
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

void webNavigatorClass::openCurrentPageInExternalNavigator() const
{
    QDesktopServices::openUrl(webView->url());
}
