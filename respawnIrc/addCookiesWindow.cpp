#include "addCookiesWindow.hpp"
#include "styleTool.hpp"

addCookiesWindowClass::addCookiesWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labHello = new QLabel("Cookie \"dlrowolleh\" :", this);
    QLabel* labConnect = new QLabel("Cookie \"coniunctio\" :", this);
    QLabel* labHelp = new QLabel("Pour de l'aide concernant l'ajout manuel des cookies, veuillez consulter le "
                                 "<a style=\"color: " + styleToolClass::getColorInfo().linkColor + ";\" href=\"https://github.com/LEpigeon888/RespawnIRC/wiki/Ajouter-manuellement-des-cookies\">wiki</a>.", this);
    QPushButton* buttonValide = new QPushButton("Valider", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);

    labHelp->setOpenExternalLinks(true);

    QGridLayout* cookiesLayout = new QGridLayout;
    cookiesLayout->addWidget(labHello, 0, 0);
    cookiesLayout->addWidget(&helloCookieLine, 0, 1);
    cookiesLayout->addWidget(labConnect, 1, 0);
    cookiesLayout->addWidget(&connectCookieLine, 1, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(buttonValide);
    buttonLayout->addWidget(buttonCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(cookiesLayout);
    mainLayout->addWidget(labHelp);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle("Ajouter des cookies");

    QObject::connect(buttonValide, &QPushButton::pressed, this, &addCookiesWindowClass::valideCookies);
    QObject::connect(buttonCancel, &QPushButton::pressed, this, &addCookiesWindowClass::close);
}

void addCookiesWindowClass::valideCookies()
{
    if(helloCookieLine.text().isEmpty() == false && connectCookieLine.text().isEmpty() == false)
    {
        emit newCookiesAvailable(helloCookieLine.text(), connectCookieLine.text());
        close();
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Au moins un des deux cookies n'a pas été renseigné.");
    }
}

