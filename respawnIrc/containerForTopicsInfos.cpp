#include <QSplitter>
#include <QHBoxLayout>

#include "containerForTopicsInfos.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

containerForTopicsInfosClass::containerForTopicsInfosClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) :
    QWidget(parent), showTopicMessages(newListOfIgnoredPseudo, newListOfColorPseudo, currentThemeName)
{
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(&showTopicMessages);
    splitter->addWidget(&showListOfTopic);
    splitter->setStretchFactor(0, 1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(splitter);
    layout->setMargin(0);

    setLayout(layout);

    if(settingToolClass::getThisBoolOption("showListOfTopic") == false)
    {
        showListOfTopic.setVisible(false);
    }

    connect(&showListOfTopic, &showListOfTopicClass::openThisTopic, this, &containerForTopicsInfosClass::topicNeedChanged);
    connect(&showListOfTopic, &showListOfTopicClass::openThisTopicInNewTab, this, &containerForTopicsInfosClass::openThisTopicInNewTab);
}

showTopicMessagesClass& containerForTopicsInfosClass::getShowTopicMessages()
{
    return showTopicMessages;
}

void containerForTopicsInfosClass::setNewCookiesForInfo(QList<QNetworkCookie> newCookies, QString newPseudoOfUser)
{
    showTopicMessages.setNewCookies(newCookies, newPseudoOfUser);
    showListOfTopic.setNewCookies(newCookies);
}

void containerForTopicsInfosClass::updateSettingsForInfo()
{
    showTopicMessages.updateSettingInfo();
    showListOfTopic.updateSettings();

    showListOfTopic.setVisible(showListOfTopic.getLoadNeeded());
}

void containerForTopicsInfosClass::setNewThemeForInfo(QString newThemeName)
{
    showTopicMessages.setNewTheme(newThemeName);
}

void containerForTopicsInfosClass::setNewTopicForInfo(QString newTopic)
{
    showTopicMessages.setNewTopic(newTopic);
    showListOfTopic.setForumLink(parsingToolClass::getForumOfTopic(newTopic));
}
