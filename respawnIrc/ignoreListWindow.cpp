#include "ignoreListWindow.hpp"
#include "addPseudoWindow.hpp"

ignoreListWindowClass::ignoreListWindowClass(QList<QString> *newListOfIgnoredPseudo, QWidget *parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QPushButton* buttonAddPseudo = new QPushButton("Ajouter", this);
    QPushButton* buttonEditPseudo = new QPushButton("Editer", this);
    QPushButton* buttonRemovePseudo = new QPushButton("Supprimer", this);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&viewListOfIgnoredPseudo, 0, 0, 1, 3);
    mainLayout->addWidget(buttonAddPseudo, 1, 0);
    mainLayout->addWidget(buttonEditPseudo, 1, 1);
    mainLayout->addWidget(buttonRemovePseudo, 1, 2);

    setLayout(mainLayout);
    setWindowTitle("Liste des ignorés");

    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    modelForListView.setStringList(*listOfIgnoredPseudo);
    viewListOfIgnoredPseudo.setModel(&modelForListView);

    connect(buttonAddPseudo, SIGNAL(pressed()), this, SLOT(addPseudo()));
    connect(buttonEditPseudo, SIGNAL(pressed()), this, SLOT(editCurrentPseudo()));
    connect(buttonRemovePseudo, SIGNAL(pressed()), this, SLOT(removeCurrentPseudo()));
}

void ignoreListWindowClass::updateList()
{
    modelForListView.setStringList(*listOfIgnoredPseudo);
    emit listHasChanged();
}

void ignoreListWindowClass::addPseudo()
{
    addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this);
    connect(myAddPseudoWindow, SIGNAL(newPseudoSet(QString)), this, SLOT(addThisPseudo(QString)));
    myAddPseudoWindow->exec();
}

void ignoreListWindowClass::editCurrentPseudo()
{
    if(viewListOfIgnoredPseudo.currentIndex().row() != -1)
    {
        addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this, listOfIgnoredPseudo->at(viewListOfIgnoredPseudo.currentIndex().row()));
        connect(myAddPseudoWindow, SIGNAL(newPseudoSet(QString)), this, SLOT(setCurrentPseudo(QString)));
        myAddPseudoWindow->exec();
    }

}

void ignoreListWindowClass::removeCurrentPseudo()
{
    if(viewListOfIgnoredPseudo.currentIndex().row() != -1)
    {
        listOfIgnoredPseudo->removeAt(viewListOfIgnoredPseudo.currentIndex().row());
        updateList();
    }
}

void ignoreListWindowClass::addThisPseudo(QString newPseudo)
{
    if(listOfIgnoredPseudo->indexOf(newPseudo.toLower()) == -1)
    {
        listOfIgnoredPseudo->push_back(newPseudo.toLower());
        updateList();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
    }
}

void ignoreListWindowClass::setCurrentPseudo(QString newPseudo)
{
    if(listOfIgnoredPseudo->indexOf(newPseudo.toLower()) == -1)
    {
        (*listOfIgnoredPseudo)[viewListOfIgnoredPseudo.currentIndex().row()] = newPseudo.toLower();
        updateList();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
    }
}
