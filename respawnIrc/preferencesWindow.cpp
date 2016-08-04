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
    vboxMessageContents->addWidget(makeNewCheckBox("Ajouter un bouton pour supprimer un message", "showDeleteButton"));
    vboxMessageContents->addWidget(makeNewCheckBox("Afficher les stickers", "showStickers"));
    vboxMessageContents->addWidget(makeNewCheckBox("Remplacer les stickers par des smileys", "stickersToSmiley"));
    vboxMessageContents->addWidget(makeNewCheckBox("Colorer le pseudo des modo/admin", "colorModoAndAdminPseudo"));
    vboxMessageContents->addWidget(makeNewCheckBox("Colorer les PEMT", "colorPEMT"));
    vboxMessageContents->addWidget(makeNewCheckBox("Colorer votre pseudo dans les messages", "colorUserPseudoInMessages"));
    vboxMessageContents->addWidget(makeNewCheckBox("Améliorer les citations", "betterQuote"));
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
    vboxOther->addWidget(makeNewCheckBox("Changer la couleur du message lors de l'édition", "changeColorOnEdit"));
    vboxOther->addWidget(makeNewCheckBox("Afficher le nombre de réponses dans la liste des topics", "showNumberOfMessagesInTopicList"));
    vboxOther->addWidget(makeNewCheckBox("Couper les longs noms de topics dans la liste des topics", "cutLongTopicNameInTopicList"));
    vboxOther->addWidget(makeNewCheckBox("Télécharger automatiquement les stickers manquants", "downloadMissingStickers"));
    vboxOther->addWidget(makeNewCheckBox("Afficher les miniatures noelshack", "downloadNoelshackImages"));
    vboxOther->addStretch(1);
    groupBoxOther->setLayout(vboxOther);

    QGroupBox* groupBoxOtherBool = new QGroupBox("Autre booléen", this);

    QVBoxLayout* vboxOtherBool = new QVBoxLayout(this);
    vboxOtherBool->addWidget(makeNewCheckBox("Afficher la liste des topics", "showListOfTopic"));
    vboxOtherBool->addWidget(makeNewCheckBox("Récupérer le premier message du topic", "getFirstMessageOfTopic"));
    vboxOtherBool->addWidget(makeNewCheckBox("Ignorer les erreurs réseau", "ignoreNetworkError"));
    vboxOtherBool->addWidget(makeNewCheckBox("Beeper lors de la réception d'un message", "beepWhenWarn"));
    vboxOtherBool->addWidget(makeNewCheckBox("Avertir visuellement lors de la réception d'un message", "warnUser"));
    vboxOtherBool->addWidget(makeNewCheckBox("Avertir lors de l'édition d'un message", "warnWhenEdit"));
    vboxOtherBool->addStretch(1);
    groupBoxOtherBool->setLayout(vboxOtherBool);

    QGroupBox* groupBoxOtherInt = new QGroupBox("Autre entier", this);

    QVBoxLayout* vboxOtherInt = new QVBoxLayout(this);
    vboxOtherInt->addLayout(makeNewSpinBox("Temps en secondes avant le timeout des requêtes", "timeoutInSecond"));
    vboxOtherInt->addLayout(makeNewSpinBox("Taux de rafraichissement des topics", "updateTopicTime"));
    vboxOtherInt->addLayout(makeNewSpinBox("Nombre de messages affichés au premier chargement", "numberOfMessageShowedFirstTime"));
    vboxOtherInt->addLayout(makeNewSpinBox("Taille des stickers", "stickersSize"));
    vboxOtherInt->addLayout(makeNewSpinBox("Taille de la zone de saisie", "textBoxSize"));
    vboxOtherInt->addLayout(makeNewSpinBox("Nombre maximal de quotes imbriquées", "maxNbOfQuotes"));
    vboxOtherInt->addStretch(1);
    groupBoxOtherInt->setLayout(vboxOtherInt);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(groupBoxMessageContents, 0, 0);
    mainLayout->addWidget(groupBoxOther, 0, 1);
    mainLayout->addWidget(groupBoxOtherBool, 1, 0);
    mainLayout->addWidget(groupBoxOtherInt, 1, 1);
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

QHBoxLayout* preferenceWindowClass::makeNewSpinBox(QString messageInfo, QString boxNameValue)
{
    QHBoxLayout* hboxForSpinBox = new QHBoxLayout();
    QSpinBox* newSpinBox = new QSpinBox(this);
    intSettingStruct infoForIntSetting = settingToolClass::getThisIntOption(boxNameValue);
    QLabel* newInfoForSpinBox = new QLabel(messageInfo +  " (entre " + QString::number(infoForIntSetting.minValue) + " et " + QString::number(infoForIntSetting.maxValue) + ") :", this);

    newSpinBox->setObjectName(boxNameValue);
    newSpinBox->setMinimum(infoForIntSetting.minValue);
    newSpinBox->setMaximum(infoForIntSetting.maxValue);
    newSpinBox->setValue(infoForIntSetting.value);

    hboxForSpinBox->addWidget(newInfoForSpinBox);
    hboxForSpinBox->addWidget(newSpinBox);

    QObject::connect(newSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &preferenceWindowClass::valueOfSpinboxChanged);

    return hboxForSpinBox;
}

void preferenceWindowClass::valueOfCheckboxChanged(bool newVal)
{
    QObject* senderObject = sender();

    if(senderObject != nullptr)
    {
        emit newValueForBoolOption(newVal, senderObject->objectName());
    }
}

void preferenceWindowClass::valueOfSpinboxChanged(int newVal)
{
    QObject* senderObject = sender();

    if(senderObject != nullptr)
    {
        emit newValueForIntOption(newVal, senderObject->objectName());
    }
}
