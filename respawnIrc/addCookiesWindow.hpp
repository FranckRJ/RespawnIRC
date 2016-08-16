#ifndef ADDCOOKIESWINDOW_HPP
#define ADDCOOKIESWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QString>

class addCookiesWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit addCookiesWindowClass(QWidget* parent);
public slots:
    void valideCookies();
signals:
    void newCookiesAvailable(QString helloCookie, QString connectCookie);
private:
    QLineEdit helloCookieLine;
    QLineEdit connectCookieLine;
};

#endif
