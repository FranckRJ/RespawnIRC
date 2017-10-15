#ifndef IGNORELISTWINDOW_HPP
#define IGNORELISTWINDOW_HPP

#include <QWidget>
#include <QList>
#include <QString>
#include <QListView>
#include <QStringListModel>

#include "baseDialog.hpp"

class ignoreListWindowClass : public baseDialogClass
{
    Q_OBJECT
public:
    explicit ignoreListWindowClass(QList<QString>* newListOfIgnoredPseudo, QWidget* parent);
private:
    void updateList();
private slots:
    void addPseudo();
    void editCurrentPseudo();
    void removeCurrentPseudo();
    void addThisPseudo(QString newPseudo);
    void setCurrentPseudo(QString newPseudo);
signals:
    void listHasChanged();
private:
    QList<QString>* listOfIgnoredPseudo;
    QListView* viewListOfIgnoredPseudo;
    QStringListModel* modelForListView;
};

#endif
