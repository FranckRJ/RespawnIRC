#ifndef CONNECTWINDOW_HPP
#define CONNECTWINDOW_HPP

#include <QtGui>
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
    void newCookiesAvailable(QList<QNetworkCookie> newCookiek, QString newPseudoOfUser, bool saveInfo);
private:
    QLineEdit pseudoLine;
    QLineEdit passwordLine;
    QCheckBox rememberBox;
    QLineEdit captchaLine;
    QNetworkReply* reply;
    QNetworkAccessManager networkManager;
    QList<QPair<QString, QString> > listOfInput;
    QLabel labPixCaptcha;
    bool captchaHere;
};

#endif
