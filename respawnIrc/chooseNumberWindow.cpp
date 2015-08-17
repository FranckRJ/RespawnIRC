#include "chooseNumberWindow.hpp"

chooseNumberWindowClass::chooseNumberWindowClass(int minNumber, int maxNumber, int currentNumber, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labNumber = new QLabel("Nombre :", this);
    QPushButton* buttonValide = new QPushButton("Valider", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);

    numberBox.setMinimum(minNumber);
    numberBox.setMaximum(maxNumber);
    numberBox.setValue(currentNumber);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonValide);
    buttonLayout->addWidget(buttonCancel);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(labNumber, 0, 0);
    mainLayout->addWidget(&numberBox, 0, 1);
    mainLayout->addLayout(buttonLayout, 1, 0, 1, 2);

    setLayout(mainLayout);
    setWindowTitle("Choisir un nombre");
    numberBox.setFocus();

    connect(buttonValide, SIGNAL(pressed()), this, SLOT(setNumber()));
    connect(buttonCancel, SIGNAL(pressed()), this, SLOT(close()));
}

void chooseNumberWindowClass::setNumber()
{
    emit newNumberSet(numberBox.value());
    close();
}

