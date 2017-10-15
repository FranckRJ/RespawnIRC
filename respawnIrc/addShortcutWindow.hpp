#ifndef ADDSHORTCUTWINDOW_HPP
#define ADDSHORTCUTWINDOW_HPP

#include <QWidget>
#include <QLineEdit>
#include <QString>

#include "baseDialog.hpp"

class addShortcutWindowClass : public baseDialogClass
{
    Q_OBJECT
public:
    explicit addShortcutWindowClass(QWidget* parent, QString currentBase = "", QString currentReplacement = "");
private slots:
    void setShortcut();
signals:
    void newShortcutSet(QString newBase, QString newReplacement);
private:
    QLineEdit* baseLine;
    QLineEdit* replacementLine;
};

#endif
