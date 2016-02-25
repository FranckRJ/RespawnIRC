#ifndef ADDCOOKIESWINDOW_HPP
#define ADDCOOKIESWINDOW_HPP

#include <QtWidgets>
#include <QtCore>

class addCookiesWindowClass : public QDialog
{
    Q_OBJECT
public:
    addCookiesWindowClass(QWidget* parent);
public slots:
    void valideCookies();
signals:
    void newCookiesAvailable(QString helloCookie, QString connectCookie);
private:
    QLineEdit helloCookieLine;
    QLineEdit connectCookieLine;
};

#endif
