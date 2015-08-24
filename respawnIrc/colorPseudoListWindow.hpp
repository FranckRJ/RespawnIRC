#ifndef COLORPSEUDOLISTWINDOW_HPP
#define COLORPSEUDOLISTWINDOW_HPP

#include <QtWidgets>
#include <QtCore>

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
    void itemSelectedHasChanged(QModelIndex index);
    void valideColor();
signals:
    void listHasChanged();
private:
    QList<pseudoWithColorStruct>* listOfColorPseudo;
    QListView viewListOfColorPseudo;
    QStringListModel modelForListView;
    QSpinBox redBox;
    QSpinBox greenBox;
    QSpinBox blueBox;
};

#endif
