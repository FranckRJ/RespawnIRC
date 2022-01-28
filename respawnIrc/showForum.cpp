#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QCoreApplication>
#include <QPixmap>
#include <QScopedPointer>

#include "showForum.hpp"
#include "utilityTool.hpp"
#include "settingTool.hpp"

namespace
{
    //pourquoi j'utilise des qscopedpointer au lieu des std::unique_ptr ? Je sais plus, je crois
    //que c'est à cause d'un problème d'ordre d'appel de constructeur/destructeur ou j'sais plus quoi
    //EDIT: Nan ça c'est la raison pour laquelle j'utilise des pointeurs, j'utilise les QScopedPointer
    //pour garder une cohérence en utilisant uniquement les classes Qt si possible etc
    QScopedPointer<QPixmap> pinnedOnTagImage;
    QScopedPointer<QPixmap> pinnedOffTagImage;
    QScopedPointer<QPixmap> hotTagImage;
    QScopedPointer<QPixmap> lockTagImage;
    QScopedPointer<QPixmap> resolvedTagImage;
    QScopedPointer<QPixmap> ghostTagImage;
    QScopedPointer<QPixmap> normalTagImage;
}

showForumClass::showForumClass(QString currentThemeName, QWidget* parent) : QWidget(parent)
{
    if(pinnedOnTagImage.isNull() == true)
    {
        pinnedOnTagImage.reset(new QPixmap);
        pinnedOnTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-marque-on.png");
    }
    if(pinnedOffTagImage.isNull() == true)
    {
        pinnedOffTagImage.reset(new QPixmap);
        pinnedOffTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-marque-off.png");
    }
    if(hotTagImage.isNull() == true)
    {
        hotTagImage.reset(new QPixmap);
        hotTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-dossier2.png");
    }
    if(lockTagImage.isNull() == true)
    {
        lockTagImage.reset(new QPixmap);
        lockTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-lock.png");
    }
    if(resolvedTagImage.isNull() == true)
    {
        resolvedTagImage.reset(new QPixmap);
        resolvedTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-resolu.png");
    }
    if(ghostTagImage.isNull() == true)
    {
        ghostTagImage.reset(new QPixmap);
        ghostTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-ghost.png");
    }
    if(normalTagImage.isNull() == true)
    {
        normalTagImage.reset(new QPixmap);
        normalTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-dossier1.png");
    }

    timerForGetList = new QTimer(this);
    timeoutForReply = new autoTimeoutReplyClass(0, this);
    listViewOfTopic = new QListView(this);
    modelForListView = new QStandardItemModel(listViewOfTopic);

    listViewOfTopic->setObjectName("listOfTopics");
    listViewOfTopic->setModel(modelForListView);
    listViewOfTopic->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listViewOfTopic->setContextMenuPolicy(Qt::CustomContextMenu);
    timerForGetList->setTimerType(Qt::CoarseTimer);
    timerForGetList->setInterval(15'000);
    updateSettings();
    setNewTheme(currentThemeName);

    networkManager = new QNetworkAccessManager(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(listViewOfTopic);
    layout->setMargin(0);

    setLayout(layout);

    connect(listViewOfTopic, &QListView::customContextMenuRequested, this, &showForumClass::createContextMenu);
    connect(listViewOfTopic, &QListView::doubleClicked, this, &showForumClass::clickedOnLink);
    connect(timerForGetList, &QTimer::timeout, this, &showForumClass::startGetListOfTopic);
}

bool showForumClass::getLoadNeeded() const
{
    return loadNeeded;
}

void showForumClass::setForumLink(QString newForumLink)
{
    if(reply != nullptr)
    {
        if(reply->isRunning())
        {
            reply->abort();
        }
    }

    forumLink = newForumLink;
    websiteOfForum = parsingTool::getWebsite(forumLink);

    modelForListView->clear();
    if(newForumLink.isEmpty() == false)
    {
        if(loadNeeded == true)
        {
            startGetListOfTopic();
            timerForGetList->start();
        }
    }
    else
    {
        timerForGetList->stop();
    }
}

void showForumClass::setNewCookie(QNetworkCookie newConnectCookie, QString newWebsiteOfCookie)
{
    currentConnectCookie = newConnectCookie;
    websiteOfCookie = newWebsiteOfCookie;
    if(networkManager != nullptr)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(utilityTool::cookieToCookieList(newConnectCookie), QUrl("https://" + websiteOfCookie));
    }
}

void showForumClass::setNewTheme(QString newThemeName)
{
    baseModelInfo = styleTool::getModelInfo(newThemeName);
}

void showForumClass::updateSettings()
{
    intSettingStruct updateTopicListTimeSetting = settingTool::getThisIntOption("updateTopicListTime");

    showNumberOfMessages = settingTool::getThisBoolOption("showNumberOfMessagesInTopicList");
    cutLongTopicName = settingTool::getThisBoolOption("cutLongTopicNameInTopicList");
    colorModoAndAdminTopic = settingTool::getThisBoolOption("colorModoAndAdminTopicInTopicList");
    showPinnedTagOnTopic = settingTool::getThisBoolOption("showPinnedTagOnTopicInTopicList");
    showHotTagOnTopic = settingTool::getThisBoolOption("showHotTagOnTopicInTopicList");
    showLockTagOnTopic = settingTool::getThisBoolOption("showLockTagOnTopicInTopicList");
    showResolvedTagOnTopic = settingTool::getThisBoolOption("showResolvedTagOnTopicInTopicList");
    showGhostTagOnTopic = settingTool::getThisBoolOption("showGhostTagOnTopicInTopicList");
    showNormalTagOnTopic = settingTool::getThisBoolOption("showNormalTagOnTopicInTopicList");
    useIconInsteadOfTag = settingTool::getThisBoolOption("useIconInsteadOfTagInTopicList");
    topicNameMaxSize = settingTool::getThisIntOption("topicNameMaxSizeInTopicList").value;
    useInternalNavigatorForLinks = settingTool::getThisBoolOption("useInternalNavigatorForLinks");
    timerForGetList->setInterval(updateTopicListTimeSetting.value);

    if(updateTopicListTimeSetting.value < updateTopicListTimeSetting.minValue)
    {
        timerForGetList->setInterval(updateTopicListTimeSetting.minValue);
    }

    setLoadNeeded(settingTool::getThisBoolOption("showListOfTopic") == true && settingTool::getThisBoolOption("fastModeEnbled") == false);
    timeoutForReply->updateTimeoutTime();
}

QStandardItem* showForumClass::createItemDependingOnTopic(const topicStruct& forThisTopic)
{
    QStandardItem* newItem = new QStandardItem();
    QString currentTopicName = forThisTopic.name;

    if(cutLongTopicName == true)
    {
        if(currentTopicName.size() >= (topicNameMaxSize + 3))
        {
            currentTopicName = currentTopicName.left(topicNameMaxSize) + "...";
        }
    }

    if(showNumberOfMessages == true)
    {
        currentTopicName.append(" (" + forThisTopic.numberOfMessage + ")");
    }

    if(forThisTopic.topicType.startsWith("topic-pin") == true)
    {
        if(showPinnedTagOnTopic == true)
        {
            if(forThisTopic.topicType == "topic-pin-on")
            {
                if(useIconInsteadOfTag == true)
                {
                    newItem->setIcon(QIcon(*pinnedOnTagImage));
                }
                else
                {
                    currentTopicName = "[EO] " + currentTopicName;
                }
            }
            else if(forThisTopic.topicType == "topic-pin-off")
            {
                if(useIconInsteadOfTag == true)
                {
                    newItem->setIcon(QIcon(*pinnedOffTagImage));
                }
                else
                {
                    currentTopicName = "[EF] " + currentTopicName;
                }
            }
        }
    }
    else if(forThisTopic.topicType == "topic-folder2")
    {
        if(showHotTagOnTopic == true)
        {
            if(useIconInsteadOfTag == true)
            {
                newItem->setIcon(QIcon(*hotTagImage));
            }
            else
            {
                currentTopicName = "[M] " + currentTopicName;
            }
        }
    }
    else if(forThisTopic.topicType == "topic-lock")
    {
        if(showLockTagOnTopic == true)
        {
            if(useIconInsteadOfTag == true)
            {
                newItem->setIcon(QIcon(*lockTagImage));
            }
            else
            {
                currentTopicName = "[F] " + currentTopicName;
            }
        }
    }
    else if(forThisTopic.topicType == "topic-resolved")
    {
        if(showResolvedTagOnTopic == true)
        {
            if(useIconInsteadOfTag == true)
            {
                newItem->setIcon(QIcon(*resolvedTagImage));
            }
            else
            {
                currentTopicName = "[R] " + currentTopicName;
            }
        }
    }
    else if(forThisTopic.topicType == "topic-removed")
    {
        if(showGhostTagOnTopic == true)
        {
            if(useIconInsteadOfTag == true)
            {
                newItem->setIcon(QIcon(*ghostTagImage));
            }
            else
            {
                currentTopicName = "[S] " + currentTopicName;
            }
        }
    }
    else
    {
        if(showNormalTagOnTopic == true)
        {
            if(useIconInsteadOfTag == true)
            {
                newItem->setIcon(QIcon(*normalTagImage));
            }
            else
            {
                currentTopicName = "[N] " + currentTopicName;
            }
        }
    }

    newItem->setText(currentTopicName);

    if(colorModoAndAdminTopic == true)
    {
        if(forThisTopic.pseudoInfo.pseudoType == "modo")
        {
            newItem->setForeground(QBrush(QColor(baseModelInfo.modoPseudoColor)));
        }
        else if(forThisTopic.pseudoInfo.pseudoType == "admin" || forThisTopic.pseudoInfo.pseudoType == "staff")
        {
            newItem->setForeground(QBrush(QColor(baseModelInfo.adminPseudoColor)));
        }
    }

    return newItem;
}

void showForumClass::setLoadNeeded(bool newVal)
{
    if(newVal != loadNeeded)
    {
        loadNeeded = newVal;

        if(loadNeeded == false)
        {
            timerForGetList->stop();
        }
        else
        {
            startGetListOfTopic();
            timerForGetList->start();
        }
    }
}

void showForumClass::startGetListOfTopic()
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
            setNewCookie(currentConnectCookie, websiteOfCookie);
        }

        if(forumLink.isEmpty() == false)
        {
            QNetworkRequest request = parsingTool::buildRequestWithThisUrl(forumLink);
            reply = timeoutForReply->resetReply(networkManager->get(request));

            if(reply->isOpen() == true)
            {
                connect(reply, &QNetworkReply::finished, this, &showForumClass::analyzeReply);
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

void showForumClass::analyzeReply()
{
    QString source;
    QString locationHeader;

    timeoutForReply->resetReply();

    if(reply->isReadable() == true)
    {
        source = reply->readAll();
        locationHeader = reply->rawHeader("Location");
    }
    reply->deleteLater();

    if(source.isEmpty() == true || source.contains("<meta http-equiv=\"refresh\"") == true)
    {
        if(locationHeader.startsWith("/forums/") == true)
        {
            forumLink = "https://" + websiteOfForum + locationHeader;
            reply = nullptr;
            startGetListOfTopic();
            return;
        }
    }
    else
    {
        QList<topicStruct> listOfTopic = parsingTool::getListOfTopic(source, websiteOfForum);
        QStandardItem* newItemToAppend;
        QFont tmpFont;

        modelForListView->clear();
        listOfLink.clear();

        newItemToAppend = new QStandardItem(parsingTool::getForumName(source));
        tmpFont = newItemToAppend->font();
        tmpFont.setBold(true);
        newItemToAppend->setFont(tmpFont);
        modelForListView->appendRow(newItemToAppend);

        listOfLink.append("");

        for(const topicStruct& thisTopic : listOfTopic)
        {
            modelForListView->appendRow(createItemDependingOnTopic(thisTopic));
            listOfLink.append(thisTopic.link);
        }
    }

    reply = nullptr;
}

void showForumClass::clickedOnLink(QModelIndex index)
{
    if(index.row() >= 1)
    {
        emit openThisTopic(listOfLink.at(index.row()));
    }
}

void showForumClass::createContextMenu(const QPoint& thisPoint)
{
    QList<QString> oldListOfTopicLink = listOfLink;
    QModelIndex indexSelected = listViewOfTopic->indexAt(thisPoint);
    if(indexSelected.row() >= 1)
    {
        QAction* actionSelected = nullptr;
        QMenu* contextMenu = new QMenu(this);
        QAction* actionOpen = contextMenu->addAction("Ouvrir ce topic dans l'onglet actuel");
        QAction* actionOpenInNewTab = contextMenu->addAction("Ouvrir ce topic dans un nouvel onglet");
        QAction* actionOpenInNavigator = contextMenu->addAction("Ouvrir ce topic dans le navigateur");
        actionSelected = contextMenu->exec(listViewOfTopic->viewport()->mapToGlobal(thisPoint));
        contextMenu->deleteLater();

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
            utilityTool::openLinkInBrowser(this, useInternalNavigatorForLinks, oldListOfTopicLink.at(indexSelected.row()), currentConnectCookie);
        }
    }
}
