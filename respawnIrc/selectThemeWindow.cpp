#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDir>
#include <QCoreApplication>
#include <QStringList>

#include "selectThemeWindow.hpp"

selectThemeWindowClass::selectThemeWindowClass(QString newCurrentThemeName, QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel* labTheme = new QLabel("Thème actuel :", this);
    QPushButton* buttonSelect = new QPushButton("Choisir ce theme", this);
    QPushButton* buttonCancel = new QPushButton("Annuler", this);

    listViewOfTheme.setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* labLayout = new QHBoxLayout;
    labLayout->addWidget(labTheme, 0, Qt::AlignLeft);
    labLayout->addWidget(&currentThemeName, 0, Qt::AlignRight);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&listViewOfTheme, 0, 0);
    mainLayout->addLayout(labLayout, 1, 0);
    mainLayout->addWidget(buttonSelect, 2, 0);
    mainLayout->addWidget(buttonCancel, 3, 0);

    if(newCurrentThemeName.isEmpty() == true)
    {
        currentThemeName.setText("Défaut");
    }
    else
    {
        currentThemeName.setText(newCurrentThemeName);
    }

    setLayout(mainLayout);
    setWindowTitle("Choisir un thème");
    listViewOfTheme.setModel(&modelForListViewOfTheme);

    connect(buttonSelect, &QPushButton::pressed, this, &selectThemeWindowClass::selectThisTheme);
    connect(buttonCancel, &QPushButton::pressed, this, &selectThemeWindowClass::close);

    loadListOfThemes();
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
    modelForListViewOfTheme.setStringList(listOfTheme);
}

void selectThemeWindowClass::selectThisTheme()
{
    if(listViewOfTheme.currentIndex().row() != -1)
    {
        emit newThemeSelected(modelForListViewOfTheme.index(listViewOfTheme.currentIndex().row()).data().toString());
        close();
    }
}
