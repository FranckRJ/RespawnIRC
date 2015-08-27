#include "captchaWindow.hpp"
#include "parsingTool.hpp"

captchaWindowClass::captchaWindowClass(QString captchaLink, QList<QNetworkCookie> listOfCookie, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
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
    QObject::connect(reply, &QNetworkReply::finished, this, &captchaWindowClass::showCaptcha);

    QObject::connect(buttonSend, &QPushButton::pressed, this, &captchaWindowClass::sendCaptchaCode);
    QObject::connect(buttonCancel, &QPushButton::pressed, this, &captchaWindowClass::close);
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
    if(captchaCodeLine.text().isEmpty() == true)
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le captcha n'est pas valide !");
    }
    else
    {
        emit codeForCaptcha(captchaCodeLine.text());
        close();
    }
}
