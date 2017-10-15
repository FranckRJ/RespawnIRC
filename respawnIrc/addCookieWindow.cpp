#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

#include "addCookieWindow.hpp"
#include "styleTool.hpp"

addCookieWindowClass::addCookieWindowClass(QWidget* parent) : baseDialogClass(parent)
{
    QLabel* labConnect = new QLabel("Cookie \"coniunctio\" :", this);
    QLabel* labHelp = new QLabel("Pour de l'aide concernant l'ajout manuel du cookie, veuillez consulter le "
                                 "<a style=\"color: " + styleTool::getColorInfo().linkColor + ";\" href=\"https://github.com/FranckRJ/RespawnIRC/wiki/Ajouter-manuellement-des-cookies\">wiki</a>.", this);
    QPushButton* buttonValide = new QPushButton("Valider", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);
    connectCookieLine = new QLineEdit(this);

    labHelp->setOpenExternalLinks(true);

    QHBoxLayout* cookieLayout = new QHBoxLayout;
    cookieLayout->addWidget(labConnect);
    cookieLayout->addWidget(connectCookieLine);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonValide);
    buttonLayout->addWidget(buttonCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(cookieLayout);
    mainLayout->addWidget(labHelp);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle("Ajouter le cookie de connexion");

    connect(buttonValide, &QPushButton::clicked, this, &addCookieWindowClass::valideCookie);
    connect(buttonCancel, &QPushButton::clicked, this, &addCookieWindowClass::close);
}

void addCookieWindowClass::valideCookie()
{
    if(connectCookieLine->text().isEmpty() == false)
    {
        emit newCookieAvailable(connectCookieLine->text());
        close();
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Le cookie de connexion n'a pas été renseigné.");
    }
}

