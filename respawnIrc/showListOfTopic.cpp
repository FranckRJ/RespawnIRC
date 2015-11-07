#include "showListOfTopic.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

showListOfTopicClass::showListOfTopicClass(QWidget *parent) : QWidget(parent)
{
    listViewOfTopic.setModel(&modelForListView);
    listViewOfTopic.setEditTriggers(QAbstractItemView::NoEditTriggers);
    listViewOfTopic.setContextMenuPolicy(Qt::CustomContextMenu);
    timerForGetList.setTimerType(Qt::CoarseTimer);
    timerForGetList.setInterval(5000);
    updateSettings();
    reply = 0;

    networkManager = new QNetworkAccessManager(this);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&listViewOfTopic);
    layout->setMargin(0);

    setLayout(layout);

    QObject::connect(&listViewOfTopic, &QListView::customContextMenuRequested, this, &showListOfTopicClass::createContextMenu);
    QObject::connect(&listViewOfTopic, &QListView::doubleClicked, this, &showListOfTopicClass::clickedOnLink);
    QObject::connect(&timerForGetList, &QTimer::timeout, this, &showListOfTopicClass::startGetListOfTopic);
}

void showListOfTopicClass::setForumLink(QString newForumLink)
{
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
    if(networkManager != 0)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
    }
}

void showListOfTopicClass::updateSettings()
{
    //timeoutReply.setInterval(settingToolClass::getThisIntOption("timeoutInSecond") * 1000);
}

void showListOfTopicClass::startGetListOfTopic()
{
    bool itsNewManager = false;

    if(networkManager == 0)
    {
        itsNewManager = true;
        networkManager = new QNetworkAccessManager(this);
    }

    if(reply == 0)
    {
        /*if(networkManager->networkAccessible() != QNetworkAccessManager::Accessible) //a changer
        {
            delete networkManager;
            networkManager = 0;
            return;
        }*/

        if(itsNewManager == true)
        {
            setNewCookies(currentCookieList);
        }

        if(forumLink.isEmpty() == false)
        {
            QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl(forumLink);
            reply = networkManager->get(request);

            QObject::connect(reply, &QNetworkReply::finished, this, &showListOfTopicClass::analyzeReply);
        }
    }
}

void showListOfTopicClass::analyzeReply()
{
    QString source;
    QString locationHeader;
    if(reply->isReadable() == true)
    {
        source = reply->readAll();
        locationHeader = reply->rawHeader("Location");
    }
    reply->deleteLater();

    if(source.size() == 0)
    {
        if(locationHeader.startsWith("/forums/") == true)
        {
            forumLink = "http://www.jeuxvideo.com" + locationHeader;
            reply = 0;
            startGetListOfTopic();
            return;
        }
    }
    else
    {
        QList<topicStruct> listOfTopic = parsingToolClass::getListOfTopic(source);
        QList<QString> listOfTopicName;

        listOfLink.clear();

        for(int i = 0; i < listOfTopic.size(); ++i)
        {
            listOfTopicName.append(listOfTopic.at(i).name);
            listOfLink.append(listOfTopic.at(i).link);
        }

        modelForListView.setStringList(listOfTopicName);
    }

    reply = 0;
}

void showListOfTopicClass::clickedOnLink(QModelIndex index)
{
    emit openThisTopic(listOfLink.at(index.row()));
}

void showListOfTopicClass::createContextMenu(const QPoint &thisPoint)
{
    QModelIndex indexSelected = listViewOfTopic.indexAt(thisPoint);
    if(indexSelected.row() >= 0)
    {
        QAction* actionSelected = 0;
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
