#include <QSplitter>
#include <QHBoxLayout>

#include "containerForTopicsInfos.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

containerForTopicsInfosClass::containerForTopicsInfosClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) :
    QWidget(parent), showTopicMessages(newListOfIgnoredPseudo, newListOfColorPseudo, currentThemeName), showListOfTopic(currentThemeName)
{
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(&showTopicMessages);
    splitter->addWidget(&showListOfTopic);
    splitter->setStretchFactor(0, 1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(splitter);
    layout->setMargin(0);

    setLayout(layout);

    if(settingToolClass::getThisBoolOption("showListOfTopic") == false || settingToolClass::getThisBoolOption("fastModeEnbled") == true)
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

typeOfSaveForPseudo containerForTopicsInfosClass::getPseudoTypeOfSave()
{
    return pseudoTypeOfSave;
}

QString containerForTopicsInfosClass::getTopicLinkFirstPage()
{
    if(showTopicMessages.getTopicLinkFirstPage().isEmpty() == true)
    {
        return bufferForTopicLinkFirstPage;
    }
    else
    {
        return showTopicMessages.getTopicLinkFirstPage();
    }
}

void containerForTopicsInfosClass::setNewCookiesForInfo(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave)
{
    pseudoTypeOfSave = newTypeOfSave;

    showTopicMessages.setNewCookies(newCookies, "www.jeuxvideo.com", newPseudoOfUser);
    showListOfTopic.setNewCookies(newCookies, "www.jeuxvideo.com");
}

void containerForTopicsInfosClass::setBufferForTopicLinkFirstPage(QString newLink)
{
    bufferForTopicLinkFirstPage = newLink;
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
    showListOfTopic.setNewTheme(newThemeName);
}

void containerForTopicsInfosClass::setNewTopicForInfo(QString newTopic)
{
    showTopicMessages.setNewTopic(newTopic);
    showListOfTopic.setForumLink(parsingToolClass::getForumOfTopic(newTopic));
}
