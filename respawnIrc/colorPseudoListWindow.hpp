#ifndef COLORPSEUDOLISTWINDOW_HPP
#define COLORPSEUDOLISTWINDOW_HPP

#include <QDialog>
#include <QListView>
#include <QStringListModel>

struct pseudoWithColorStruct
{
    QString pseudo;
    int red;
    int green;
    int blue;
};

class colorPseudoListWindowClass : public QDialog
{
    Q_OBJECT
public:
    colorPseudoListWindowClass(QList<pseudoWithColorStruct>* newListOfColorPseudo, QWidget* parent);
    bool addPseudoToColorPseudoList(QString newPseudo, bool reallyAddPseudoToList = true);
    void updateList();
public slots:
    void addPseudo();
    void editCurrentPseudo();
    void removeCurrentPseudo();
    void addThisPseudo(QString newPseudo);
    void setCurrentPseudo(QString newPseudo);
    void chooseColor();
signals:
    void listHasChanged();
private:
    QList<pseudoWithColorStruct>* listOfColorPseudo;
    QListView viewListOfColorPseudo;
    QStringListModel modelForListView;
};

#endif
