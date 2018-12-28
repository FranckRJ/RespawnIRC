#ifndef CONNECTWINDOW_HPP
#define CONNECTWINDOW_HPP

#include <QWidget>
#include <QNetworkCookie>
#include <QWebEngineView>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QString>

#include "baseDialog.hpp"

class connectWindowClass : public baseDialogClass
{
    Q_OBJECT
public:
    explicit connectWindowClass(QWidget* parent, bool showRemeberBox = true);
private slots:
    void addWebView();
    void checkThisCookie(QNetworkCookie cookie);
    void showAddCookieWindow();
    void addCookieManually(QString newConnectCookie);
    void valideConnect();
    void showHelpConnect();
signals:
    void newCookieAvailable(QNetworkCookie newConnectCookie, QString newPseudoOfUser, bool saveAccountList, bool savePseudo);
private:
    bool firstTimeAdjustSize = true;
    QNetworkCookie connectCookie;
    QWebEngineView* webView = nullptr;
    QPushButton* buttonShowWebView;
    QVBoxLayout* mainLayout;
    QLineEdit* pseudoLine;
    QCheckBox* rememberBox;
};

#endif
