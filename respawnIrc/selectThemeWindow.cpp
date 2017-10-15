#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDir>
#include <QCoreApplication>
#include <QStringList>
#include <QItemSelectionModel>

#include "selectThemeWindow.hpp"

selectThemeWindowClass::selectThemeWindowClass(QString newCurrentThemeName, QWidget* parent) : baseDialogClass(parent)
{
    QLabel* labTheme = new QLabel("Thème actuel :", this);
    QPushButton* buttonSelect = new QPushButton("Sélectionner ce thème", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);
    currentThemeName = new QLabel(this);
    listViewOfTheme = new QListView(this);
    modelForListViewOfTheme = new QStringListModel(listViewOfTheme);

    viewThemeInfos = new viewThemeInfosClass(this);

    listViewOfTheme->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* labLayout = new QHBoxLayout;
    labLayout->addWidget(labTheme, 0, Qt::AlignLeft);
    labLayout->addWidget(currentThemeName, 0, Qt::AlignRight);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(listViewOfTheme, 0, 0);
    mainLayout->addWidget(viewThemeInfos, 0, 1);
    mainLayout->addLayout(labLayout, 1, 0);
    mainLayout->addWidget(buttonSelect, 2, 0, 1, 2);
    mainLayout->addWidget(buttonCancel, 3, 0, 1, 2);

    if(newCurrentThemeName.isEmpty() == true)
    {
        currentThemeName->setText("Défaut");
    }
    else
    {
        currentThemeName->setText(newCurrentThemeName);
    }

    setLayout(mainLayout);
    setWindowTitle("Sélectionner un thème");
    listViewOfTheme->setModel(modelForListViewOfTheme);

    connect(buttonSelect, &QPushButton::clicked, this, &selectThemeWindowClass::selectThisTheme);
    connect(buttonCancel, &QPushButton::clicked, this, &selectThemeWindowClass::close);
    connect(listViewOfTheme->selectionModel(), &QItemSelectionModel::currentChanged, this, &selectThemeWindowClass::changeThemeInfos);

    loadListOfThemes();
    listViewOfTheme->setFocus();
}

void selectThemeWindowClass::loadListOfThemes()
{
    QDir themeDir(QCoreApplication::applicationDirPath() + "/themes/");
    QStringList listOfTheme;

    if(themeDir.exists() == true)
    {
        listOfTheme = themeDir.entryList(QDir::Dirs);
        listOfTheme.removeFirst();
        listOfTheme.removeFirst();
    }

    listOfTheme.push_front("Défaut");
    modelForListViewOfTheme->setStringList(listOfTheme);

    /* Valide même si indexof renvoi -1 car il faut associer l'index actuel à un index invalide dans ce cas */
    listViewOfTheme->setCurrentIndex(modelForListViewOfTheme->index(listOfTheme.indexOf(currentThemeName->text())));

    changeThemeInfos(listViewOfTheme->currentIndex());
}

void selectThemeWindowClass::selectThisTheme()
{
    if(listViewOfTheme->currentIndex().row() == 0)
    {
        emit newThemeSelected("");
        close();
    }
    else if(listViewOfTheme->currentIndex().row() != -1)
    {
        emit newThemeSelected(modelForListViewOfTheme->index(listViewOfTheme->currentIndex().row()).data().toString());
        close();
    }
}

void selectThemeWindowClass::changeThemeInfos(const QModelIndex& selection)
{
    if(selection.row() != -1)
    {
        viewThemeInfos->setThemeToShow(modelForListViewOfTheme->index(selection.row()).data().toString());
    }
    else
    {
        viewThemeInfos->setThemeToShow("");
    }
}
