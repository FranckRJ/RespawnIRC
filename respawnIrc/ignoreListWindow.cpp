#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

#include "ignoreListWindow.hpp"
#include "addPseudoWindow.hpp"

ignoreListWindowClass::ignoreListWindowClass(QList<QString> *newListOfIgnoredPseudo, QWidget *parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QPushButton* buttonAddPseudo = new QPushButton("Ajouter", this);
    QPushButton* buttonEditPseudo = new QPushButton("Editer", this);
    QPushButton* buttonRemovePseudo = new QPushButton("Supprimer", this);
    QPushButton* buttonOk = new QPushButton("OK", this);

    viewListOfIgnoredPseudo.setEditTriggers(QAbstractItemView::NoEditTriggers);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&viewListOfIgnoredPseudo, 0, 0, 1, 3);
    mainLayout->addWidget(buttonAddPseudo, 1, 0);
    mainLayout->addWidget(buttonEditPseudo, 1, 1);
    mainLayout->addWidget(buttonRemovePseudo, 1, 2);
    mainLayout->addWidget(buttonOk, 2, 0, 1, 3);

    setLayout(mainLayout);
    setWindowTitle("Liste des ignorés");

    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    modelForListView.setStringList(*listOfIgnoredPseudo);
    viewListOfIgnoredPseudo.setModel(&modelForListView);

    QObject::connect(buttonAddPseudo, &QPushButton::pressed, this, &ignoreListWindowClass::addPseudo);
    QObject::connect(buttonEditPseudo, &QPushButton::pressed, this, &ignoreListWindowClass::editCurrentPseudo);
    QObject::connect(buttonRemovePseudo, &QPushButton::pressed, this, &ignoreListWindowClass::removeCurrentPseudo);
    QObject::connect(buttonOk, &QPushButton::pressed, this, &ignoreListWindowClass::close);
}

void ignoreListWindowClass::updateList()
{
    modelForListView.setStringList(*listOfIgnoredPseudo);
    emit listHasChanged();
}

void ignoreListWindowClass::addPseudo()
{
    addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this);
    QObject::connect(myAddPseudoWindow, &addPseudoWindowClass::newPseudoSet, this, &ignoreListWindowClass::addThisPseudo);
    myAddPseudoWindow->exec();
}

void ignoreListWindowClass::editCurrentPseudo()
{
    if(viewListOfIgnoredPseudo.currentIndex().row() != -1)
    {
        addPseudoWindowClass* myAddPseudoWindow = new addPseudoWindowClass(this, listOfIgnoredPseudo->at(viewListOfIgnoredPseudo.currentIndex().row()));
        QObject::connect(myAddPseudoWindow, &addPseudoWindowClass::newPseudoSet, this, &ignoreListWindowClass::setCurrentPseudo);
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
        QMessageBox::warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
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
        QMessageBox::warning(this, "Erreur", "Le pseudo est déjà présent dans la liste.");
    }
}
