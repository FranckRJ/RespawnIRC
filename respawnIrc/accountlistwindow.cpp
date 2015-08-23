#include "accountlistwindow.hpp"
#include "connectWindow.hpp"

accountListWindowClass::accountListWindowClass(QList<accountStruct>* newListOfAccount, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labRemember = new QLabel("Se souvenir :", this);
    QPushButton* buttonAddAccount = new QPushButton("Ajouter", this);
    QPushButton* buttonRemoveAccount = new QPushButton("Supprimer", this);
    QPushButton* buttonLogin = new QPushButton("Se connecter sur tout les onglets", this);
    QPushButton* buttonLoginOneTopic = new QPushButton("Se connecter sur l'onglet acutel", this);

    rememberBox.setChecked(true);
    viewListOfAccount.setEditTriggers(QAbstractItemView::NoEditTriggers);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&viewListOfAccount, 0, 0, 1, 2);
    mainLayout->addWidget(labRemember, 1, 0);
    mainLayout->addWidget(&rememberBox, 1, 1);
    mainLayout->addWidget(buttonAddAccount, 2, 0);
    mainLayout->addWidget(buttonRemoveAccount, 2, 1);
    mainLayout->addWidget(buttonLogin, 3, 0);
    mainLayout->addWidget(buttonLoginOneTopic, 3, 1);

    setLayout(mainLayout);
    setWindowTitle("Liste des comptes");

    listOfAccount = newListOfAccount;
    updateList();
    viewListOfAccount.setModel(&modelForListView);

    QObject::connect(buttonAddAccount, &QPushButton::pressed, this, &accountListWindowClass::showConnectWindow);
    QObject::connect(buttonRemoveAccount, &QPushButton::pressed, this, &accountListWindowClass::removeCurrentAccount);
    QObject::connect(buttonLogin, &QPushButton::pressed, this, &accountListWindowClass::connectWithThisAccount);
    QObject::connect(buttonLoginOneTopic, &QPushButton::pressed, this, &accountListWindowClass::connectToOneTopicWithThisAccount);
}

bool accountListWindowClass::addAcountToThisList(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, QList<accountStruct>* thisList)
{
    if(newCookies.isEmpty() == false)
    {
        for(int i = 0; i < thisList->size(); ++i)
        {
            if(thisList->at(i).pseudo.toLower() == newPseudoOfUser.toLower())
            {
                return false;
            }
        }

        thisList->push_back(accountStruct());
        thisList->back().listOfCookie = newCookies;
        thisList->back().pseudo = newPseudoOfUser;

        return true;
    }

    return false;
}

void accountListWindowClass::updateList()
{
    QList<QString> listOfAccountName;

    for(int i = 0; i < listOfAccount->size(); ++i)
    {
        listOfAccountName.append(listOfAccount->at(i).pseudo);
    }

    modelForListView.setStringList(listOfAccountName);
    emit listHasChanged();
}

void accountListWindowClass::showConnectWindow()
{
    connectWindowClass* myConnectWindow = new connectWindowClass(this, false);
    QObject::connect(myConnectWindow, &connectWindowClass::newCookiesAvailable, this, &accountListWindowClass::addAccount);
    myConnectWindow->exec();
}

void accountListWindowClass::addAccount(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo)
{
    (void) saveInfo;

    if(addAcountToThisList(newCookies, newPseudoOfUser, listOfAccount) == true)
    {
        updateList();
    }
}

void accountListWindowClass::removeCurrentAccount()
{
    if(viewListOfAccount.currentIndex().row() != -1)
    {
        listOfAccount->removeAt(viewListOfAccount.currentIndex().row());
        updateList();
    }
}

void accountListWindowClass::connectWithThisAccount()
{
    if(viewListOfAccount.currentIndex().row() != -1)
    {
        emit useThisAccount(listOfAccount->at(viewListOfAccount.currentIndex().row()).listOfCookie, listOfAccount->at(viewListOfAccount.currentIndex().row()).pseudo, false, rememberBox.isChecked());
        close();
    }
}

void accountListWindowClass::connectToOneTopicWithThisAccount()
{
    if(viewListOfAccount.currentIndex().row() != -1)
    {
        emit useThisAccountForOneTopic(listOfAccount->at(viewListOfAccount.currentIndex().row()).listOfCookie, listOfAccount->at(viewListOfAccount.currentIndex().row()).pseudo, rememberBox.isChecked());
        close();
    }
}
