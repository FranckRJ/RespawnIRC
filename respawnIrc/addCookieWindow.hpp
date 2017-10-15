#ifndef ADDCOOKIEWINDOW_HPP
#define ADDCOOKIEWINDOW_HPP

#include <QWidget>
#include <QLineEdit>
#include <QString>

#include "baseDialog.hpp"

class addCookieWindowClass : public baseDialogClass
{
    Q_OBJECT
public:
    explicit addCookieWindowClass(QWidget* parent);
private slots:
    void valideCookie();
signals:
    void newCookieAvailable(QString connectCookie);
private:
    QLineEdit* connectCookieLine;
};

#endif
