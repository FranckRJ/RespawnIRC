#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>

#include "accountListWindow.hpp"
#include "connectWindow.hpp"

accountListWindowClass::accountListWindowClass(QList<accountStruct>* newListOfAccount, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labRemember = new QLabel("Se souvenir :", this);
    QPushButton* buttonAddAccount = new QPushButton("Ajouter", this);
    QPushButton* buttonRemoveAccount = new QPushButton("Supprimer", this);
    QPushButton* buttonLogin = new QPushButton("Se connecter sur tous les onglets", this);
    QPushButton* buttonLoginOneTopic = new QPushButton("Se connecter sur l'onglet actuel", this);
    rememberBox = new QCheckBox(this);
    viewListOfAccount = new QListView(this);
    modelForListView = new QStringListModel(viewListOfAccount);

    rememberBox->setChecked(true);
    viewListOfAccount->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(viewListOfAccount, 0, 0, 1, 2);
    mainLayout->addWidget(labRemember, 1, 0);
    mainLayout->addWidget(rememberBox, 1, 1);
    mainLayout->addWidget(buttonAddAccount, 2, 0);
    mainLayout->addWidget(buttonRemoveAccount, 2, 1);
    mainLayout->addWidget(buttonLogin, 3, 0);
    mainLayout->addWidget(buttonLoginOneTopic, 3, 1);

    setLayout(mainLayout);
    setWindowTitle("Liste des comptes");

    listOfAccount = newListOfAccount;
    updateList();
    viewListOfAccount->setModel(modelForListView);

    connect(buttonAddAccount, &QPushButton::clicked, this, &accountListWindowClass::showConnectWindow);
    connect(buttonRemoveAccount, &QPushButton::clicked, this, &accountListWindowClass::removeCurrentAccount);
    connect(buttonLogin, &QPushButton::clicked, this, &accountListWindowClass::connectWithThisAccount);
    connect(buttonLoginOneTopic, &QPushButton::clicked, this, &accountListWindowClass::connectToOneTopicWithThisAccount);
}

void accountListWindowClass::addOrUpdateAcountInThisList(QNetworkCookie newConnectCookie, QString newPseudoOfUser, QList<accountStruct>* thisList)
{
    accountStruct* accountToChange = nullptr;

    for(accountStruct& thisAccount : *thisList)
    {
        if(thisAccount.pseudo.toLower() == newPseudoOfUser.toLower())
        {
            accountToChange = &thisAccount;
        }
    }

    if(accountToChange == nullptr)
    {
        thisList->push_back(accountStruct());
        accountToChange = &thisList->back();
    }

    accountToChange->connectCookie = newConnectCookie;
    accountToChange->pseudo = newPseudoOfUser;
}

void accountListWindowClass::updateList()
{
    QList<QString> listOfAccountName;

    for(const accountStruct& thisAccount : *listOfAccount)
    {
        listOfAccountName.append(thisAccount.pseudo);
    }

    modelForListView->setStringList(listOfAccountName);
    emit listHasChanged();
}

void accountListWindowClass::showConnectWindow()
{
    connectWindowClass* myConnectWindow = new connectWindowClass(this, false);
    connect(myConnectWindow, &connectWindowClass::newCookieAvailable, this, &accountListWindowClass::addAccount);
    myConnectWindow->exec();
}

void accountListWindowClass::addAccount(QNetworkCookie newConnectCookie, QString newPseudoOfUser, bool saveInfo)
{
    (void) saveInfo;

    addOrUpdateAcountInThisList(newConnectCookie, newPseudoOfUser, listOfAccount);
    updateList();
}

void accountListWindowClass::removeCurrentAccount()
{
    if(viewListOfAccount->currentIndex().row() != -1)
    {
        emit eraseThisPseudo(listOfAccount->at(viewListOfAccount->currentIndex().row()).pseudo);
        listOfAccount->removeAt(viewListOfAccount->currentIndex().row());
        updateList();
    }
}

void accountListWindowClass::connectWithThisAccount()
{
    if(viewListOfAccount->currentIndex().row() != -1)
    {
        emit useThisAccount(listOfAccount->at(viewListOfAccount->currentIndex().row()).connectCookie, listOfAccount->at(viewListOfAccount->currentIndex().row()).pseudo, false, rememberBox->isChecked());
        close();
    }
}

void accountListWindowClass::connectToOneTopicWithThisAccount()
{
    if(viewListOfAccount->currentIndex().row() != -1)
    {
        emit useThisAccountForOneTopic(listOfAccount->at(viewListOfAccount->currentIndex().row()).connectCookie, listOfAccount->at(viewListOfAccount->currentIndex().row()).pseudo, rememberBox->isChecked());
        close();
    }
}
