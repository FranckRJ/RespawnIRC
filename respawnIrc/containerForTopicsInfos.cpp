#include <QSplitter>
#include <QHBoxLayout>

#include "containerForTopicsInfos.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

containerForTopicsInfosClass::containerForTopicsInfosClass(const QList<QString>* newListOfIgnoredPseudo, const QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) :
    QWidget(parent)
{
    showTopic = new showTopicClass(newListOfIgnoredPseudo, newListOfColorPseudo, currentThemeName, this);
    showForum = new showForumClass(currentThemeName, this);

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(showTopic);
    splitter->addWidget(showForum);
    splitter->setStretchFactor(0, 1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(splitter);
    layout->setMargin(0);

    setLayout(layout);

    if(settingTool::getThisBoolOption("showListOfTopic") == false || settingTool::getThisBoolOption("fastModeEnbled") == true)
    {
        showForum->setVisible(false);
    }

    connect(showForum, &showForumClass::openThisTopic, this, &containerForTopicsInfosClass::topicNeedChanged);
    connect(showForum, &showForumClass::openThisTopicInNewTab, this, &containerForTopicsInfosClass::openThisTopicInNewTab);
}

const showTopicClass& containerForTopicsInfosClass::getConstShowTopic() const
{
    return *showTopic;
}

showTopicClass& containerForTopicsInfosClass::getShowTopic()
{
    return *showTopic;
}

typeOfSaveForPseudo containerForTopicsInfosClass::getPseudoTypeOfSave() const
{
    return pseudoTypeOfSave;
}

QString containerForTopicsInfosClass::getTopicLinkFirstPage() const
{
    if(showTopic->getTopicLinkFirstPage().isEmpty() == true)
    {
        return bufferForTopicLinkFirstPage;
    }
    else
    {
        return showTopic->getTopicLinkFirstPage();
    }
}

void containerForTopicsInfosClass::setNewCookiesForInfo(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave)
{
    pseudoTypeOfSave = newTypeOfSave;

    showTopic->setNewCookies(newCookies, "www.jeuxvideo.com", newPseudoOfUser);
    showForum->setNewCookies(newCookies, "www.jeuxvideo.com");
}

void containerForTopicsInfosClass::setBufferForTopicLinkFirstPage(QString newLink)
{
    bufferForTopicLinkFirstPage = newLink;
}

void containerForTopicsInfosClass::updateSettingsForInfo()
{
    showTopic->updateSettingInfo();
    showForum->updateSettings();

    showForum->setVisible(showForum->getLoadNeeded());
}

void containerForTopicsInfosClass::setNewThemeForInfo(QString newThemeName)
{
    showTopic->setNewTheme(newThemeName);
    showForum->setNewTheme(newThemeName);
}

void containerForTopicsInfosClass::setNewTopicForInfo(QString newTopic)
{
    showTopic->setNewTopic(newTopic);
    showForum->setForumLink(parsingTool::getForumOfTopic(newTopic));
}
