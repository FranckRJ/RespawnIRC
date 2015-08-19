#ifndef CONNECTWINDOW_HPP
#define CONNECTWINDOW_HPP

#include <QtWidgets>
#include <QtCore>
#include <QtNetwork>

class connectWindowClass : public QDialog
{
    Q_OBJECT
public:
    connectWindowClass(QWidget* parent);
    ~connectWindowClass();
    void getLoginInfo();
    void removeCaptcha();
public slots:
    void getFormInput();
    void startLogin();
    void showCaptcha();
signals:
    void newCookiesAvailable(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo);
private:
    QLineEdit pseudoLine;
    QLineEdit passwordLine;
    QCheckBox rememberBox;
    QLineEdit captchaLine;
    QNetworkReply* reply;
    QNetworkAccessManager networkManager;
    QList<QNetworkCookie> listOfCookieFromLastReply;
    QList<QPair<QString, QString> > listOfInput;
    QLabel labPixCaptcha;
    bool captchaHere;
};
#endif
