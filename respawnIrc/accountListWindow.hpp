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
    QList<QNetworkCookie> listOfCookie;
};

class accountListWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit accountListWindowClass(QList<accountStruct>* newListOfAccount, QWidget* parent);
    static bool addAcountToThisList(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, QList<accountStruct>* thisList);
    void updateList();
public slots:
    void showConnectWindow();
    void addAccount(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo);
    void removeCurrentAccount();
    void connectWithThisAccount();
    void connectToOneTopicWithThisAccount();
signals:
    void listHasChanged();
    void useThisAccount(QList<QNetworkCookie> withTheseCookie, QString withThisPseudo, bool saveAccountList, bool savePseudo);
    void useThisAccountForOneTopic(QList<QNetworkCookie> withTheseCookie, QString withThisPseudo, bool savePseudo);
    void eraseThisPseudo(QString thisPseudo);
private:
    QList<accountStruct>* listOfAccount;
    QListView viewListOfAccount;
    QStringListModel modelForListView;
    QCheckBox rememberBox;
};

#endif
