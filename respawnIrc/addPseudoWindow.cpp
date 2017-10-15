#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QRegExp>

#include "addPseudoWindow.hpp"

addPseudoWindowClass::addPseudoWindowClass(QWidget* parent, QString currentPseudo) : baseDialogClass(parent)
{
    QLabel* labPseudo = new QLabel("Pseudo :", this);
    QPushButton* buttonValide = new QPushButton("Valider", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);
    pseudoLine = new QLineEdit(this);

    pseudoLine->setText(currentPseudo);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonValide);
    buttonLayout->addWidget(buttonCancel);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(labPseudo, 0, 0);
    mainLayout->addWidget(pseudoLine, 0, 1);
    mainLayout->addLayout(buttonLayout, 1, 0, 1, 2);

    setLayout(mainLayout);
    setWindowTitle("Choisir un pseudo");
    pseudoLine->setFocus();

    connect(buttonValide, &QPushButton::clicked, this, &addPseudoWindowClass::setPseudo);
    connect(buttonCancel, &QPushButton::clicked, this, &addPseudoWindowClass::close);
}

bool addPseudoWindowClass::pseudoIsValide(QString pseudo)
{
    if(pseudo.size() > 15 || pseudo.size() < 3)
    {
        return false;
    }

    if(pseudo.contains(QRegExp(R"rgx([^a-zA-Z0-9_\[\]-]+)rgx")) == true)
    {
        return false;
    }

    return true;
}

void addPseudoWindowClass::setPseudo()
{
    if(pseudoIsValide(pseudoLine->text()) == true)
    {
        emit newPseudoSet(pseudoLine->text());
        close();
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Le pseudo n'est pas valide !");
    }
}
