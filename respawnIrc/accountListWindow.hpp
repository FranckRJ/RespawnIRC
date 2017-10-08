#ifndef ACCOUNTLISTWINDOW_HPP
#define ACCOUNTLISTWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QNetworkCookie>
#include <QList>
#include <QString>
#include <QListView>
#include <QStringListModel>
#include <QCheckBox>

struct accountStruct
{
    QString pseudo;
    QNetworkCookie connectCookie;
};

class accountListWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit accountListWindowClass(QList<accountStruct>* newListOfAccount, QWidget* parent);
    static void addOrUpdateAcountInThisList(QNetworkCookie newConnectCookie, QString newPseudoOfUser, QList<accountStruct>* thisList);
private:
    void updateList();
private slots:
    void showConnectWindow();
    void addAccount(QNetworkCookie newConnectCookie, QString newPseudoOfUser, bool saveInfo);
    void removeCurrentAccount();
    void connectWithThisAccount();
    void connectToOneTopicWithThisAccount();
signals:
    void listHasChanged();
    void useThisAccount(QNetworkCookie withThisCookie, QString withThisPseudo, bool saveAccountList, bool savePseudo);
    void useThisAccountForOneTopic(QNetworkCookie withThisCookie, QString withThisPseudo, bool savePseudo);
    void eraseThisPseudo(QString thisPseudo);
private:
    QList<accountStruct>* listOfAccount;
    QListView* viewListOfAccount;
    QStringListModel* modelForListView;
    QCheckBox* rememberBox;
};

#endif
