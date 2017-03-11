#ifndef IGNORELISTWINDOW_HPP
#define IGNORELISTWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QList>
#include <QString>
#include <QListView>
#include <QStringListModel>

class ignoreListWindowClass : public QDialog
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
    QListView viewListOfIgnoredPseudo;
    QStringListModel modelForListView;
};

#endif
