#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>

#include "preferencesWindow.hpp"
#include "settingTool.hpp"

preferenceWindowClass::preferenceWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QTabWidget* mainWidget = new QTabWidget(this);

    QPushButton* buttonOK = new QPushButton("OK", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);
    QPushButton* buttonApply = new QPushButton("Appliquer", this);
    buttonApply->setEnabled(false);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(buttonOK);
    buttonLayout->addWidget(buttonCancel);
    buttonLayout->addWidget(buttonApply);

    QVBoxLayout* realMainLayout = new QVBoxLayout(this);
    realMainLayout->addWidget(mainWidget);
    realMainLayout->addLayout(buttonLayout);
    realMainLayout->setMargin(5);
    realMainLayout->setSizeConstraint(QLayout::SetFixedSize);

    mainWidget->addTab(createWidgetForMainTab(), "Général");
    mainWidget->addTab(createWidgetForMessageAndTopicStyleTab(), "Style des messages / topics");
    mainWidget->addTab(createWidgetForImageTab(), "Image");

    setLayout(realMainLayout);
    setWindowTitle("Préférences");

    QObject::connect(this, &preferenceWindowClass::setApplyButtonEnable, buttonApply, &QPushButton::setEnabled);
    QObject::connect(buttonOK, &QPushButton::pressed, this, &preferenceWindowClass::applySettingsAndClose);
    QObject::connect(buttonCancel, &QPushButton::pressed, this, &preferenceWindowClass::close);
    QObject::connect(buttonApply, &QPushButton::pressed, this, &preferenceWindowClass::applySettings);
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

    QGroupBox* groupBoxMiscellaneous = new QGroupBox("Divers", this);

    QVBoxLayout* vboxMiscellaneous = new QVBoxLayout();
    vboxMiscellaneous->addWidget(makeNewCheckBox("Vérifier l'orthographe", "useSpellChecker"));
    vboxMiscellaneous->addWidget(makeNewCheckBox("Changer la couleur du message lors de l'édition", "changeColorOnEdit"));
    vboxMiscellaneous->addWidget(makeNewCheckBox("Chercher les mises à jour au lancement", "searchForUpdateAtLaunch"));
    vboxMiscellaneous->addStretch(1);
    groupBoxMiscellaneous->setLayout(vboxMiscellaneous);

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
    mainLayout->addWidget(groupBoxMiscellaneous, 0, 1);
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
    vboxGeneral->addLayout(makeNewComboBox("Type de rafraichissement des images :", "typeOfImageRefresh", {"Manuel (lors d'une action de l'utilisateur)", "Optimisé (toutes en même temps)", "Rapide (une par une)"}));
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
    hboxForSpinBox->addWidget(newSpinBox, 1);

    QObject::connect(newSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &preferenceWindowClass::valueOfIntBoxChanged);

    return hboxForSpinBox;
}

QHBoxLayout* preferenceWindowClass::makeNewComboBox(QString messageInfo, QString boxNameValue, QStringList listOfChoices)
{
    QHBoxLayout* hboxForComboBox = new QHBoxLayout();
    QComboBox* newComboBox = new QComboBox(this);
    QLabel* newInfoForComboBox = new QLabel(messageInfo, this);

    newComboBox->setObjectName(boxNameValue);
    newComboBox->addItems(listOfChoices);
    newComboBox->setCurrentIndex(settingToolClass::getThisIntOption(boxNameValue).value);

    hboxForComboBox->addWidget(newInfoForComboBox);
    hboxForComboBox->addWidget(newComboBox, 1);

    QObject::connect(newComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &preferenceWindowClass::valueOfIntBoxChanged);

    return hboxForComboBox;
}

void preferenceWindowClass::valueOfCheckboxChanged(bool newVal)
{
    QObject* senderObject = sender();

    if(senderObject != nullptr)
    {
        emit setApplyButtonEnable(true);
        listOfBoolOptionChanged[senderObject->objectName()] = newVal;
    }
}

void preferenceWindowClass::valueOfIntBoxChanged(int newVal)
{
    QObject* senderObject = sender();

    if(senderObject != nullptr)
    {
        emit setApplyButtonEnable(true);
        listOfIntOptionChanged[senderObject->objectName()] = newVal;
    }
}

void preferenceWindowClass::applySettingsAndClose()
{
    applySettings();
    close();
}

void preferenceWindowClass::applySettings()
{
    emit newSettingsAvailable(listOfBoolOptionChanged, listOfIntOptionChanged);
    listOfBoolOptionChanged.clear();
    listOfIntOptionChanged.clear();
    emit setApplyButtonEnable(false);
}
