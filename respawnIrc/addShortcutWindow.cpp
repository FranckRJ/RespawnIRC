#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "addShortcutWindow.hpp"

addShortcutWindowClass::addShortcutWindowClass(QWidget* parent, QString currentBase, QString currentReplacement) :
    baseDialogClass(parent)
{
    QLabel* baseLabel = new QLabel("Base :", this);
    baseLine = new QLineEdit(this);
    QLabel* replacementLabel = new QLabel("Remplacement :", this);
    replacementLine = new QLineEdit(this);
    QPushButton* validateButton = new QPushButton("Valider", this);
    QPushButton* cancelButton = new QPushButton("Annuler", this);

    baseLine->setText(currentBase);
    replacementLine->setText(currentReplacement);

    QHBoxLayout* linesLayout = new QHBoxLayout();
    linesLayout->addWidget(baseLabel);
    linesLayout->addWidget(baseLine);
    linesLayout->addWidget(replacementLabel);
    linesLayout->addWidget(replacementLine);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(validateButton);
    buttonsLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(linesLayout);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
    setWindowTitle((currentBase.isEmpty() == true ? "Ajouter un raccourci" : "Éditer un raccourci"));
    baseLine->setFocus();

    connect(validateButton, &QPushButton::clicked, this, &addShortcutWindowClass::setShortcut);
    connect(cancelButton, &QPushButton::clicked, this, &addShortcutWindowClass::close);
}

void addShortcutWindowClass::setShortcut()
{
    if(baseLine->text().isEmpty() == true || baseLine->text().contains(' ') == true)
    {
        QMessageBox::warning(this, "Erreur", "La base ne doit ni être vide ni contenir d'espaces !");
    }
    else
    {
        emit newShortcutSet(baseLine->text(), replacementLine->text());
        close();
    }
}
