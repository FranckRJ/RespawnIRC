#include "chooseNumberWindow.hpp"

chooseNumberWindowClass::chooseNumberWindowClass(int minNumber, int maxNumber, int currentNumber, QString newOptionName, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labNumber = new QLabel("Nombre (entre " + QString::number(minNumber) + " et " + QString::number(maxNumber) + ") :", this);
    QPushButton* buttonValide = new QPushButton("Valider", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);

    numberBox.setMinimum(minNumber);
    numberBox.setMaximum(maxNumber);
    numberBox.setValue(currentNumber);
    optionName = newOptionName;

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

    QObject::connect(buttonValide, &QPushButton::pressed, this, &chooseNumberWindowClass::setNumber);
    QObject::connect(buttonCancel, &QPushButton::pressed, this, &chooseNumberWindowClass::close);
}

void chooseNumberWindowClass::setNumber()
{
    emit newNumberSet(numberBox.value(), optionName);
    close();
}

