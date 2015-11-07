#include "connectWindow.hpp"
#include "parsingTool.hpp"

connectWindowClass::connectWindowClass(QWidget* parent, bool showRemeberBox) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labForPseudo = new QLabel("Entrez le pseudo avec lequel vous voulez vous connecter :", this);
    QLabel* labForButton = new QLabel("Une fois connecté, cliquez ici :", this);
    QPushButton* buttonValidate = new QPushButton("Valider", this);
    QPushButton* buttonHelp = new QPushButton("Aide pour se connecter", this);

    webView = new QWebView(this);
    rememberBox.setChecked(false);

    QHBoxLayout* bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(labForPseudo);
    bottomLayout->addWidget(&pseudoLine);
    bottomLayout->addWidget(labForButton, 1, Qt::AlignRight);
    bottomLayout->addWidget(buttonValidate);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(webView);
    mainLayout->addLayout(bottomLayout);

    if(showRemeberBox == true)
    {
        QLabel* labForRemember = new QLabel("Enregistrer le compte dans la liste des comptes :", this);
        QHBoxLayout* rememberLayout = new QHBoxLayout;
        rememberLayout->addWidget(labForRemember);
        rememberLayout->addWidget(&rememberBox, 1, Qt::AlignLeft);
        mainLayout->addLayout(rememberLayout);
    }
    else
    {
        rememberBox.setVisible(false);
    }

    mainLayout->addWidget(buttonHelp);

    setLayout(mainLayout);

    QNetworkAccessManager* tmpManagerForReply;

    webView->load(QUrl("http://www.jeuxvideo.com/login"));
    tmpManagerForReply = webView->page()->networkAccessManager();

    QObject::connect(tmpManagerForReply, &QNetworkAccessManager::finished, this, &connectWindowClass::newPageLoaded);
    QObject::connect(buttonValidate, &QPushButton::pressed, this, &connectWindowClass::valideConnect);
    QObject::connect(buttonHelp, &QPushButton::pressed, this, &connectWindowClass::showHelpConnect);
}

void connectWindowClass::newPageLoaded(QNetworkReply* reply)
{
    if(reply->isReadable() == true)
    {
        if(reply->url().toDisplayString().startsWith("http://www.jeuxvideo.com"))
        {
            QList<QNetworkCookie> newCookieList = qvariant_cast<QList<QNetworkCookie> >(reply->header(QNetworkRequest::SetCookieHeader));

            for(int i = 0; i < newCookieList.size(); ++i)
            {
                if(newCookieList.at(i).name() == "dlrowolleh" || newCookieList.at(i).name() == "coniunctio")
                {
                    for(int j = 0; j < cookieList.size(); ++j)
                    {
                        if(cookieList.at(j).name() == newCookieList.at(i).name())
                        {
                            cookieList.removeAt(j);
                            break;
                        }
                    }
                    cookieList.append(newCookieList.at(i));
                }
            }
        }
    }
}

void connectWindowClass::valideConnect()
{
    if(pseudoLine.text().isEmpty() == false && cookieList.size() >= 2)
    {
        emit newCookiesAvailable(cookieList, pseudoLine.text(), rememberBox.isChecked(), rememberBox.isChecked());
        close();
        return;
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le pseudo n'est pas entré ou vous n'êtes pas connecté.");
    }
}

void connectWindowClass::showHelpConnect()
{
    QMessageBox messageBox;
    messageBox.information(this, "Aide", "Pour vous connecter, veuillez suivre ces étapes :\n"
                           "- renseignez le pseudo que vous allez utiliser dans le champ présent en bas de la fenêtre.\n"
                           "- connectez-vous sur JVC avec ce même pseudo.\n"
                           "- après avoir cliqué sur le bouton \"VALIDER\" qui possède un fond vert sur la page de JVC, attendez que la page d'accueil ait fini de charger puis "
                           "cliquez sur le bouton \"Valider\" en bas à droite de la fenêtre.\n\n"
                           "Information importante : il est possible que juste après vous être connecté vous ne puissiez pas utiliser votre pseudo, "
                           "dans ce cas relancez RespawnIRC.");
}
