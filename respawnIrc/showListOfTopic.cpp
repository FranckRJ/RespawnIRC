#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QStandardItem>
#include <QCoreApplication>

#include "showListOfTopic.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

showListOfTopicClass::showListOfTopicClass(QString currentThemeName, QWidget* parent) : QWidget(parent)
{
    pinnedOnTagImage.load(QCoreApplication::applicationDirPath() + "/resources/topic-marque-on.png");
    pinnedOffTagImage.load(QCoreApplication::applicationDirPath() + "/resources/topic-marque-off.png");
    hotTagImage.load(QCoreApplication::applicationDirPath() + "/resources/topic-dossier2.png");
    lockTagImage.load(QCoreApplication::applicationDirPath() + "/resources/topic-lock.png");
    resolvedTagImage.load(QCoreApplication::applicationDirPath() + "/resources/topic-resolu.png");
    normalTagImage.load(QCoreApplication::applicationDirPath() + "/resources/topic-dossier1.png");

    listViewOfTopic.setModel(&modelForListView);
    listViewOfTopic.setEditTriggers(QAbstractItemView::NoEditTriggers);
    listViewOfTopic.setContextMenuPolicy(Qt::CustomContextMenu);
    timerForGetList.setTimerType(Qt::CoarseTimer);
    timerForGetList.setInterval(5000);
    updateSettings();
    setNewTheme(currentThemeName);

    networkManager = new QNetworkAccessManager(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&listViewOfTopic);
    layout->setMargin(0);

    setLayout(layout);

    connect(&listViewOfTopic, &QListView::customContextMenuRequested, this, &showListOfTopicClass::createContextMenu);
    connect(&listViewOfTopic, &QListView::doubleClicked, this, &showListOfTopicClass::clickedOnLink);
    connect(&timerForGetList, &QTimer::timeout, this, &showListOfTopicClass::startGetListOfTopic);
}

bool showListOfTopicClass::getLoadNeeded()
{
    return loadNeeded;
}

void showListOfTopicClass::setForumLink(QString newForumLink)
{
    if(reply != nullptr)
    {
        if(reply->isRunning())
        {
            reply->abort();
        }
    }

    forumLink = newForumLink;
    websiteOfForum = parsingToolClass::getWebsite(forumLink);

    modelForListView.clear();
    if(newForumLink.isEmpty() == false)
    {
        if(loadNeeded == true)
        {
            startGetListOfTopic();
            timerForGetList.start();
        }
    }
    else
    {
        timerForGetList.stop();
    }
}

void showListOfTopicClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newWebsiteOfCookies)
{
    currentCookieList = newCookies;
    websiteOfCookies = newWebsiteOfCookies;
    if(networkManager != nullptr)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://" + websiteOfCookies));
    }
}

void showListOfTopicClass::updateSettings()
{
    intSettingStruct updateTopicListTimeSetting = settingToolClass::getThisIntOption("updateTopicListTime");

    showNumberOfMessages = settingToolClass::getThisBoolOption("showNumberOfMessagesInTopicList");
    cutLongTopicName = settingToolClass::getThisBoolOption("cutLongTopicNameInTopicList");
    colorModoAndAdminTopic = settingToolClass::getThisBoolOption("colorModoAndAdminTopicInTopicList");
    showPinnedTagOnTopic = settingToolClass::getThisBoolOption("showPinnedTagOnTopicInTopicList");
    showHotTagOnTopic = settingToolClass::getThisBoolOption("showHotTagOnTopicInTopicList");
    showLockTagOnTopic = settingToolClass::getThisBoolOption("showLockTagOnTopicInTopicList");
    showResolvedTagOnTopic = settingToolClass::getThisBoolOption("showResolvedTagOnTopicInTopicList");
    showNormalTagOnTopic = settingToolClass::getThisBoolOption("showNormalTagOnTopicInTopicList");
    useIconInsteadOfTag = settingToolClass::getThisBoolOption("useIconInsteadOfTagInTopicList");
    topicNameMaxSize = settingToolClass::getThisIntOption("topicNameMaxSizeInTopicList").value;
    timerForGetList.setInterval(updateTopicListTimeSetting.value);

    if(updateTopicListTimeSetting.value < updateTopicListTimeSetting.minValue)
    {
        timerForGetList.setInterval(updateTopicListTimeSetting.minValue);
    }

    setLoadNeeded(settingToolClass::getThisBoolOption("showListOfTopic") == true && settingToolClass::getThisBoolOption("fastModeEnbled") == false);
    timeoutForReply.updateTimeoutTime();
}

void showListOfTopicClass::setLoadNeeded(bool newVal)
{
    if(newVal != loadNeeded)
    {
        loadNeeded = newVal;

        if(loadNeeded == false)
        {
            timerForGetList.stop();
        }
        else
        {
            startGetListOfTopic();
            timerForGetList.start();
        }
    }
}

void showListOfTopicClass::setNewTheme(QString newThemeName)
{
    baseModelInfo = styleToolClass::getModelInfo(newThemeName);
}

void showListOfTopicClass::startGetListOfTopic()
{
    bool itsNewManager = false;

    if(networkManager == nullptr)
    {
        itsNewManager = true;
        networkManager = new QNetworkAccessManager(this);
    }

    if(reply == nullptr)
    {
        if(itsNewManager == true)
        {
            setNewCookies(currentCookieList, websiteOfCookies);
        }

        if(forumLink.isEmpty() == false)
        {
            QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl(forumLink);
            reply = timeoutForReply.resetReply(networkManager->get(request));

            if(reply->isOpen() == true)
            {
                connect(reply, &QNetworkReply::finished, this, &showListOfTopicClass::analyzeReply);
            }
            else
            {
                analyzeReply();
                networkManager->deleteLater();
                networkManager = nullptr;
            }
        }
    }
}

void showListOfTopicClass::analyzeReply()
{
    QString source;
    QString locationHeader;

    timeoutForReply.resetReply();

    if(reply->isReadable() == true)
    {
        source = reply->readAll();
        locationHeader = reply->rawHeader("Location");
    }
    reply->deleteLater();

    if(source.isEmpty() == true)
    {
        if(locationHeader.startsWith("/forums/") == true)
        {
            forumLink = "http://" + websiteOfForum + locationHeader;
            reply = nullptr;
            startGetListOfTopic();
            return;
        }
    }
    else
    {
        QList<topicStruct> listOfTopic = parsingToolClass::getListOfTopic(source, websiteOfForum);
        QStandardItem* newItemToAppend;
        QFont tmpFont;

        modelForListView.clear();
        listOfLink.clear();

        newItemToAppend = new QStandardItem(parsingToolClass::getForumName(source));
        newItemToAppend->setEditable(false);
        tmpFont = newItemToAppend->font();
        tmpFont.setBold(true);
        newItemToAppend->setFont(tmpFont);
        modelForListView.appendRow(newItemToAppend);

        listOfLink.append("");

        for(const topicStruct& thisTopic : listOfTopic)
        {
            QString currentTopicName = thisTopic.name;

            if(cutLongTopicName == true)
            {
                if(currentTopicName.size() >= (topicNameMaxSize + 3))
                {
                    currentTopicName = currentTopicName.left(topicNameMaxSize) + "...";
                }
            }

            if(showNumberOfMessages == true)
            {
                currentTopicName.append(" (" + thisTopic.numberOfMessage + ")");
            }

            newItemToAppend = new QStandardItem();
            newItemToAppend->setEditable(false);

            if(thisTopic.topicType.startsWith("marque") == true)
            {
                if(showPinnedTagOnTopic == true)
                {
                    if(thisTopic.topicType == "marque-on")
                    {
                        if(useIconInsteadOfTag == true)
                        {
                            newItemToAppend->setIcon(QIcon(pinnedOnTagImage));
                        }
                        else
                        {
                            currentTopicName = "[EO] " + currentTopicName;
                        }
                    }
                    else if(thisTopic.topicType == "marque-off")
                    {
                        if(useIconInsteadOfTag == true)
                        {
                            newItemToAppend->setIcon(QIcon(pinnedOffTagImage));
                        }
                        else
                        {
                            currentTopicName = "[EF] " + currentTopicName;
                        }
                    }
                }
            }
            else if(thisTopic.topicType == "dossier2")
            {
                if(showHotTagOnTopic == true)
                {
                    if(useIconInsteadOfTag == true)
                    {
                        newItemToAppend->setIcon(QIcon(hotTagImage));
                    }
                    else
                    {
                        currentTopicName = "[M] " + currentTopicName;
                    }
                }
            }
            else if(thisTopic.topicType == "lock")
            {
                if(showLockTagOnTopic == true)
                {
                    if(useIconInsteadOfTag == true)
                    {
                        newItemToAppend->setIcon(QIcon(lockTagImage));
                    }
                    else
                    {
                        currentTopicName = "[F] " + currentTopicName;
                    }
                }
            }
            else if(thisTopic.topicType == "resolu")
            {
                if(showResolvedTagOnTopic == true)
                {
                    if(useIconInsteadOfTag == true)
                    {
                        newItemToAppend->setIcon(QIcon(resolvedTagImage));
                    }
                    else
                    {
                        currentTopicName = "[R] " + currentTopicName;
                    }
                }
            }
            else
            {
                if(showNormalTagOnTopic == true)
                {
                    if(useIconInsteadOfTag == true)
                    {
                        newItemToAppend->setIcon(QIcon(normalTagImage));
                    }
                    else
                    {
                        currentTopicName = "[N] " + currentTopicName;
                    }
                }
            }

            newItemToAppend->setText(currentTopicName);

            if(colorModoAndAdminTopic == true)
            {
                if(thisTopic.pseudoInfo.pseudoType == "modo")
                {
                    newItemToAppend->setForeground(QBrush(QColor(baseModelInfo.modoPseudoColor)));
                }
                else if(thisTopic.pseudoInfo.pseudoType == "admin" || thisTopic.pseudoInfo.pseudoType == "staff")
                {
                    newItemToAppend->setForeground(QBrush(QColor(baseModelInfo.adminPseudoColor)));
                }
            }

            modelForListView.appendRow(newItemToAppend);

            listOfLink.append(thisTopic.link);
        }
    }

    reply = nullptr;
}

void showListOfTopicClass::clickedOnLink(QModelIndex index)
{
    if(index.row() >= 1)
    {
        emit openThisTopic(listOfLink.at(index.row()));
    }
}

void showListOfTopicClass::createContextMenu(const QPoint& thisPoint)
{
    QList<QString> oldListOfTopicLink = listOfLink;
    QModelIndex indexSelected = listViewOfTopic.indexAt(thisPoint);
    if(indexSelected.row() >= 1)
    {
        QAction* actionSelected = nullptr;
        QMenu contextMenu;
        QAction* actionOpen = contextMenu.addAction("Ouvrir ce topic dans l'onglet actuel");
        QAction* actionOpenInNewTab = contextMenu.addAction("Ouvrir ce topic dans un nouvel onglet");
        QAction* actionOpenInNavigator = contextMenu.addAction("Ouvrir ce topic dans le navigateur");
        actionSelected = contextMenu.exec(listViewOfTopic.viewport()->mapToGlobal(thisPoint));

        if(actionSelected == actionOpen)
        {
            emit openThisTopic(oldListOfTopicLink.at(indexSelected.row()));
        }
        else if(actionSelected == actionOpenInNewTab)
        {
            emit openThisTopicInNewTab(oldListOfTopicLink.at(indexSelected.row()));
        }
        else if(actionSelected == actionOpenInNavigator)
        {
            QDesktopServices::openUrl(oldListOfTopicLink.at(indexSelected.row()));
        }
    }
}
