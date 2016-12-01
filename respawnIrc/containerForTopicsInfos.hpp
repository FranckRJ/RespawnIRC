#ifndef CONTAINERFORTOPICSINFOS_HPP
#define CONTAINERFORTOPICSINFOS_HPP

#include <QWidget>
#include <QList>
#include <QString>
#include <QNetworkCookie>

#include "showTopicMessages.hpp"
#include "showListOfTopic.hpp"
#include "colorPseudoListWindow.hpp"

enum class typeOfSaveForPseudo { DONT_REMEMBER, DEFAULT, REMEMBER };

class containerForTopicsInfosClass : public QWidget
{
    Q_OBJECT
public:
    explicit containerForTopicsInfosClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent = 0);
    showTopicMessagesClass& getShowTopicMessages();
    typeOfSaveForPseudo getPseudoTypeOfSave();
    QString getTopicLinkFirstPage();
    void setNewCookiesForInfo(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave);
    void setBufferForTopicLinkFirstPage(QString newLink);
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
    typeOfSaveForPseudo pseudoTypeOfSave = typeOfSaveForPseudo::DEFAULT;
    QString bufferForTopicLinkFirstPage;

};

#endif
