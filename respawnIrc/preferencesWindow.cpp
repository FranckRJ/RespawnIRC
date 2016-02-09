#include "preferencesWindow.hpp"
#include "settingTool.hpp"

preferenceWindowClass::preferenceWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QGroupBox* groupBoxMessageContents = new QGroupBox("Contenu des messages", this);

    QVBoxLayout* vboxMessageContents = new QVBoxLayout(this);
    vboxMessageContents->addWidget(makeNewCheckBox("Ajouter un bouton pour citer un message", "showQuoteButton"));
    vboxMessageContents->addWidget(makeNewCheckBox("Ajouter un bouton pour ignorer un pseudo", "showBlacklistButton"));
    vboxMessageContents->addWidget(makeNewCheckBox("Ajouter un bouton pour éditer un message", "showEditButton"));
    vboxMessageContents->addWidget(makeNewCheckBox("Afficher les stickers", "showStickers"));
    vboxMessageContents->addWidget(makeNewCheckBox("Colorer le pseudo des modo/admin", "colorModoAndAdminPseudo"));
    vboxMessageContents->addWidget(makeNewCheckBox("Colorer les PEMT", "colorPEMT"));
    vboxMessageContents->addStretch(1);
    groupBoxMessageContents->setLayout(vboxMessageContents);

    QGroupBox* groupBoxOther = new QGroupBox("Autre", this);

    QVBoxLayout* vboxOther = new QVBoxLayout(this);
    vboxOther->addWidget(makeNewCheckBox("Afficher les boutons de décoration de texte", "showTextDecorationButton"));
    vboxOther->addWidget(makeNewCheckBox("Saisie du message en mode multiligne", "setMultilineEdit"));
    vboxOther->addWidget(makeNewCheckBox("Vérifier l'orthographe", "useSpellChecker"));
    vboxOther->addWidget(makeNewCheckBox("Charger les deux dernières pages", "loadTwoLastPage"));
    vboxOther->addWidget(makeNewCheckBox("Chercher les mises à jour au lancement", "searchForUpdateAtLaunch"));
    vboxOther->addWidget(makeNewCheckBox("Sauvegarder la taille de la fenêtre", "saveWindowGeometry"));
    vboxOther->addStretch(1);
    groupBoxOther->setLayout(vboxOther);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(groupBoxMessageContents);
    mainLayout->addWidget(groupBoxOther);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    setLayout(mainLayout);
    setWindowTitle("Préférences");
}

QCheckBox* preferenceWindowClass::makeNewCheckBox(QString messageInfo, QString boxNameValue)
{
    QCheckBox* tmp = new QCheckBox(messageInfo, this);
    tmp->setObjectName(boxNameValue);
    tmp->setChecked(settingToolClass::getThisBoolOption(boxNameValue));

    QObject::connect(tmp, &QCheckBox::toggled, this, &preferenceWindowClass::valueOfCheckboxChanged);

    return tmp;
}

void preferenceWindowClass::valueOfCheckboxChanged(bool newVal)
{
    QObject* senderObject = sender();

    if(senderObject != 0)
    {
        emit newValueForOption(newVal, senderObject->objectName());
    }
}
