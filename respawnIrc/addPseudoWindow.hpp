#ifndef ADDPSEUDOWINDOW_HPP
#define ADDPSEUDOWINDOW_HPP

#include <QtWidgets>
#include <QtCore>

class addPseudoWindowClass : public QDialog
{
    Q_OBJECT
public:
    addPseudoWindowClass(QWidget* parent, QString currentPseudo = "");
    bool pseudoIsValide(QString pseudo);
public slots:
    void setPseudo();
signals:
    void newPseudoSet(QString newPseudo);
private:
    QLineEdit pseudoLine;
};

#endif
