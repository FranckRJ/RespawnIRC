#include <QSplitter>
#include <QHBoxLayout>

#include "containerForTopicsInfos.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

containerForTopicsInfosClass::containerForTopicsInfosClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) :
    QWidget(parent), showTopic(newListOfIgnoredPseudo, newListOfColorPseudo, currentThemeName), showForum(currentThemeName)
{
    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(&showTopic);
    splitter->addWidget(&showForum);
    splitter->setStretchFactor(0, 1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(splitter);
    layout->setMargin(0);

    setLayout(layout);

    if(settingToolClass::getThisBoolOption("showListOfTopic") == false || settingToolClass::getThisBoolOption("fastModeEnbled") == true)
    {
        showForum.setVisible(false);
    }

    connect(&showForum, &showForumClass::openThisTopic, this, &containerForTopicsInfosClass::topicNeedChanged);
    connect(&showForum, &showForumClass::openThisTopicInNewTab, this, &containerForTopicsInfosClass::openThisTopicInNewTab);
}

showTopicClass& containerForTopicsInfosClass::getShowTopic()
{
    return showTopic;
}

typeOfSaveForPseudo containerForTopicsInfosClass::getPseudoTypeOfSave() const
{
    return pseudoTypeOfSave;
}

QString containerForTopicsInfosClass::getTopicLinkFirstPage() const
{
    if(showTopic.getTopicLinkFirstPage().isEmpty() == true)
    {
        return bufferForTopicLinkFirstPage;
    }
    else
    {
        return showTopic.getTopicLinkFirstPage();
    }
}

void containerForTopicsInfosClass::setNewCookiesForInfo(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave)
{
    pseudoTypeOfSave = newTypeOfSave;

    showTopic.setNewCookies(newCookies, "www.jeuxvideo.com", newPseudoOfUser);
    showForum.setNewCookies(newCookies, "www.jeuxvideo.com");
}

void containerForTopicsInfosClass::setBufferForTopicLinkFirstPage(QString newLink)
{
    bufferForTopicLinkFirstPage = newLink;
}

void containerForTopicsInfosClass::updateSettingsForInfo()
{
    showTopic.updateSettingInfo();
    showForum.updateSettings();

    showForum.setVisible(showForum.getLoadNeeded());
}

void containerForTopicsInfosClass::setNewThemeForInfo(QString newThemeName)
{
    showTopic.setNewTheme(newThemeName);
    showForum.setNewTheme(newThemeName);
}

void containerForTopicsInfosClass::setNewTopicForInfo(QString newTopic)
{
    showTopic.setNewTopic(newTopic);
    showForum.setForumLink(parsingToolClass::getForumOfTopic(newTopic));
}
