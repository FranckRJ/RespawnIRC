#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QVector>

#include "getTopicMessages.hpp"

getTopicMessagesClass::getTopicMessagesClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);

    timerForGetMessage = new QTimer(this);

    timerForGetMessage->setTimerType(Qt::CoarseTimer);
    timerForGetMessage->setInterval(7500);
    timerForGetMessage->stop();

    connect(timerForGetMessage, &QTimer::timeout, this, &getTopicMessagesClass::getMessages);
}

void getTopicMessagesClass::setNewTopic(QString newTopicLink, bool getFirstMessage)
{
    needToGetFirstMessage = getFirstMessage;
    listOfEdit.clear();
    linkHasChanged = true;
    firstTimeGetMessages = true;
    idOfLastMessage = 0;
    needToGetMessages = false;

    if(settingsForMessageParsing.isInOptimizedMode == true)
    {
        numberOfPagesToDownload = 2;
    }
    else
    {
        numberOfPagesToDownload = 1;
    }

    if(newTopicLink.isEmpty() == true)
    {
        topicLink.clear();
        timerForGetMessage->stop();
        return;
    }

    topicLink = parsingTool::getFirstPageOfTopic(newTopicLink);
    websiteOfTopic = parsingTool::getWebsite(topicLink);

    if(retrievesMessage == false)
    {
        emit newMessageStatus("Nouveau topic.");
    }
    else
    {
        for(QNetworkReply*& thisReply : listOfReplys)
        {
            if(thisReply != nullptr)
            {
                if(thisReply->isRunning())
                {
                    thisReply->abort();
                }
            }
        }
    }
    emit newNumberOfConnectedAndMP("", -1, true);
    startGetMessage();
}

void getTopicMessagesClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newWebsiteOfCookies, QString newPseudoOfUser, bool updateMessages)
{
    currentCookieList = newCookies;
    websiteOfCookies = newWebsiteOfCookies;
    pseudoOfUser = newPseudoOfUser;

    if(networkManager != nullptr)
    {
        if(retrievesMessage == false)
        {
            networkManager->clearAccessCache();
            networkManager->setCookieJar(new QNetworkCookieJar(this));
            networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://" + websiteOfCookies));

            if(updateMessages == true)
            {
                startGetMessage();
            }
        }
        else
        {
            needToSetCookies = true;
        }
    }
}

void getTopicMessagesClass::settingsChanged(settingsForMessageParsingStruct newSettings)
{
    settingsForMessageParsing = newSettings;

    if(settingsForMessageParsing.isInOptimizedMode == true)
    {
        numberOfPagesToDownload = 2;
    }
    else
    {
        numberOfPagesToDownload = 1;
    }

    timerForGetMessage->setInterval(settingsForMessageParsing.timerTime);

    for(autoTimeoutReplyClass*& autoTimeout : listOfTimeoutForReplys)
    {
        if(autoTimeout != nullptr)
        {
            autoTimeout->updateTimeoutTime(settingsForMessageParsing.timeoutTime);
        }
    }
}

void getTopicMessagesClass::startGetMessage()
{
    needToGetMessages = false;
    if(topicLink.isEmpty() == false)
    {
        if(retrievesMessage == false)
        {
            timerForGetMessage->start();
            getMessages();
        }
        else
        {
            needToGetMessages = true;
        }
    }
}

void getTopicMessagesClass::analyzeMessages()
{
    QString newTopicLink;
    ajaxInfoStruct ajaxInfo;
    QList<messageStruct> listOfNewMessages;
    QList<QPair<QString, QString>> listOfInput;
    QStringList tmpListOfStickersUsed;
    QStringList tmpListOfNoelshackImagesUsed;
    QVector<QString> listOfPageSource;
    QVector<QString> listOfPageUrl;
    QString locationHeader = ".";
    int firstValidePageNumber = -1;

    settingsForMessageParsing.infoForMessageParsing.listOfStickersUsed = (settingsForMessageParsing.downloadMissingStickers == true ? &tmpListOfStickersUsed : nullptr);
    settingsForMessageParsing.infoForMessageParsing.listOfNoelshackImageUsed = (settingsForMessageParsing.downloadNoelshackImages == true ? &tmpListOfNoelshackImagesUsed : nullptr);

    for(autoTimeoutReplyClass*& autoTimeout : listOfTimeoutForReplys)
    {
        if(autoTimeout != nullptr)
        {
            autoTimeout->resetReply();
            autoTimeout->deleteLater();
            autoTimeout = nullptr;
        }
    }

    listOfPageSource.resize(listOfReplys.size());
    listOfPageUrl.resize(listOfReplys.size());

    for(int i = 0; i < listOfReplys.size(); ++i)
    {
        if(listOfReplys[i] != nullptr)
        {
            if(listOfReplys[i]->isReadable() == true)
            {
                listOfPageSource[i] = listOfReplys[i]->readAll();

                if(locationHeader == ".")
                {
                    locationHeader = listOfReplys[i]->rawHeader("Location");
                }

                if(firstValidePageNumber == -1 && listOfPageSource[i].isEmpty() == false)
                {
                    firstValidePageNumber = i;
                }
            }

            listOfPageUrl[i] = listOfReplys[i]->request().url().toDisplayString();

            listOfReplys[i]->deleteLater();
            listOfReplys[i] = nullptr;
        }
    }

    emit newMessageStatus("Récupération des messages terminée !");

    if(linkHasChanged == true)
    {
        retrievesMessage = false;
        startGetMessage();
        return;
    }

    if(firstValidePageNumber == -1)
    {
        if(locationHeader.startsWith("/forums/") == true)
        {
            QString locationHeaderTopicLink = "http://" + websiteOfTopic + locationHeader;
            if(parsingTool::checkIfTopicAreSame(topicLink, locationHeaderTopicLink) == true)
            {
                topicLink = locationHeaderTopicLink;
                emit newLinkForTopic(topicLink);
                retrievesMessage = false;
                startGetMessage();
                return;
            }
        }

        emit newMessagesAreAvailable(QList<messageStruct>(), listOfInput, ajaxInfo, topicLink, true);
        retrievesMessage = false;
        return;
    }

    if(currentCookieList.isEmpty() == false)
    {
        emit newNumberOfConnectedAndMP(parsingTool::getNumberOfConnected(listOfPageSource[firstValidePageNumber]),
                                       parsingTool::getNumberOfMp(listOfPageSource[firstValidePageNumber]), false);
    }
    else
    {
        emit newNumberOfConnectedAndMP(parsingTool::getNumberOfConnected(listOfPageSource[firstValidePageNumber]), -1, false);
    }

    if(firstTimeGetMessages == false)
    {
        newTopicLink = parsingTool::getNextPageOfTopic(listOfPageSource[firstValidePageNumber], websiteOfTopic);
    }
    else
    {
        newTopicLink = parsingTool::getLastPageOfTopic(listOfPageSource[firstValidePageNumber], websiteOfTopic);
    }

    if(newTopicLink.isEmpty() == true && topicLink != listOfPageUrl[firstValidePageNumber])
    {
        newTopicLink = listOfPageUrl[firstValidePageNumber];
    }

    if(firstTimeGetMessages == true)
    {
        QString topicName = parsingTool::getNameOfTopic(listOfPageSource[firstValidePageNumber]);

        if(topicName.isEmpty() == false)
        {
            emit newNameForTopic(topicName);
        }

        if(needToGetFirstMessage == true)
        {
            QList<messageStruct> tmpList = parsingTool::getListOfEntireMessagesWithoutMessagePars(listOfPageSource[0]);

            if(tmpList.isEmpty() == false)
            {
                messageStruct tmpMsg = tmpList.first();

                tmpMsg.isFirstMessage = true;
                tmpMsg.isAnEdit = false;
                tmpMsg.message = parsingTool::parsingMessages(tmpMsg.message, settingsForMessageParsing.infoForMessageParsing);
                tmpMsg.signature = parsingTool::parsingMessages(tmpMsg.signature, settingsForMessageParsing.infoForMessageParsing, false);

                listOfNewMessages.push_front(tmpMsg);
            }
        }
    }

    if(firstTimeGetMessages == false || newTopicLink.isEmpty() == true)
    {
        int numberOfMessagesInLastPage = 0;
        QList<messageStruct> listOfEntireMessages;

        for(int i = listOfPageSource.size() - 1; i >= 0; --i)
        {
            QList<messageStruct> listForThisPage = parsingTool::getListOfEntireMessagesWithoutMessagePars(listOfPageSource[i]);
            numberOfMessagesInLastPage = listForThisPage.size();
            listOfEntireMessages.append(listForThisPage);
        }

        if(settingsForMessageParsing.isInOptimizedMode == true)
        {
            numberOfPagesToDownload = (numberOfMessagesInLastPage >= settingsForMessageParsing.numberOfMessagesForOptimizationStart ? 1 : 2);
        }

        if(listOfEntireMessages.isEmpty() == true)
        {
            emit newMessagesAreAvailable(QList<messageStruct>(), listOfInput, ajaxInfo, topicLink, true);
            retrievesMessage = false;
            return;
        }

        for(messageStruct& currentMessage : listOfEntireMessages)
        {
            QMap<long, QString>::const_iterator listOfEditIterator = listOfEdit.find(currentMessage.idOfMessage);
            QString valueOfEditIte = currentMessage.lastTimeEdit;

            if(listOfEditIterator != listOfEdit.end())
            {
                valueOfEditIte = listOfEditIterator.value();
            }

            if(currentMessage.idOfMessage > idOfLastMessage || (valueOfEditIte != currentMessage.lastTimeEdit))
            {
                if(valueOfEditIte != currentMessage.lastTimeEdit)
                {
                    currentMessage.isAnEdit = true;
                }
                else
                {
                    currentMessage.isAnEdit = false;
                    idOfLastMessage = currentMessage.idOfMessage;
                }

                currentMessage.message = parsingTool::parsingMessages(currentMessage.message, settingsForMessageParsing.infoForMessageParsing);
                currentMessage.signature = parsingTool::parsingMessages(currentMessage.signature, settingsForMessageParsing.infoForMessageParsing, false);
                listOfNewMessages.push_back(currentMessage);

                listOfEdit[currentMessage.idOfMessage] = currentMessage.lastTimeEdit;
            }
        }

        while(listOfEdit.size() > 40)
        {
            listOfEdit.erase(listOfEdit.begin());
        }
    }

    if(currentCookieList.isEmpty() == false && needToSetCookies == false)
    {
        ajaxInfo = parsingTool::getAjaxInfo(listOfPageSource[firstValidePageNumber]);
        parsingTool::getListOfHiddenInputFromThisForm(listOfPageSource[firstValidePageNumber], "form-post-topic", listOfInput);

        if(listOfInput.isEmpty() == true)
        {
            if(websiteOfTopic == "www.forumjv.com")
            {
                listOfInput.append(QPair<QString, QString>("forumjv", "true"));
            }
            else if(parsingTool::getTopicLocked(listOfPageSource[firstValidePageNumber]) == true)
            {
                listOfInput.append(QPair<QString, QString>("locked", "true"));
            }
        }
    }
    firstTimeGetMessages = false;
    retrievesMessage = false;

    emit newMessagesAreAvailable(listOfNewMessages, listOfInput, ajaxInfo, topicLink, false);
    emit theseStickersAreUsed(tmpListOfStickersUsed);
    emit theseNoelshackImagesAreUsed(tmpListOfNoelshackImagesUsed);

    if(newTopicLink.isEmpty() == false)
    {
        topicLink = newTopicLink;
        startGetMessage();
    }
    else if(needToGetMessages == true)
    {
        startGetMessage();
    }
}

void getTopicMessagesClass::getMessages()
{
    bool itsNewManager = false;

    if(networkManager == nullptr)
    {
        itsNewManager = true;
        networkManager = new QNetworkAccessManager(this);
    }

    if(retrievesMessage == false)
    {
        QString currentPageToLoad;
        bool errorWhenTryToGetMessages = false;

        for(QNetworkReply*& thisReply : listOfReplys)
        {
            if(thisReply != nullptr)
            {
                thisReply->deleteLater();
                thisReply = nullptr;
            }
        }

        for(autoTimeoutReplyClass*& autoTimeout : listOfTimeoutForReplys)
        {
            if(autoTimeout != nullptr)
            {
                autoTimeout->deleteLater();
                autoTimeout = nullptr;
            }
        }

        if(itsNewManager == true || needToSetCookies == true)
        {
            setNewCookies(currentCookieList, websiteOfCookies, pseudoOfUser, false);
            needToSetCookies = false;
        }

        retrievesMessage = true;
        currentPageToLoad = topicLink;
        linkHasChanged = false;
        emit newMessageStatus("Récupération des messages en cours...");
        listOfReplys.fill(nullptr, numberOfPagesToDownload);
        listOfTimeoutForReplys.fill(nullptr, listOfReplys.size());

        for(int i = 0; i < listOfReplys.size(); ++i)
        {
            if(currentPageToLoad.isEmpty() == false)
            {
                QNetworkRequest requestForThisPage = parsingTool::buildRequestWithThisUrl(currentPageToLoad);

                listOfTimeoutForReplys[i] = new autoTimeoutReplyClass(settingsForMessageParsing.timeoutTime, this);
                listOfReplys[i] = listOfTimeoutForReplys[i]->resetReply(networkManager->get(requestForThisPage));

                if(listOfReplys[i]->isOpen() == true)
                {
                    connect(listOfReplys[i], &QNetworkReply::finished, this, &getTopicMessagesClass::loadForThisPageFinish);
                }
                else
                {
                    errorWhenTryToGetMessages = true;
                    break;
                }

                currentPageToLoad = parsingTool::getBeforeLastPageOfTopic(currentPageToLoad);
            }
            else
            {
                break;
            }
        }

        if(errorWhenTryToGetMessages == true)
        {
            analyzeMessages();
            networkManager->deleteLater();
            networkManager = nullptr;
        }
    }
}


void getTopicMessagesClass::loadForThisPageFinish()
{
    bool allIsFinished = true;

    for(QNetworkReply*& thisReply : listOfReplys)
    {
        if(thisReply != nullptr)
        {
            if(thisReply->isFinished() == false)
            {
                allIsFinished = false;
            }
        }
    }

    if(allIsFinished == true)
    {
        analyzeMessages();
    }
}
