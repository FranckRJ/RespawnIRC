#ifndef BASEDIALOG_HPP
#define BASEDIALOG_HPP

#include <QDialog>
#include <QWidget>
#include <QShowEvent>

class baseDialogClass : public QDialog
{
public:
    baseDialogClass(QWidget* parent, Qt::WindowFlags newWindowFlags = Qt::WindowFlags());
    virtual ~baseDialogClass() {}
protected:
    void showEvent(QShowEvent* event) override;
private:
    bool firstTimeDialogIsShowed = true;
};

#endif
