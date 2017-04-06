#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QPixmap>
#include <QScopedPointer>

#include "showForum.hpp"
#include "settingTool.hpp"

namespace
{
    QScopedPointer<QPixmap> pinnedOnTagImage;
    QScopedPointer<QPixmap> pinnedOffTagImage;
    QScopedPointer<QPixmap> hotTagImage;
    QScopedPointer<QPixmap> lockTagImage;
    QScopedPointer<QPixmap> resolvedTagImage;
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
    if(normalTagImage.isNull() == true)
    {
        normalTagImage.reset(new QPixmap);
        normalTagImage->load(QCoreApplication::applicationDirPath() + "/resources/topic-dossier1.png");
    }

    listViewOfTopic.setModel(&modelForListView);
    listViewOfTopic.setEditTriggers(QAbstractItemView::NoEditTriggers);
    listViewOfTopic.setContextMenuPolicy(Qt::CustomContextMenu);
    timerForGetList.setTimerType(Qt::CoarseTimer);
    timerForGetList.setInterval(15000);
    updateSettings();
    setNewTheme(currentThemeName);

    networkManager = new QNetworkAccessManager(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&listViewOfTopic);
    layout->setMargin(0);

    setLayout(layout);

    connect(&listViewOfTopic, &QListView::customContextMenuRequested, this, &showForumClass::createContextMenu);
    connect(&listViewOfTopic, &QListView::doubleClicked, this, &showForumClass::clickedOnLink);
    connect(&timerForGetList, &QTimer::timeout, this, &showForumClass::startGetListOfTopic);
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

void showForumClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newWebsiteOfCookies)
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

void showForumClass::setNewTheme(QString newThemeName)
{
    baseModelInfo = styleToolClass::getModelInfo(newThemeName);
}

void showForumClass::updateSettings()
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

    if(forThisTopic.topicType.startsWith("marque") == true)
    {
        if(showPinnedTagOnTopic == true)
        {
            if(forThisTopic.topicType == "marque-on")
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
            else if(forThisTopic.topicType == "marque-off")
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
    else if(forThisTopic.topicType == "dossier2")
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
    else if(forThisTopic.topicType == "lock")
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
    else if(forThisTopic.topicType == "resolu")
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
            timerForGetList.stop();
        }
        else
        {
            startGetListOfTopic();
            timerForGetList.start();
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
            setNewCookies(currentCookieList, websiteOfCookies);
        }

        if(forumLink.isEmpty() == false)
        {
            QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl(forumLink);
            reply = timeoutForReply.resetReply(networkManager->get(request));

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
        tmpFont = newItemToAppend->font();
        tmpFont.setBold(true);
        newItemToAppend->setFont(tmpFont);
        modelForListView.appendRow(newItemToAppend);

        listOfLink.append("");

        for(const topicStruct& thisTopic : listOfTopic)
        {
            modelForListView.appendRow(createItemDependingOnTopic(thisTopic));
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
