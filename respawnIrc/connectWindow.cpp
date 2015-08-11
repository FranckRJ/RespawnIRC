#include "connectWindow.hpp"
#include "parsingTool.hpp"

connectWindowClass::connectWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labPseudo = new QLabel("Pseudo :", this);
    QLabel* labPassword = new QLabel("Mot de passe :", this);
    QLabel* labCaptcha = new QLabel("Captcha :", this);
    QPushButton* buttonConnect = new QPushButton("Se connecter", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);

    passwordLine.setEchoMode(QLineEdit::Password);
    captchaLine.setDisabled(true);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonConnect);
    buttonLayout->addWidget(buttonCancel);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(labPseudo, 0, 0);
    mainLayout->addWidget(&pseudoLine, 0, 1);
    mainLayout->addWidget(labPassword, 1, 0);
    mainLayout->addWidget(&passwordLine, 1, 1);
    mainLayout->addWidget(labCaptcha, 2, 0);
    mainLayout->addWidget(&labPixCaptcha, 2, 1);
    mainLayout->addWidget(&captchaLine, 3, 0, 1, 2);
    mainLayout->addLayout(buttonLayout, 4, 0, 1, 2);

    setLayout(mainLayout);
    setWindowTitle("Se connecter");
    pseudoLine.setFocus();

    networkManager.setCookieJar(new QNetworkCookieJar(&networkManager));
    captchaHere = false;
    reply = 0;

    connect(buttonConnect, SIGNAL(pressed()), this, SLOT(startLogin()));
    connect(buttonCancel, SIGNAL(pressed()), this, SLOT(close()));
    getLoginInfo();
}

connectWindowClass::~connectWindowClass()
{
    if(reply != 0)
    {
        reply->deleteLater();
    }
}

void connectWindowClass::getLoginInfo()
{
    QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com/login");
    reply = networkManager.get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(getFormInput()));
}

void connectWindowClass::removeCaptcha()
{
    labPixCaptcha.setPixmap(QPixmap());
    captchaLine.clear();
    captchaLine.setDisabled(true);
}

void connectWindowClass::getFormInput()
{
    QString source = reply->readAll();
    reply->deleteLater();
    reply = 0;

    if(source.contains("Combinaison pseudo / mot de passe invalide.") == true)
    {
        captchaHere = false;
        removeCaptcha();
    }

    listOfInput.clear();
    parsingToolClass::getListOfHiddenInputFromThisForm(source, "form-connect-jv", listOfInput);

    if(captchaHere == true)
    {
        if(source.isEmpty() == true)
        {
            emit newCookiesAvailable(networkManager.cookieJar()->cookiesForUrl(QUrl("http://www.jeuxvideo.com")));
            close();
            return;
        }

        QRegExp expForCaptcha("<img src=\"([^\"]*)\" alt=\"Captcha\" />");
        expForCaptcha.setMinimal(true);
        source.contains(expForCaptcha);
        QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com" + expForCaptcha.cap(1));

        reply = networkManager.get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(showCaptcha()));
    }
}

void connectWindowClass::startLogin()
{
    if(reply == 0 && listOfInput.isEmpty() == false)
    {
        QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com/login");
        QString data;

        data += "login_pseudo=" + pseudoLine.text() + "&login_password=" + passwordLine.text();

        for(int i = 0; i < listOfInput.size(); ++i)
        {
            data += "&" + listOfInput.at(i).first + "=" + listOfInput.at(i).second;
        }

        if(captchaHere == true)
        {
            data += "&fs_ccode=" + captchaLine.text();
        }

        listOfInput.clear();
        captchaHere = true;
        reply = networkManager.post(request, data.toAscii());

        connect(reply, SIGNAL(finished()), this, SLOT(getFormInput()));
    }
}

void connectWindowClass::showCaptcha()
{
    QPixmap pixmap;
    QByteArray imageData = reply->readAll();
    reply->deleteLater();
    reply = 0;
    pixmap.loadFromData(imageData);
    labPixCaptcha.setPixmap(pixmap);
    captchaLine.clear();
    captchaLine.setDisabled(false);
}
