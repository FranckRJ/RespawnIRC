#ifndef CONTAINERFORTOPICSINFOS_HPP
#define CONTAINERFORTOPICSINFOS_HPP

#include <QtWidgets>

#include "showTopicMessages.hpp"
#include "showListOfTopic.hpp"

class containerForTopicsInfosClass : public QWidget
{
    Q_OBJECT
public:
    containerForTopicsInfosClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct> *newListOfColorPseudo, QString currentThemeName, QWidget* parent = 0);
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
