#ifndef ADDPSEUDOWINDOW_HPP
#define ADDPSEUDOWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QLineEdit>
#include <QString>

class addPseudoWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit addPseudoWindowClass(QWidget* parent, QString currentPseudo = "");
    static bool pseudoIsValide(QString pseudo);
private slots:
    void setPseudo();
signals:
    void newPseudoSet(QString newPseudo);
private:
    QLineEdit* pseudoLine;
};

#endif
