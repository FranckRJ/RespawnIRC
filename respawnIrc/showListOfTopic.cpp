#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkCookieJar>
#include <QVBoxLayout>
#include <QMenu>
#include <QDesktopServices>

#include "showListOfTopic.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

showListOfTopicClass::showListOfTopicClass(QWidget* parent) : QWidget(parent)
{
    listViewOfTopic.setModel(&modelForListView);
    listViewOfTopic.setEditTriggers(QAbstractItemView::NoEditTriggers);
    listViewOfTopic.setContextMenuPolicy(Qt::CustomContextMenu);
    timerForGetList.setTimerType(Qt::CoarseTimer);
    timerForGetList.setInterval(5000);
    updateSettings();

    networkManager = new QNetworkAccessManager(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&listViewOfTopic);
    layout->setMargin(0);

    setLayout(layout);

    connect(&listViewOfTopic, &QListView::customContextMenuRequested, this, &showListOfTopicClass::createContextMenu);
    connect(&listViewOfTopic, &QListView::doubleClicked, this, &showListOfTopicClass::clickedOnLink);
    connect(&timerForGetList, &QTimer::timeout, this, &showListOfTopicClass::startGetListOfTopic);
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
    modelForListView.setStringList(QList<QString>());
    if(newForumLink.isEmpty() == false)
    {
        startGetListOfTopic();
        timerForGetList.start();
    }
    else
    {
        timerForGetList.stop();
    }
}

void showListOfTopicClass::setNewCookies(QList<QNetworkCookie> newCookies)
{
    currentCookieList = newCookies;
    if(networkManager != nullptr)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
    }
}

void showListOfTopicClass::updateSettings()
{
    showNumberOfMessages = settingToolClass::getThisBoolOption("showNumberOfMessagesInTopicList");
    cutLongTopicName = settingToolClass::getThisBoolOption("cutLongTopicNameInTopicList");
    timeoutForReply.updateTimeoutTime();
}

void showListOfTopicClass::setLoadNeeded(bool newVal)
{
    if(newVal == false)
    {
        timerForGetList.stop();
    }
    else
    {
        timerForGetList.start();
    }
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
            setNewCookies(currentCookieList);
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
            forumLink = "http://www.jeuxvideo.com" + locationHeader;
            reply = nullptr;
            startGetListOfTopic();
            return;
        }
    }
    else
    {
        QList<topicStruct> listOfTopic = parsingToolClass::getListOfTopic(source);
        QList<QString> listOfTopicName;

        listOfLink.clear();
        listOfTopicName.append(parsingToolClass::getForumName(source));
        listOfLink.append("");

        for(const topicStruct& thisTopic : listOfTopic)
        {
            listOfTopicName.append(thisTopic.name);

            if(cutLongTopicName == true)
            {
                if(listOfTopicName.back().size() >= (topicNameMaxSize + 3))
                {
                    listOfTopicName.back() = listOfTopicName.back().left(topicNameMaxSize) + "...";
                }
            }

            if(showNumberOfMessages == true)
            {
                listOfTopicName.back().append(" (" + thisTopic.numberOfMessage + ")");
            }

            listOfLink.append(thisTopic.link);
        }

        modelForListView.setStringList(listOfTopicName);
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

void showListOfTopicClass::createContextMenu(const QPoint &thisPoint)
{
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
            emit openThisTopic(listOfLink.at(indexSelected.row()));
        }
        else if(actionSelected == actionOpenInNewTab)
        {
            emit openThisTopicInNewTab(listOfLink.at(indexSelected.row()));
        }
        else if(actionSelected == actionOpenInNavigator)
        {
            QDesktopServices::openUrl(listOfLink.at(indexSelected.row()));
        }
    }
}
