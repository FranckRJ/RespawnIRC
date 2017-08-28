#ifndef ADDCOOKIEWINDOW_HPP
#define ADDCOOKIEWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QString>

class addCookieWindowClass : public QDialog
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
