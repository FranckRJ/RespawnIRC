#include "colorPseudoListWindow.hpp"
#include "addPseudoWindow.hpp"

colorPseudoListWindowClass::colorPseudoListWindowClass(QList<pseudoWithColorStruct> *newListOfColorPseudo, QWidget *parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labRedBox = new QLabel("Rouge :", this);
    QLabel* labGreenBox = new QLabel("Vert :", this);
    QLabel* labBlueBox = new QLabel("Bleu :", this);
    QPushButton* buttonAddPseudo = new QPushButton("Ajouter", this);
    QPushButton* buttonEditPseudo = new QPushButton("Editer", this);
    QPushButton* buttonRemovePseudo = new QPushButton("Supprimer", this);
    QPushButton* buttonValidate = new QPushButton("Valide la couleur", this);

    redBox.setMinimum(0);
    redBox.setMaximum(255);
    greenBox.setMinimum(0);
    greenBox.setMaximum(255);
    blueBox.setMinimum(0);
    blueBox.setMaximum(255);
    viewListOfColorPseudo.setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* layoutForSpinBox = new QHBoxLayout;
    layoutForSpinBox->addWidget(labRedBox);
    layoutForSpinBox->addWidget(&redBox);
    layoutForSpinBox->addWidget(labGreenBox);
    layoutForSpinBox->addWidget(&greenBox);
    layoutForSpinBox->addWidget(labBlueBox);
    layoutForSpinBox->addWidget(&blueBox);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&viewListOfColorPseudo, 0, 0, 1, 3);
    mainLayout->addLayout(layoutForSpinBox, 1, 0, 1, 3);
    mainLayout->addWidget(buttonAddPseudo, 2, 0);
    mainLayout->addWidget(buttonEditPseudo, 2, 1);
    mainLayout->addWidget(buttonRemovePseudo, 2, 2);
    mainLayout->addWidget(buttonValidate, 3, 0, 1, 3);

    setLayout(mainLayout);
    setWindowTitle("Liste des pseudo avec couleur");

    listOfColorPseudo = newListOfColorPseudo;
    updateList();
    viewListOfColorPseudo.setModel(&modelForListView);

    QObject::connect(buttonAddPseudo, &QPushButton::pressed, this, &colorPseudoListWindowClass::addPseudo);
    QObject::connect(buttonEditPseudo, &QPushButton::pressed, this, &colorPseudoListWindowClass::editCurrentPseudo);
    QObject::connect(buttonRemovePseudo, &QPushButton::pressed, this, &colorPseudoListWindowClass::removeCurrentPseudo);
    QObject::connect(&viewListOfColorPseudo, &QListView::activated, this, &colorPseudoListWindowClass::itemSelectedHasChanged);
    QObject::connect(buttonValidate, &QPushButton::pressed, this, &colorPseudoListWindowClass::valideColor);
}

bool colorPseudoListWindowClass::addPseudoToColorPseudoList(QString newPseudo, bool reallyAddPseudoToList)
{
    for(int i = 0; i < listOfColorPseudo->size(); ++i)
    {
        if(listOfColorPseudo->at(i).pseudo == newPseudo.toLower())
        {
            return false;
        }
    }

    if(reallyAddPseudoToList == true)
    {
        listOfColorPseudo->push_back(pseudoWithColorStruct());
        listOfColorPseudo->back().pseudo = newPseudo.toLower();
        listOfColorPseudo->back().red = 105;
        listOfColorPseudo->back().green = 105;
        listOfColorPseudo->back().blue = 105;
    }

    return true;
}

void colorPseudoListWindowClass::updateList()
{
    QList<QString> listOfPseudo;

    for(int i = 0; i < listOfColorPseudo->size(); ++i)
    {
        listOfPseudo.append(listOfColorPseudo->at(i).pseudo);
    }

    modelForListView.setStringList(listOfPseudo);
    emit listHasChanged();
}

void colorPseudoListWindowClass::addPseudo()
{
    addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this);
    QObject::connect(myAddPseudoWindow, &addPseudoWindowClass::newPseudoSet, this, &colorPseudoListWindowClass::addThisPseudo);
    myAddPseudoWindow->exec();
}

void colorPseudoListWindowClass::editCurrentPseudo()
{
    if(viewListOfColorPseudo.currentIndex().row() != -1)
    {
        addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this, listOfColorPseudo->at(viewListOfColorPseudo.currentIndex().row()).pseudo);
        QObject::connect(myAddPseudoWindow, &addPseudoWindowClass::newPseudoSet, this, &colorPseudoListWindowClass::setCurrentPseudo);
        myAddPseudoWindow->exec();
    }

}

void colorPseudoListWindowClass::removeCurrentPseudo()
{
    if(viewListOfColorPseudo.currentIndex().row() != -1)
    {
        listOfColorPseudo->removeAt(viewListOfColorPseudo.currentIndex().row());
        updateList();
    }
}

void colorPseudoListWindowClass::addThisPseudo(QString newPseudo)
{
    if(addPseudoToColorPseudoList(newPseudo) == true)
    {
        updateList();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
    }
}

void colorPseudoListWindowClass::setCurrentPseudo(QString newPseudo)
{
    if(addPseudoToColorPseudoList(newPseudo, false) == true)
    {
        (*listOfColorPseudo)[viewListOfColorPseudo.currentIndex().row()].pseudo = newPseudo.toLower();
        updateList();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
    }
}

void colorPseudoListWindowClass::itemSelectedHasChanged(QModelIndex index)
{
    redBox.setValue(listOfColorPseudo->at(index.row()).red);
    greenBox.setValue(listOfColorPseudo->at(index.row()).green);
    blueBox.setValue(listOfColorPseudo->at(index.row()).blue);
}

void colorPseudoListWindowClass::valideColor()
{
    if(viewListOfColorPseudo.currentIndex().row() != -1)
    {
        (*listOfColorPseudo)[viewListOfColorPseudo.currentIndex().row()].red = redBox.value();
        (*listOfColorPseudo)[viewListOfColorPseudo.currentIndex().row()].green = greenBox.value();
        (*listOfColorPseudo)[viewListOfColorPseudo.currentIndex().row()].blue = blueBox.value();
        emit listHasChanged();
    }
}
