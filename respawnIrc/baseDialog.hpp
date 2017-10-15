#ifndef BASEDIALOG_HPP
#define BASEDIALOG_HPP

#include <QDialog>
#include <QWidget>
#include <QShowEvent>

class baseDialogClass : public QDialog
{
public:
    baseDialogClass(QWidget* parent);
    virtual ~baseDialogClass() {}
protected:
    void showEvent(QShowEvent* event) override;
};

#endif
