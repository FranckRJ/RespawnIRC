#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QColorDialog>
#include <QColor>

#include "colorPseudoListWindow.hpp"
#include "addPseudoWindow.hpp"

colorPseudoListWindowClass::colorPseudoListWindowClass(QList<pseudoWithColorStruct>* newListOfColorPseudo, QWidget* parent) :
    baseDialogClass(parent)
{
    QPushButton* buttonAddPseudo = new QPushButton("Ajouter", this);
    QPushButton* buttonEditPseudo = new QPushButton("Editer", this);
    QPushButton* buttonRemovePseudo = new QPushButton("Supprimer", this);
    QPushButton* buttonValidate = new QPushButton("Choisir une couleur", this);
    QPushButton* buttonOk = new QPushButton("OK", this);
    viewListOfColorPseudo = new QListView(this);
    modelForListView = new QStringListModel(viewListOfColorPseudo);

    viewListOfColorPseudo->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(viewListOfColorPseudo, 0, 0, 1, 3);
    mainLayout->addWidget(buttonAddPseudo, 1, 0);
    mainLayout->addWidget(buttonEditPseudo, 1, 1);
    mainLayout->addWidget(buttonRemovePseudo, 1, 2);
    mainLayout->addWidget(buttonValidate, 2, 0, 1, 3);
    mainLayout->addWidget(buttonOk, 3, 0, 1, 3);

    setLayout(mainLayout);
    setWindowTitle("Liste des pseudo avec couleur");

    listOfColorPseudo = newListOfColorPseudo;
    updateList();
    viewListOfColorPseudo->setModel(modelForListView);

    connect(buttonAddPseudo, &QPushButton::clicked, this, &colorPseudoListWindowClass::addPseudo);
    connect(buttonEditPseudo, &QPushButton::clicked, this, &colorPseudoListWindowClass::editCurrentPseudo);
    connect(buttonRemovePseudo, &QPushButton::clicked, this, &colorPseudoListWindowClass::removeCurrentPseudo);
    connect(buttonValidate, &QPushButton::clicked, this, &colorPseudoListWindowClass::chooseColor);
    connect(buttonOk, &QPushButton::clicked, this, &colorPseudoListWindowClass::close);
}

bool colorPseudoListWindowClass::addPseudoToColorPseudoList(QString newPseudo, bool reallyAddPseudoToList)
{
    for(const pseudoWithColorStruct& thisColor : *listOfColorPseudo)
    {
        if(thisColor.pseudo == newPseudo.toLower())
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

    for(const pseudoWithColorStruct& thisColor : *listOfColorPseudo)
    {
        listOfPseudo.append(thisColor.pseudo);
    }

    modelForListView->setStringList(listOfPseudo);
    emit listHasChanged();
}

void colorPseudoListWindowClass::addPseudo()
{
    addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this);
    connect(myAddPseudoWindow, &addPseudoWindowClass::newPseudoSet, this, &colorPseudoListWindowClass::addThisPseudo);
    myAddPseudoWindow->exec();
}

void colorPseudoListWindowClass::editCurrentPseudo()
{
    if(viewListOfColorPseudo->currentIndex().row() != -1)
    {
        addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this, listOfColorPseudo->at(viewListOfColorPseudo->currentIndex().row()).pseudo);
        connect(myAddPseudoWindow, &addPseudoWindowClass::newPseudoSet, this, &colorPseudoListWindowClass::setCurrentPseudo);
        myAddPseudoWindow->exec();
    }

}

void colorPseudoListWindowClass::removeCurrentPseudo()
{
    if(viewListOfColorPseudo->currentIndex().row() != -1)
    {
        listOfColorPseudo->removeAt(viewListOfColorPseudo->currentIndex().row());
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
        QMessageBox::warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
    }
}

void colorPseudoListWindowClass::setCurrentPseudo(QString newPseudo)
{
    if(addPseudoToColorPseudoList(newPseudo, false) == true)
    {
        (*listOfColorPseudo)[viewListOfColorPseudo->currentIndex().row()].pseudo = newPseudo.toLower();
        updateList();
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
    }
}

void colorPseudoListWindowClass::chooseColor()
{
    if(viewListOfColorPseudo->currentIndex().row() != -1)
    {
        QColor newColor = QColorDialog::getColor(QColor(listOfColorPseudo->at(viewListOfColorPseudo->currentIndex().row()).red,
                                                        listOfColorPseudo->at(viewListOfColorPseudo->currentIndex().row()).green,
                                                        listOfColorPseudo->at(viewListOfColorPseudo->currentIndex().row()).blue), this, "Choisir une couleur");
        (*listOfColorPseudo)[viewListOfColorPseudo->currentIndex().row()].red = newColor.red();
        (*listOfColorPseudo)[viewListOfColorPseudo->currentIndex().row()].green = newColor.green();
        (*listOfColorPseudo)[viewListOfColorPseudo->currentIndex().row()].blue = newColor.blue();
        emit listHasChanged();
    }
}
