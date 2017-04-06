#ifndef ADDSHORTCUTWINDOW_HPP
#define ADDSHORTCUTWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QString>

class addShortcutWindowClass : public QDialog
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
