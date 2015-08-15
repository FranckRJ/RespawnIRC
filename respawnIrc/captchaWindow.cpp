#include "captchaWindow.hpp"
#include "parsingTool.hpp"

captchaWindowClass::captchaWindowClass(QString captchaLink, QList<QNetworkCookie> listOfCookie, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labCaptcha = new QLabel("Captcha :", this);
    QPushButton* buttonSend = new QPushButton("Envoyer", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonSend);
    buttonLayout->addWidget(buttonCancel);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(labCaptcha, 0, 0);
    mainLayout->addWidget(&labPixCaptcha, 0, 1);
    mainLayout->addWidget(&captchaCodeLine, 1, 0, 1, 2);
    mainLayout->addLayout(buttonLayout, 2, 0, 1, 2);

    setLayout(mainLayout);
    setWindowTitle("Entrer un captcha");
    captchaCodeLine.setFocus();

    networkManager.setCookieJar(new QNetworkCookieJar(&networkManager));
    networkManager.cookieJar()->setCookiesFromUrl(listOfCookie, QUrl("http://www.jeuxvideo.com"));

    reply = networkManager.get(parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com" + captchaLink));
    connect(reply, SIGNAL(finished()), this, SLOT(showCaptcha()));

    connect(buttonSend, SIGNAL(pressed()), this, SLOT(sendCaptchaCode()));
    connect(buttonCancel, SIGNAL(pressed()), this, SLOT(close()));
}

captchaWindowClass::~captchaWindowClass()
{
    if(reply != 0)
    {
        reply->deleteLater();
    }
}

void captchaWindowClass::showCaptcha()
{
    QPixmap pixmap;
    QByteArray imageData = reply->readAll();
    reply->deleteLater();
    reply = 0;
    pixmap.loadFromData(imageData);
    labPixCaptcha.setPixmap(pixmap);
}

void captchaWindowClass::sendCaptchaCode()
{
    emit codeForCaptcha(captchaCodeLine.text());
    close();
}
