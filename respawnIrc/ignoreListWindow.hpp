#ifndef IGNORELISTWINDOW_HPP
#define IGNORELISTWINDOW_HPP

#include <QtGui>
#include <QtCore>

class ignoreListWindowClass : public QDialog
{
    Q_OBJECT
public:
    ignoreListWindowClass(QList<QString>* newListOfIgnoredPseudo, QWidget* parent);
    void updateList();
public slots:
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
