#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QScrollBar>
#include <QAbstractItemView>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "manageShortcutWindow.hpp"
#include "addShortcutWindow.hpp"
#include "shortcutTool.hpp"

namespace
{
    const int BASE_NUMBER = 0;
    const int REPLACEMENT_NUMBER = 1;
}

manageShortcutWindowClass::manageShortcutWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    shortcutsListView = new QTreeView(this);
    shortcutsItemModel = new QStandardItemModel(shortcutsListView);
    shortcutsListView->setModel(shortcutsItemModel);
    shortcutsListView->header()->setDefaultSectionSize(shortcutsListView->header()->defaultSectionSize() * 1.5);
    shortcutsListView->setRootIsDecorated(false);
    shortcutsListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QPushButton* addButton = new QPushButton("Ajouter", this);
    QPushButton* editButton = new QPushButton("Editer", this);
    QPushButton* removeButton = new QPushButton("Supprimer", this);
    QPushButton* upButton = new QPushButton("Monter", this);
    QPushButton* downButton = new QPushButton("Baisser", this);

    QHBoxLayout* usesButtonsLayout = new QHBoxLayout();
    usesButtonsLayout->addWidget(addButton);
    usesButtonsLayout->addWidget(editButton);
    usesButtonsLayout->addWidget(removeButton);
    usesButtonsLayout->addWidget(upButton);
    usesButtonsLayout->addWidget(downButton);

    textZone = new QTextEdit(this);
    previewZone = new QTextEdit(this);
    textZone->setMaximumHeight(65);
    textZone->setPlaceholderText("Écrivez votre message ici...");
    textZone->setAcceptRichText(false);
    textZone->setTabChangesFocus(true);
    previewZone->setMaximumHeight(65);
    previewZone->setPlaceholderText("Prévisualisation...");
    previewZone->setAcceptRichText(false);
    previewZone->setReadOnly(true);
    previewZone->setTabChangesFocus(true);

    QPushButton* validateButton = new QPushButton("Sauvegarder et fermer", this);
    QPushButton* cancelButton = new QPushButton("Annuler les modifications", this);

    QHBoxLayout* endsButtonsLayout = new QHBoxLayout();
    endsButtonsLayout->addWidget(validateButton);
    endsButtonsLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(shortcutsListView);
    mainLayout->addLayout(usesButtonsLayout);
    mainLayout->addWidget(textZone);
    mainLayout->addWidget(previewZone);
    mainLayout->addLayout(endsButtonsLayout);

    setLayout(mainLayout);
    setWindowTitle("Gérer les raccourcis");

    connect(addButton, &QPushButton::clicked, this, &manageShortcutWindowClass::showAddShortcutWindow);
    connect(editButton, &QPushButton::clicked, this, &manageShortcutWindowClass::showEditShortcutWindow);
    connect(removeButton, &QPushButton::clicked, this, &manageShortcutWindowClass::removeShortcut);
    connect(upButton, &QPushButton::clicked, this, &manageShortcutWindowClass::moveUpShortcut);
    connect(downButton, &QPushButton::clicked, this, &manageShortcutWindowClass::moveDownShortcut);
    connect(textZone, &QTextEdit::textChanged, this, &manageShortcutWindowClass::textInTextZoneChanged);
    connect(validateButton, &QPushButton::clicked, this, &manageShortcutWindowClass::validateShortcuts);
    connect(cancelButton, &QPushButton::clicked, this, &manageShortcutWindowClass::close);

    initializeShortcutsModel();
}

void manageShortcutWindowClass::initializeShortcutsModel()
{
    const QList<shortcutInfosStruct>* listOfShortcuts = shortcutTool::getListOfShortcutsForRule("shortcut");

    shortcutsItemModel->clear();
    shortcutsItemModel->setHorizontalHeaderLabels(QStringList({"Base", "Remplacement"}));

    for(const shortcutInfosStruct& thisShortcut : *listOfShortcuts)
    {
        addRowToModel(thisShortcut.base, thisShortcut.replacement);
    }

    shortcutsListView->setCurrentIndex(shortcutsItemModel->index(-1, -1));
    updateShortcutRuleAndPreview();
}

void manageShortcutWindowClass::addRowToModel(QString base, QString replacement)
{
    QStandardItem* baseItem = new QStandardItem(base);
    QStandardItem* replacementItem = new QStandardItem(replacement);

    shortcutsItemModel->appendRow({baseItem, replacementItem});
}

void manageShortcutWindowClass::updateShortcutRuleAndPreview()
{
    shortcutRuleStruct newShortcutRule;
    newShortcutRule.needToUseRegex = false;

    for(int i = 0; i < shortcutsItemModel->rowCount(); ++i)
    {
        shortcutInfosStruct newShortcutInfo;
        newShortcutInfo.base = shortcutsItemModel->item(i, BASE_NUMBER)->text();
        newShortcutInfo.replacement = shortcutsItemModel->item(i, REPLACEMENT_NUMBER)->text();
        newShortcutRule.listOfShortcuts.push_back(newShortcutInfo);
    }

    shortcutTool::setThisShortcutRule("tmp_shortcut", newShortcutRule);
    textInTextZoneChanged();
}

void manageShortcutWindowClass::addNewShortcut(QString base, QString replacement)
{
    addRowToModel(base, replacement);
    shortcutsListView->setCurrentIndex(shortcutsItemModel->index(shortcutsItemModel->rowCount() - 1, BASE_NUMBER));
    updateShortcutRuleAndPreview();
}

void manageShortcutWindowClass::setCurrentShortcut(QString base, QString replacement)
{
    int selectedRow = shortcutsListView->currentIndex().row();
    if(selectedRow != -1)
    {
        shortcutsItemModel->item(selectedRow, BASE_NUMBER)->setText(base);
        shortcutsItemModel->item(selectedRow, REPLACEMENT_NUMBER)->setText(replacement);
        updateShortcutRuleAndPreview();
    }
}

void manageShortcutWindowClass::showAddShortcutWindow()
{
    addShortcutWindowClass* myAddShortcutWindow = new addShortcutWindowClass(this);
    connect(myAddShortcutWindow, &addShortcutWindowClass::newShortcutSet, this, &manageShortcutWindowClass::addNewShortcut);
    myAddShortcutWindow->exec();
}

void manageShortcutWindowClass::showEditShortcutWindow()
{
    int selectedRow = shortcutsListView->currentIndex().row();
    if(selectedRow != -1)
    {
        addShortcutWindowClass* myAddShortcutWindow = new addShortcutWindowClass(this,shortcutsItemModel->item(selectedRow, BASE_NUMBER)->text(), shortcutsItemModel->item(selectedRow, REPLACEMENT_NUMBER)->text());
        connect(myAddShortcutWindow, &addShortcutWindowClass::newShortcutSet, this, &manageShortcutWindowClass::setCurrentShortcut);
        myAddShortcutWindow->exec();
    }
}

void manageShortcutWindowClass::removeShortcut()
{
    if(shortcutsListView->currentIndex().row() != -1)
    {
        shortcutsItemModel->removeRow(shortcutsListView->currentIndex().row());
        shortcutsListView->setCurrentIndex(shortcutsItemModel->index(-1, -1));
        updateShortcutRuleAndPreview();
    }
}

void manageShortcutWindowClass::moveUpShortcut()
{
    int selectedRow = shortcutsListView->currentIndex().row();
    if(selectedRow > 0)
    {
        shortcutsItemModel->insertRow(selectedRow - 1, shortcutsItemModel->takeRow(selectedRow));
        shortcutsListView->setCurrentIndex(shortcutsItemModel->index(selectedRow - 1, BASE_NUMBER));
        updateShortcutRuleAndPreview();
    }
}

void manageShortcutWindowClass::moveDownShortcut()
{
    int selectedRow = shortcutsListView->currentIndex().row();
    if(selectedRow < shortcutsItemModel->rowCount() - 1 && selectedRow != -1)
    {
        shortcutsItemModel->insertRow(selectedRow + 1, shortcutsItemModel->takeRow(selectedRow));
        shortcutsListView->setCurrentIndex(shortcutsItemModel->index(selectedRow + 1, BASE_NUMBER));
        updateShortcutRuleAndPreview();
    }
}

void manageShortcutWindowClass::textInTextZoneChanged()
{
    int oldScrollValue = previewZone->verticalScrollBar()->value();
    previewZone->setText(shortcutTool::transformMessage(textZone->toPlainText(), "tmp_shortcut"));
    previewZone->verticalScrollBar()->setValue(oldScrollValue);
}

void manageShortcutWindowClass::validateShortcuts()
{
    QFile thisFile(QCoreApplication::applicationDirPath() + "/resources/shortcut.txt");

    if(thisFile.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream textStream(&thisFile);

        for(int i = 0; i < shortcutsItemModel->rowCount(); ++i)
        {
            textStream << shortcutsItemModel->item(i, BASE_NUMBER)->text() << " ";
            textStream << shortcutsItemModel->item(i, REPLACEMENT_NUMBER)->text() << "\n";
        }
    }

    shortcutTool::initializeAllShortcutsRules();
    close();
}
