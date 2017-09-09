#include <QPushButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTabWidget>
#include <QObject>

#include "preferencesWindow.hpp"
#include "settingTool.hpp"

preferenceWindowClass::preferenceWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    expertMode = settingTool::getThisBoolOption("expertMode");

    typeOfPageLoad = new QComboBox(this);
    layoutTypeOfPageLoad = new QHBoxLayout();
    showTopicListEnabled = new QCheckBox(this);
    topicRefreshTimeNumber = new QSpinBox(this);
    layoutTopicRefreshTimeNumber = new QHBoxLayout();

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
    mainWidget->addTab(createWidgetForMessagesTab(), "Messages");
    mainWidget->addTab(createWidgetForTopicListTab(), "Liste des topics");
    mainWidget->addTab(createWidgetForImageTab(), "Image");

    if(settingTool::getThisBoolOption("fastModeEnbled") == true)
    {
        valueOfFastModeCheckBoxChanged(true);
    }

    setLayout(realMainLayout);
    setWindowTitle("Préférences");

    connect(this, &preferenceWindowClass::setApplyButtonEnable, buttonApply, &QPushButton::setEnabled);
    connect(buttonOK, &QPushButton::clicked, this, &preferenceWindowClass::applySettingsAndClose);
    connect(buttonCancel, &QPushButton::clicked, this, &preferenceWindowClass::close);
    connect(buttonApply, &QPushButton::clicked, this, &preferenceWindowClass::applySettings);
}

QWidget* preferenceWindowClass::createWidgetForMainTab()
{
    QWidget* mainTabWidget = new QWidget(this);

    QGroupBox* groupBoxAlert = new QGroupBox("Alerte", this);

    QVBoxLayout* vboxAlert = new QVBoxLayout();
    vboxAlert->addWidget(makeNewCheckBox("Beeper lors de la réception d'un MP", "beepForNewMP"));
    vboxAlert->addWidget(makeNewCheckBox("Beeper lors de la réception d'un message", "beepWhenWarn"));
    vboxAlert->addWidget(makeNewCheckBox("Avertir visuellement lors de la réception d'un message", "warnUser"));
    vboxAlert->addWidget(makeNewCheckBox("Avertir lors de l'édition d'un message", "warnWhenEdit"));
    vboxAlert->addWidget(makeNewCheckBox("Avertir lors de la première réception des messages", "warnOnFirstTime"));
    vboxAlert->addStretch(1);
    groupBoxAlert->setLayout(vboxAlert);

    QGroupBox* groupBoxMiscellaneous = new QGroupBox("Divers", this);

    QVBoxLayout* vboxMiscellaneous = new QVBoxLayout();
    vboxMiscellaneous->addWidget(makeNewCheckBox("Vérifier l'orthographe", "useSpellChecker"));
    vboxMiscellaneous->addWidget(makeNewCheckBox("Changer la couleur du message lors de l'édition", "changeColorOnEdit"));
    vboxMiscellaneous->addWidget(makeNewCheckBox("Utiliser RespawnIRC Navigator pour ouvrir les liens", "useInternalNavigatorForLinks"));
    vboxMiscellaneous->addWidget(makeNewCheckBox("Chercher les mises à jour au lancement", "searchForUpdateAtLaunch"));
    vboxMiscellaneous->addStretch(1);
    groupBoxMiscellaneous->setLayout(vboxMiscellaneous);

    QGroupBox* groupBoxWindowLayout = new QGroupBox("Agencement de la fenêtre", this);

    QVBoxLayout* vboxWindowLayout = new QVBoxLayout();
    vboxWindowLayout->addWidget(makeNewCheckBox("Sauvegarder la taille de la fenêtre", "saveWindowGeometry"));
    vboxWindowLayout->addWidget(makeNewCheckBox("Afficher les boutons de décoration de texte", "showTextDecorationButton"));
    vboxWindowLayout->addWidget(makeNewCheckBox("Afficher la liste des topics", "showListOfTopic", showTopicListEnabled));
    vboxWindowLayout->addWidget(makeNewCheckBox("Saisie du message en mode multiligne", "setMultilineEdit"));
    vboxWindowLayout->addLayout(makeNewSpinBox("Taille de la zone de saisie", "textBoxSize"));
    vboxWindowLayout->addStretch(1);
    groupBoxWindowLayout->setLayout(vboxWindowLayout);

    QGroupBox* groupBoxAdvanced = new QGroupBox("Avancé", this);

    QVBoxLayout* vboxAdvanced = new QVBoxLayout();
    vboxAdvanced->addLayout(makeNewComboBox("Type de chargement des pages :", "numberOfPageToLoadForOpti", {"Minimal (une page)", "Optimisé (entre une et deux pages)"}, typeOfPageLoad, layoutTypeOfPageLoad));
    vboxAdvanced->addWidget(makeFastModeCheckBox());
    vboxAdvanced->addWidget(makeNewCheckBox("Ignorer les erreurs réseau", "ignoreNetworkError"));
    vboxAdvanced->addLayout(makeNewSpinBox("Temps en secondes avant le timeout des requêtes", "timeoutInSecond"));
    vboxAdvanced->addLayout(makeNewSpinBox("Nombre d'erreurs avant avertissement", "numberOfErrorsBeforeWarning"));
    vboxAdvanced->addLayout(makeNewSpinBox("Taux de rafraichissement des topics", "updateTopicTime", topicRefreshTimeNumber, layoutTopicRefreshTimeNumber));
    vboxAdvanced->addLayout(makeNewSpinBox("Nombre maximal de quotes imbriquées", "maxNbOfQuotes"));
    vboxAdvanced->addStretch(1);
    groupBoxAdvanced->setLayout(vboxAdvanced);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(groupBoxAlert, 0, 0);
    mainLayout->addWidget(groupBoxMiscellaneous, 0, 1);
    mainLayout->addWidget(groupBoxWindowLayout, 1, 0);
    mainLayout->addWidget(groupBoxAdvanced, 1, 1);
    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);

    if(expertMode == true)
    {
        QGroupBox* groupBoxExpert = new QGroupBox("Expert", this);

        QVBoxLayout* vboxExpert = new QVBoxLayout();
        vboxExpert->addLayout(makeNewSpinBox("Nombre de messages avant début de l'optimisation", "numberOfMessagesForOptimizationStart"));
        vboxExpert->addStretch(1);
        groupBoxExpert->setLayout(vboxExpert);

        mainLayout->addWidget(groupBoxExpert, 2, 0, 1, 2);
    }

    QVBoxLayout* realMainLayout = new QVBoxLayout();
    realMainLayout->addLayout(mainLayout);
    realMainLayout->addStretch(1);

    mainTabWidget->setLayout(realMainLayout);

    return mainTabWidget;
}

QWidget* preferenceWindowClass::createWidgetForMessagesTab()
{
    QWidget* mainTabWidget = new QWidget(this);

    QGroupBox* groupBoxMessageButtons = new QGroupBox("Bouton des messages", this);

    QVBoxLayout* vboxMessageButtons = new QVBoxLayout();
    vboxMessageButtons->addWidget(makeNewCheckBox("Ajouter un bouton pour citer un message", "showQuoteButton"));
    vboxMessageButtons->addWidget(makeNewCheckBox("Désactiver le bouton citer pour soi-même", "disableSelfQuoteButton"));
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
    vboxMessageStyle->addWidget(makeNewCheckBox("Améliorer l'affichage des balises code", "betterCodeTag"));
    vboxMessageStyle->addStretch(1);
    groupBoxMessageStyle->setLayout(vboxMessageStyle);

    QGroupBox* groupBoxMessageBehavior = new QGroupBox("Comportement des messages", this);

    QVBoxLayout* vboxMessageBehavior = new QVBoxLayout();
    vboxMessageBehavior->addWidget(makeNewCheckBox("Récupérer le premier message du topic", "getFirstMessageOfTopic"));
    vboxMessageBehavior->addLayout(makeNewSpinBox("Nombre de messages affichés au premier chargement", "numberOfMessageShowedFirstTime"));
    vboxMessageBehavior->addLayout(makeNewComboBox("Type d'édition des messages :", "typeOfEdit", {"Ajout", "Ajout avec modification de l'ancien message", "Modification de l'ancien message"}));
    vboxMessageBehavior->addStretch(1);
    groupBoxMessageBehavior->setLayout(vboxMessageBehavior);

    QGroupBox* groupBoxThemesSettings = new QGroupBox("Options spécifiques aux thèmes", this);

    QVBoxLayout* vboxThemesSettings = new QVBoxLayout();
    vboxThemesSettings->addWidget(makeNewCheckBox("Afficher les signatures (si activé sur JVC)", "showSignatures"));
    vboxThemesSettings->addWidget(makeNewCheckBox("Afficher les avatars (si activé sur JVC)", "showAvatars"));
    vboxThemesSettings->addStretch(1);
    groupBoxThemesSettings->setLayout(vboxThemesSettings);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(groupBoxMessageButtons, 0, 0);
    mainLayout->addWidget(groupBoxMessageStyle, 0, 1);
    mainLayout->addWidget(groupBoxThemesSettings, 1, 0);
    mainLayout->addWidget(groupBoxMessageBehavior, 1, 1);
    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);

    QVBoxLayout* realMainLayout = new QVBoxLayout();
    realMainLayout->addLayout(mainLayout);
    realMainLayout->addStretch(1);

    mainTabWidget->setLayout(realMainLayout);

    return mainTabWidget;
}

QWidget* preferenceWindowClass::createWidgetForTopicListTab()
{
    QWidget* mainTabWidget = new QWidget(this);

    QGroupBox* groupBoxGeneral = new QGroupBox("Général", this);

    QVBoxLayout* vboxGeneral = new QVBoxLayout();
    vboxGeneral->addLayout(makeNewSpinBox("Taux de rafraichissement de la liste des topics", "updateTopicListTime"));
    vboxGeneral->addStretch(1);
    groupBoxGeneral->setLayout(vboxGeneral);

    QGroupBox* groupBoxTopicTag = new QGroupBox("Balise des topics", this);

    QVBoxLayout* vboxTopicTag = new QVBoxLayout();
    vboxTopicTag->addWidget(makeNewCheckBox("Afficher une balise pour les topics épinglés ouverts/fermés", "showPinnedTagOnTopicInTopicList"));
    vboxTopicTag->addWidget(makeNewCheckBox("Afficher une balise pour les topics multipages", "showHotTagOnTopicInTopicList"));
    vboxTopicTag->addWidget(makeNewCheckBox("Afficher une balise pour les topics fermés", "showLockTagOnTopicInTopicList"));
    vboxTopicTag->addWidget(makeNewCheckBox("Afficher une balise pour les topics résolus", "showResolvedTagOnTopicInTopicList"));
    vboxTopicTag->addWidget(makeNewCheckBox("Afficher une balise pour les topics supprimés", "showGhostTagOnTopicInTopicList"));
    vboxTopicTag->addWidget(makeNewCheckBox("Afficher une balise pour les topics normaux", "showNormalTagOnTopicInTopicList"));
    vboxTopicTag->addWidget(makeNewCheckBox("Utiliser des icônes au lieu des balises", "useIconInsteadOfTagInTopicList"));
    vboxTopicTag->addStretch(1);
    groupBoxTopicTag->setLayout(vboxTopicTag);

    QGroupBox* groupBoxTopicStyle = new QGroupBox("Style des topics", this);

    QVBoxLayout* vboxTopicStyle = new QVBoxLayout();
    vboxTopicStyle->addWidget(makeNewCheckBox("Colorer les topics des modo/admin", "colorModoAndAdminTopicInTopicList"));
    vboxTopicStyle->addWidget(makeNewCheckBox("Afficher le nombre de réponses dans la liste des topics", "showNumberOfMessagesInTopicList"));
    vboxTopicStyle->addWidget(makeNewCheckBox("Couper les longs noms de topics dans la liste des topics", "cutLongTopicNameInTopicList"));
    vboxTopicStyle->addLayout(makeNewSpinBox("Taille maximum des titres des topics", "topicNameMaxSizeInTopicList"));
    vboxTopicStyle->addStretch(1);
    groupBoxTopicStyle->setLayout(vboxTopicStyle);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(groupBoxGeneral, 0, 0, 1, 2);
    mainLayout->addWidget(groupBoxTopicTag, 1, 0);
    mainLayout->addWidget(groupBoxTopicStyle, 1, 1);
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

    QGroupBox* groupBoxNoelshack = new QGroupBox("Noelshack", this);

    QVBoxLayout* vboxNoelshack = new QVBoxLayout();
    vboxNoelshack->addWidget(makeNewCheckBox("Afficher les miniatures noelshack", "downloadNoelshackImages"));
    vboxNoelshack->addWidget(makeNewCheckBox("Cacher les images nuisibles", "hideUglyImages"));
    vboxNoelshack->addWidget(makeNewCheckBox("Améliorer le redimensionnement des miniatures noelshack", "smartNoelshackResizing"));
    vboxNoelshack->addLayout(makeNewSpinBox("Largeur des miniatures noelshack", "noelshackImageWidth"));
    vboxNoelshack->addLayout(makeNewSpinBox("Hauteur des miniatures noelshack", "noelshackImageHeight"));
    vboxNoelshack->addStretch(1);
    groupBoxNoelshack->setLayout(vboxNoelshack);

    QGroupBox* groupBoxAvatar = new QGroupBox("Avatar", this);

    QVBoxLayout* vboxAvatar = new QVBoxLayout();
    vboxAvatar->addLayout(makeNewSpinBox("Taille des avatars, si supporté par le thème", "avatarSize"));
    vboxAvatar->addWidget(makeNewCheckBox("Améliorer le redimensionnement des avatars\n(obligatoire pour le faire fonctionner avec certains thèmes)", "smartAvatarResizing"));
    /*TODO: vboxAvatar->addWidget(makeNewCheckBox("Télécharger les avatars en HD", "downloadHighDefAvatar"));*/
    vboxAvatar->addStretch(1);
    groupBoxAvatar->setLayout(vboxAvatar);

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(groupBoxGeneral, 0, 0, 1, 2);
    mainLayout->addWidget(groupBoxStickers, 1, 0);
    mainLayout->addWidget(groupBoxNoelshack, 1, 1);
    mainLayout->addWidget(groupBoxAvatar, 2, 0);
    mainLayout->setSizeConstraint(QLayout::SetMaximumSize);

    QVBoxLayout* realMainLayout = new QVBoxLayout();
    realMainLayout->addLayout(mainLayout);
    realMainLayout->addStretch(1);

    mainTabWidget->setLayout(realMainLayout);

    return mainTabWidget;
}

QCheckBox* preferenceWindowClass::makeNewCheckBox(QString messageInfo, QString boxNameValue, QCheckBox* useThisCheckBox)
{
    if(useThisCheckBox == nullptr)
    {
        useThisCheckBox = new QCheckBox(messageInfo, this);
    }
    else
    {
        useThisCheckBox->setText(messageInfo);
    }
    useThisCheckBox->setObjectName(boxNameValue);
    useThisCheckBox->setChecked(settingTool::getThisBoolOption(boxNameValue));

    connect(useThisCheckBox, &QCheckBox::toggled, this, &preferenceWindowClass::valueOfCheckboxChanged);

    return useThisCheckBox;
}

QHBoxLayout* preferenceWindowClass::makeNewSpinBox(QString messageInfo, QString boxNameValue, QSpinBox* useThisSpinBox, QHBoxLayout* useThisLayout)
{
    if(useThisSpinBox == nullptr)
    {
        useThisSpinBox = new QSpinBox(this);
    }
    if(useThisLayout == nullptr)
    {
        useThisLayout = new QHBoxLayout();
    }
    intSettingStruct infoForIntSetting = settingTool::getThisIntOption(boxNameValue);
    QLabel* newInfoForSpinBox = new QLabel(messageInfo +  " (entre " + QString::number(infoForIntSetting.minValue) + " et " + QString::number(infoForIntSetting.maxValue) + ") :", this);

    useThisSpinBox->setObjectName(boxNameValue);
    useThisSpinBox->setMinimum(infoForIntSetting.minValue);
    useThisSpinBox->setMaximum(infoForIntSetting.maxValue);
    useThisSpinBox->setValue(infoForIntSetting.value);

    useThisLayout->addWidget(newInfoForSpinBox);
    useThisLayout->addWidget(useThisSpinBox, 1);

    connect(useThisSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &preferenceWindowClass::valueOfIntBoxChanged);

    return useThisLayout;
}

QHBoxLayout* preferenceWindowClass::makeNewComboBox(QString messageInfo, QString boxNameValue, QStringList listOfChoices, QComboBox* useThisComboBox, QHBoxLayout* useThisLayout)
{
    if(useThisComboBox == nullptr)
    {
        useThisComboBox = new QComboBox(this);
    }
    if(useThisLayout == nullptr)
    {
        useThisLayout = new QHBoxLayout();
    }
    QLabel* newInfoForComboBox = new QLabel(messageInfo, this);

    useThisComboBox->setObjectName(boxNameValue);
    useThisComboBox->addItems(listOfChoices);
    useThisComboBox->setCurrentIndex(settingTool::getThisIntOption(boxNameValue).value);

    useThisLayout->addWidget(newInfoForComboBox);
    useThisLayout->addWidget(useThisComboBox, 1);

    connect(useThisComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &preferenceWindowClass::valueOfIntBoxChanged);

    return useThisLayout;
}

QCheckBox* preferenceWindowClass::makeFastModeCheckBox()
{
    QCheckBox* newCheckBox = makeNewCheckBox("Activer le mode rapide", "fastModeEnbled");
    connect(newCheckBox, &QCheckBox::toggled, this, &preferenceWindowClass::valueOfFastModeCheckBoxChanged);

    return newCheckBox;
}

void preferenceWindowClass::valueOfCheckboxChanged(bool newVal)
{
    QObject* senderObject = sender();

    if(senderObject != nullptr && updateValues == true)
    {
        emit setApplyButtonEnable(true);
        listOfBoolOptionChanged[senderObject->objectName()] = newVal;
    }
}

void preferenceWindowClass::valueOfIntBoxChanged(int newVal)
{
    QObject* senderObject = sender();

    if(senderObject != nullptr && updateValues == true)
    {
        emit setApplyButtonEnable(true);
        listOfIntOptionChanged[senderObject->objectName()] = newVal;
    }
}

void preferenceWindowClass::valueOfFastModeCheckBoxChanged(bool newVal)
{
    updateValues = false;
    if(newVal == true)
    {
        for(int i = 0; i < layoutTypeOfPageLoad->count(); ++i)
        {
            layoutTypeOfPageLoad->itemAt(i)->widget()->setEnabled(false);
        }
        for(int i = 0; i < layoutTopicRefreshTimeNumber->count(); ++i)
        {
            layoutTopicRefreshTimeNumber->itemAt(i)->widget()->setEnabled(false);
        }
        typeOfPageLoad->setEnabled(false);
        showTopicListEnabled->setEnabled(false);
        topicRefreshTimeNumber->setEnabled(false);
        typeOfPageLoad->setCurrentIndex(0);
        showTopicListEnabled->setChecked(false);
        topicRefreshTimeNumber->setMinimum(settingTool::fastModeSpeedRefresh);
        topicRefreshTimeNumber->setValue(settingTool::fastModeSpeedRefresh);
    }
    else
    {
        typeOfPageLoad->setCurrentIndex(settingTool::getThisIntOption("numberOfPageToLoadForOpti").value);
        showTopicListEnabled->setChecked(settingTool::getThisBoolOption("showListOfTopic"));
        topicRefreshTimeNumber->setMinimum(settingTool::getThisIntOption("updateTopicTime").minValue);
        topicRefreshTimeNumber->setValue(settingTool::getThisIntOption("updateTopicTime").value);
        showTopicListEnabled->setEnabled(true);
        typeOfPageLoad->setEnabled(true);
        topicRefreshTimeNumber->setEnabled(true);
        for(int i = 0; i < layoutTypeOfPageLoad->count(); ++i)
        {
            layoutTypeOfPageLoad->itemAt(i)->widget()->setEnabled(true);
        }
        for(int i = 0; i < layoutTopicRefreshTimeNumber->count(); ++i)
        {
            layoutTopicRefreshTimeNumber->itemAt(i)->widget()->setEnabled(true);
        }
    }
    updateValues = true;
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
