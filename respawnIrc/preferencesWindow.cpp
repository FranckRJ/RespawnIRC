#include "preferencesWindow.hpp"
#include "settingTool.hpp"

preferenceWindowClass::preferenceWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QTabWidget* mainWidget = new QTabWidget(this);

    QHBoxLayout* realMainLayout = new QHBoxLayout(this);
    realMainLayout->addWidget(mainWidget);
    realMainLayout->setMargin(5);
    realMainLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainWidget->addTab(createWidgetForMainTab(), "Général");
    mainWidget->addTab(createWidgetForMessageAndTopicStyleTab(), "Style des messages / topics");
    mainWidget->addTab(createWidgetForImageTab(), "Image");

    setLayout(realMainLayout);
    setWindowTitle("Préférences");
}

QWidget* preferenceWindowClass::createWidgetForMainTab()
{
    QWidget* mainTabWidget = new QWidget(this);

    QGroupBox* groupBoxAlert = new QGroupBox("Alerte", this);

    QVBoxLayout* vboxAlert = new QVBoxLayout();
    vboxAlert->addWidget(makeNewCheckBox("Beeper lors de la réception d'un message", "beepWhenWarn"));
    vboxAlert->addWidget(makeNewCheckBox("Avertir visuellement lors de la réception d'un message", "warnUser"));
    vboxAlert->addWidget(makeNewCheckBox("Avertir lors de l'édition d'un message", "warnWhenEdit"));
    vboxAlert->addStretch(1);
    groupBoxAlert->setLayout(vboxAlert);

    QGroupBox* groupBoxOther = new QGroupBox("Autre", this);

    QVBoxLayout* vboxOther = new QVBoxLayout();
    vboxOther->addWidget(makeNewCheckBox("Vérifier l'orthographe", "useSpellChecker"));
    vboxOther->addWidget(makeNewCheckBox("Changer la couleur du message lors de l'édition", "changeColorOnEdit"));
    vboxOther->addWidget(makeNewCheckBox("Chercher les mises à jour au lancement", "searchForUpdateAtLaunch"));
    vboxOther->addStretch(1);
    groupBoxOther->setLayout(vboxOther);

    QGroupBox* groupBoxWindowLayout = new QGroupBox("Agencement de la fenêtre", this);

    QVBoxLayout* vboxWindowLayout = new QVBoxLayout();
    vboxWindowLayout->addWidget(makeNewCheckBox("Sauvegarder la taille de la fenêtre", "saveWindowGeometry"));
    vboxWindowLayout->addWidget(makeNewCheckBox("Afficher les boutons de décoration de texte", "showTextDecorationButton"));
    vboxWindowLayout->addWidget(makeNewCheckBox("Afficher la liste des topics", "showListOfTopic"));
    vboxWindowLayout->addWidget(makeNewCheckBox("Saisie du message en mode multiligne", "setMultilineEdit"));
    vboxWindowLayout->addLayout(makeNewSpinBox("Taille de la zone de saisie", "textBoxSize"));
    vboxWindowLayout->addStretch(1);
    groupBoxWindowLayout->setLayout(vboxWindowLayout);

    QGroupBox* groupBoxAdvanced = new QGroupBox("Avancé", this);

    QVBoxLayout* vboxAdvanced = new QVBoxLayout();
    vboxAdvanced->addWidget(makeNewCheckBox("Charger les deux dernières pages", "loadTwoLastPage"));
    vboxAdvanced->addWidget(makeNewCheckBox("Ignorer les erreurs réseau", "ignoreNetworkError"));
    vboxAdvanced->addLayout(makeNewSpinBox("Temps en secondes avant le timeout des requêtes", "timeoutInSecond"));
    vboxAdvanced->addLayout(makeNewSpinBox("Taux de rafraichissement des topics", "updateTopicTime"));
    vboxAdvanced->addLayout(makeNewSpinBox("Nombre maximal de quotes imbriquées", "maxNbOfQuotes"));
    vboxAdvanced->addStretch(1);
    groupBoxAdvanced->setLayout(vboxAdvanced);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(groupBoxAlert, 0, 0);
    mainLayout->addWidget(groupBoxOther, 0, 1);
    mainLayout->addWidget(groupBoxWindowLayout, 1, 0);
    mainLayout->addWidget(groupBoxAdvanced, 1, 1);
    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);

    QVBoxLayout* realMainLayout = new QVBoxLayout();
    realMainLayout->addLayout(mainLayout);
    realMainLayout->addStretch(1);

    mainTabWidget->setLayout(realMainLayout);

    return mainTabWidget;
}

QWidget* preferenceWindowClass::createWidgetForImageTab()
{
    QWidget* mainTabWidget = new QWidget(this);

    QGroupBox* groupBoxGeneral = new QGroupBox("Général", this);

    QVBoxLayout* vboxGeneral = new QVBoxLayout();
    vboxGeneral->addWidget(makeNewCheckBox("Type de rafraichissement des images (imaginez une liste déroulante)", "existepas"));
    vboxGeneral->addStretch(1);
    groupBoxGeneral->setLayout(vboxGeneral);

    QGroupBox* groupBoxStickers = new QGroupBox("Stickers", this);

    QVBoxLayout* vboxStickers = new QVBoxLayout();
    vboxStickers->addWidget(makeNewCheckBox("Afficher les stickers", "showStickers"));
    vboxStickers->addWidget(makeNewCheckBox("Télécharger automatiquement les stickers manquants", "downloadMissingStickers"));
    vboxStickers->addWidget(makeNewCheckBox("Remplacer les stickers par des smileys", "stickersToSmiley"));
    vboxStickers->addLayout(makeNewSpinBox("Taille des stickers", "stickersSize"));
    vboxStickers->addStretch(1);
    groupBoxStickers->setLayout(vboxStickers);

    QGroupBox* groupBoxOtherNoelshack = new QGroupBox("Noelshack", this);

    QVBoxLayout* vboxNoelshack = new QVBoxLayout();
    vboxNoelshack->addWidget(makeNewCheckBox("Afficher les miniatures noelshack", "downloadNoelshackImages"));
    vboxNoelshack->addLayout(makeNewSpinBox("Largeur des miniatures noelshack", "noelshackImageWidth"));
    vboxNoelshack->addLayout(makeNewSpinBox("Hauteur des miniatures noelshack", "noelshackImageHeight"));
    vboxNoelshack->addStretch(1);
    groupBoxOtherNoelshack->setLayout(vboxNoelshack);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(groupBoxGeneral, 0, 0, 1, 2);
    mainLayout->addWidget(groupBoxStickers, 1, 0);
    mainLayout->addWidget(groupBoxOtherNoelshack, 1, 1);
    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);

    QVBoxLayout* realMainLayout = new QVBoxLayout();
    realMainLayout->addLayout(mainLayout);
    realMainLayout->addStretch(1);

    mainTabWidget->setLayout(realMainLayout);

    return mainTabWidget;
}

QWidget* preferenceWindowClass::createWidgetForMessageAndTopicStyleTab()
{
    QWidget* mainTabWidget = new QWidget(this);

    QGroupBox* groupBoxTopicStyle = new QGroupBox("Style des topics", this);

    QVBoxLayout* vboxTopicStyle = new QVBoxLayout();
    vboxTopicStyle->addWidget(makeNewCheckBox("Afficher le nombre de réponses dans la liste des topics", "showNumberOfMessagesInTopicList"));
    vboxTopicStyle->addWidget(makeNewCheckBox("Couper les longs noms de topics dans la liste des topics", "cutLongTopicNameInTopicList"));
    vboxTopicStyle->addStretch(1);
    groupBoxTopicStyle->setLayout(vboxTopicStyle);

    QGroupBox* groupBoxMessageButtons = new QGroupBox("Bouton des messages", this);

    QVBoxLayout* vboxMessageButtons = new QVBoxLayout();
    vboxMessageButtons->addWidget(makeNewCheckBox("Ajouter un bouton pour citer un message", "showQuoteButton"));
    vboxMessageButtons->addWidget(makeNewCheckBox("Ajouter un bouton pour ignorer un pseudo", "showBlacklistButton"));
    vboxMessageButtons->addWidget(makeNewCheckBox("Ajouter un bouton pour éditer un message", "showEditButton"));
    vboxMessageButtons->addWidget(makeNewCheckBox("Ajouter un bouton pour supprimer un message", "showDeleteButton"));
    vboxMessageButtons->addStretch(1);
    groupBoxMessageButtons->setLayout(vboxMessageButtons);

    QGroupBox* groupBoxMessageStyle = new QGroupBox("Style des messages", this);

    QVBoxLayout* vboxMessageStyle = new QVBoxLayout();
    vboxMessageStyle->addWidget(makeNewCheckBox("Colorer le pseudo des modo/admin", "colorModoAndAdminPseudo"));
    vboxMessageStyle->addWidget(makeNewCheckBox("Colorer les PEMT", "colorPEMT"));
    vboxMessageStyle->addWidget(makeNewCheckBox("Colorer votre pseudo dans les messages", "colorUserPseudoInMessages"));
    vboxMessageStyle->addWidget(makeNewCheckBox("Améliorer les citations", "betterQuote"));
    vboxMessageStyle->addWidget(makeNewCheckBox("Récupérer le premier message du topic", "getFirstMessageOfTopic"));
    vboxMessageStyle->addLayout(makeNewSpinBox("Nombre de messages affichés au premier chargement", "numberOfMessageShowedFirstTime"));
    vboxMessageStyle->addStretch(1);
    groupBoxMessageStyle->setLayout(vboxMessageStyle);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(groupBoxTopicStyle, 0, 0, 1, 2);
    mainLayout->addWidget(groupBoxMessageButtons, 1, 0);
    mainLayout->addWidget(groupBoxMessageStyle, 1, 1);
    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);

    QVBoxLayout* realMainLayout = new QVBoxLayout();
    realMainLayout->addLayout(mainLayout);
    realMainLayout->addStretch(1);

    mainTabWidget->setLayout(realMainLayout);

    return mainTabWidget;
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
