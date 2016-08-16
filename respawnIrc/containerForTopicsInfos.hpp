#ifndef CONTAINERFORTOPICSINFOS_HPP
#define CONTAINERFORTOPICSINFOS_HPP

#include <QWidget>
#include <QList>
#include <QString>
#include <QNetworkCookie>

#include "showTopicMessages.hpp"
#include "showListOfTopic.hpp"
#include "colorPseudoListWindow.hpp"

class containerForTopicsInfosClass : public QWidget
{
    Q_OBJECT
public:
    explicit containerForTopicsInfosClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent = 0);
    showTopicMessagesClass& getShowTopicMessages();
    void setNewCookiesForInfo(QList<QNetworkCookie> newCookies, QString newPseudoOfUser);
    void updateSettingsForInfo();
public slots:
    void setNewThemeForInfo(QString newThemeName);
    void setNewTopicForInfo(QString newTopic);
signals:
    void topicNeedChanged(QString topicLink);
    void openThisTopicInNewTab(QString topicLink);
private:
    showTopicMessagesClass showTopicMessages;
    showListOfTopicClass showListOfTopic;
};

#endif
