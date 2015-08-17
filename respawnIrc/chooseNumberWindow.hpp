#ifndef CHOOSENUMBERWINDOW_HPP
#define CHOOSENUMBERWINDOW_HPP

#include <QtGui>
#include <QtCore>

class chooseNumberWindowClass : public QDialog
{
    Q_OBJECT
public:
    chooseNumberWindowClass(int minNumber, int maxNumber, int currentNumber, QWidget* parent);
public slots:
    void setNumber();
signals:
    void newNumberSet(int newNumber);
private:
    QSpinBox numberBox;
};

#endif
