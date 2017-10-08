#include <QWebEngineProfile>
#include <QWebEngineCookieStore>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>

#include "connectWindow.hpp"
#include "addCookieWindow.hpp"
#include "addPseudoWindow.hpp"
#include "parsingTool.hpp"

connectWindowClass::connectWindowClass(QWidget* parent, bool showRemeberBox) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labForPseudo = new QLabel("Entrez le pseudo avec lequel vous voulez vous connecter :", this);
    QLabel* labForButton = new QLabel("Une fois connecté, cliquez ici :", this);
    buttonShowWebView = new QPushButton("Afficher la page de connexion", this);
    QPushButton* buttonAddCookie = new QPushButton("Ajouter le cookie", this);
    QPushButton* buttonValidate = new QPushButton("Valider", this);
    QPushButton* buttonHelp = new QPushButton("Aide pour se connecter", this);
    pseudoLine = new QLineEdit(this);
    rememberBox = new QCheckBox(this);

    rememberBox->setChecked(true);
    buttonValidate->setDefault(true);

    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(buttonAddCookie);
    bottomLayout->addWidget(labForPseudo);
    bottomLayout->addWidget(pseudoLine);
    bottomLayout->addWidget(labForButton, 1, Qt::AlignRight);
    bottomLayout->addWidget(buttonValidate);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(buttonShowWebView);
    mainLayout->addLayout(bottomLayout);

    if(showRemeberBox == true)
    {
        QLabel* labForRemember = new QLabel("Enregistrer le compte dans la liste des comptes (se souvenir) :", this);
        QHBoxLayout* rememberLayout = new QHBoxLayout;
        rememberLayout->addWidget(labForRemember);
        rememberLayout->addWidget(rememberBox, 1, Qt::AlignLeft);
        mainLayout->addLayout(rememberLayout);
    }
    else
    {
        rememberBox->setVisible(false);
    }

    mainLayout->addWidget(buttonHelp);

    setLayout(mainLayout);
    setWindowTitle("Page de connexion");

    connect(buttonShowWebView, &QPushButton::clicked, this, &connectWindowClass::addWebView);
    connect(buttonAddCookie, &QPushButton::clicked, this, &connectWindowClass::showAddCookieWindow);
    connect(buttonValidate, &QPushButton::clicked, this, &connectWindowClass::valideConnect);
    connect(buttonHelp, &QPushButton::clicked, this, &connectWindowClass::showHelpConnect);
}

void connectWindowClass::addWebView()
{
    if(webView == nullptr)
    {
        QWebEngineProfile* customProfile = new QWebEngineProfile(this);
        QWebEnginePage* customPage = new QWebEnginePage(customProfile, this);

        webView = new QWebEngineView(this);

        webView->setPage(customPage);
        webView->load(QUrl("https://www.jeuxvideo.com/login"));

        mainLayout->removeWidget(buttonShowWebView);
        buttonShowWebView->setEnabled(false);
        buttonShowWebView->setVisible(false);
        mainLayout->insertWidget(0, webView);

        connect(webView->page()->profile()->cookieStore(), &QWebEngineCookieStore::cookieAdded, this, &connectWindowClass::checkThisCookie);
    }
}

void connectWindowClass::checkThisCookie(QNetworkCookie cookie)
{
    if(cookie.name() == "coniunctio")
    {
        cookie.setExpirationDate(QDateTime::currentDateTime().addYears(8));
        connectCookie = cookie;
    }

    adjustSize();
}

void connectWindowClass::showAddCookieWindow()
{
    addCookieWindowClass* myAddCookieWindow = new addCookieWindowClass(this);
    connect(myAddCookieWindow, &addCookieWindowClass::newCookieAvailable, this, &connectWindowClass::addCookieManually);
    myAddCookieWindow->exec();
}

void connectWindowClass::addCookieManually(QString newConnectCookie)
{
    connectCookie = QNetworkCookie("coniunctio", newConnectCookie.toStdString().c_str());
}

void connectWindowClass::valideConnect()
{
    if(addPseudoWindowClass::pseudoIsValide(pseudoLine->text()) == true && connectCookie.value().isEmpty() == false)
    {
        emit newCookieAvailable(connectCookie, pseudoLine->text(), rememberBox->isChecked(), rememberBox->isChecked());
        close();
        return;
    }
    else
    {
        if(connectCookie.value().isEmpty() == true)
        {
            QMessageBox::warning(this, "Erreur", "Vous ne vous êtes pas connecté.");
        }
        else
        {
            QMessageBox::warning(this, "Erreur", "Le pseudo n'est pas entré ou n'est pas valide.");
        }
    }
}

void connectWindowClass::showHelpConnect()
{
    QMessageBox::information(this, "Aide", "Pour vous connecter, veuillez suivre ces étapes :\n"
                           "1 - cliquez sur le bouton \"Afficher la page de connexion\", si le logiciel crash vous devrez ajouter le cookie manuellement (voir dernière ligne de cette aide).\n"
                           "2 - connectez-vous sur JVC avec votre pseudo.\n"
                           "3 - cliquez sur le bouton \"VALIDER\" qui possède un fond vert sur la page de JVC, attendez que la page d'accueil ait fini de charger puis "
                           "passez à l'étape suivante.\n"
                           "4 - renseignez votre pseudo dans le champ présent en bas de la fenêtre.\n"
                           "5 - cliquez sur le bouton \"Valider\" en bas à droite de la fenêtre.\n\n"
                           "Informations importantes : \n"
                           "- si après avoir cliqué sur \"Afficher la page de connexion\" vous ne voyez pas la page de connexion, agrandissez la fenêtre.\n"
                           "- il est possible que juste après vous être connecté vous ne puissiez pas utiliser votre pseudo, "
                           "dans ce cas relancez RespawnIRC.\n"
                           "- si vous ne voyez pas le captcha (ou si vous ne pouvez pas le remplir) cliquez sur le bouton \"Ajouter le cookie\" "
                           "afin d'ajouter manuellement le cookie de connexion, après cela, passez directement à l'étape 4.");
}
