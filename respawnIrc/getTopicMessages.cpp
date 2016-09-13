#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QVector>

#include "getTopicMessages.hpp"

getTopicMessagesClass::getTopicMessagesClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);

    timerForGetMessage = new QTimer(this);

    timerForGetMessage->setTimerType(Qt::CoarseTimer);
    timerForGetMessage->setInterval(5000);
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

    if(newTopicLink.isEmpty() == true)
    {
        topicLink.clear();
        timerForGetMessage->stop();
        return;
    }

    topicLink = parsingToolClass::getFirstPageOfTopic(newTopicLink);

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
    emit newNumberOfConnectedAndMP("", "", true);
    startGetMessage();
}

void getTopicMessagesClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool updateMessages)
{
    currentCookieList = newCookies;
    pseudoOfUser = newPseudoOfUser;

    if(networkManager != nullptr)
    {
        if(retrievesMessage == false)
        {
            networkManager->clearAccessCache();
            networkManager->setCookieJar(new QNetworkCookieJar(this));
            networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));

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

    numberOfPagesToDownload = (settingsForMessageParsing.loadTwoLastPage == true ? 2 : 1);
    if(settingsForMessageParsing.numberOfPagesToLoad != -1)
    {
        numberOfPagesToDownload = settingsForMessageParsing.numberOfPagesToLoad;
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
            setNewCookies(currentCookieList, pseudoOfUser, false);
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
                QNetworkRequest requestForThisPage = parsingToolClass::buildRequestWithThisUrl(currentPageToLoad);

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

                currentPageToLoad = parsingToolClass::getBeforeLastPageOfTopic(currentPageToLoad);
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

void getTopicMessagesClass::analyzeMessages()
{
    QString newTopicLink;
    ajaxInfoStruct ajaxInfo;
    QList<messageStruct> listOfNewMessages;
    QList<QPair<QString, QString> > listOfInput;
    QStringList listOfStickersUsed;
    QStringList listOfNoelshackImagesUsed;
    QVector<QString> listOfPageSource;
    QVector<QString> listOfPageUrl;
    QString locationHeader = ".";
    int firstValidePageNumber = -1;

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
            topicLink = "http://www.jeuxvideo.com" + locationHeader;
            emit newLinkForTopic(topicLink);
            retrievesMessage = false;
            startGetMessage();
            return;
        }
        else
        {
            emit newMessagesAreAvailable(QList<messageStruct>(), listOfInput, ajaxInfo, topicLink, true);
            retrievesMessage = false;
            return;
        }
    }

    if(currentCookieList.isEmpty() == false)
    {
        emit newNumberOfConnectedAndMP(parsingToolClass::getNumberOfConnected(listOfPageSource[firstValidePageNumber]),
                                       parsingToolClass::getNumberOfMp(listOfPageSource[firstValidePageNumber]), false);
    }
    else
    {
        emit newNumberOfConnectedAndMP(parsingToolClass::getNumberOfConnected(listOfPageSource[firstValidePageNumber]), "", false);
    }

    newTopicLink = parsingToolClass::getLastPageOfTopic(listOfPageSource[firstValidePageNumber]);

    if(newTopicLink.isEmpty() == true && topicLink != listOfPageUrl[firstValidePageNumber])
    {
        newTopicLink = listOfPageUrl[firstValidePageNumber];
    }

    if(firstTimeGetMessages == true)
    {
        QString topicName = parsingToolClass::getNameOfTopic(listOfPageSource[firstValidePageNumber]);

        if(topicName.isEmpty() == false)
        {
            emit newNameForTopic(topicName);
        }

        if(needToGetFirstMessage == true)
        {
            QList<messageStruct> tmpList = parsingToolClass::getListOfEntireMessagesWithoutMessagePars(listOfPageSource[0]);

            if(tmpList.isEmpty() == false)
            {
                messageStruct tmpMsg = tmpList.first();

                tmpMsg.isFirstMessage = true;
                tmpMsg.isAnEdit = false;
                tmpMsg.message = parsingToolClass::parsingMessages(tmpMsg.message, settingsForMessageParsing.infoForMessageParsing,
                                                                   (settingsForMessageParsing.downloadMissingStickers == true ? &listOfStickersUsed : nullptr),
                                                                   (settingsForMessageParsing.downloadNoelshackImages == true ? &listOfNoelshackImagesUsed : nullptr));
                tmpMsg.signature = parsingToolClass::parsingMessages(tmpMsg.signature, settingsForMessageParsing.infoForMessageParsing, nullptr,
                                                                     (settingsForMessageParsing.downloadNoelshackImages == true ? &listOfNoelshackImagesUsed : nullptr));

                listOfNewMessages.push_front(tmpMsg);
            }
        }
    }

    if(firstTimeGetMessages == false || newTopicLink.isEmpty() == true)
    {
        QList<messageStruct> listOfEntireMessages;

        for(int i = listOfPageSource.size() - 1; i >= 0; --i)
        {
            listOfEntireMessages.append(parsingToolClass::getListOfEntireMessagesWithoutMessagePars(listOfPageSource[i]));
        }

        if(listOfEntireMessages.isEmpty() == true)
        {
            emit newMessagesAreAvailable(QList<messageStruct>(), listOfInput, ajaxInfo, topicLink, true);
            retrievesMessage = false;
            return;
        }

        for(messageStruct& currentMessage : listOfEntireMessages)
        {
            QMap<int, QString>::const_iterator listOfEditIterator = listOfEdit.find(currentMessage.idOfMessage);
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

                currentMessage.message = parsingToolClass::parsingMessages(currentMessage.message, settingsForMessageParsing.infoForMessageParsing,
                                                                           (settingsForMessageParsing.downloadMissingStickers == true ? &listOfStickersUsed : nullptr),
                                                                           (settingsForMessageParsing.downloadNoelshackImages == true ? &listOfNoelshackImagesUsed : nullptr));
                currentMessage.signature = parsingToolClass::parsingMessages(currentMessage.signature, settingsForMessageParsing.infoForMessageParsing, nullptr,
                                                                             (settingsForMessageParsing.downloadNoelshackImages == true ? &listOfNoelshackImagesUsed : nullptr));
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
        ajaxInfo = parsingToolClass::getAjaxInfo(listOfPageSource[firstValidePageNumber]);
        parsingToolClass::getListOfHiddenInputFromThisForm(listOfPageSource[firstValidePageNumber], "form-post-topic", listOfInput);

        if(listOfInput.isEmpty() == true)
        {
            if(parsingToolClass::getTopicLocked(listOfPageSource[firstValidePageNumber]) == true)
            {
                listOfInput.append(QPair<QString, QString>("locked", "true"));
            }
        }
    }
    firstTimeGetMessages = false;
    retrievesMessage = false;

    emit newMessagesAreAvailable(listOfNewMessages, listOfInput, ajaxInfo, topicLink, false);
    emit theseStickersAreUsed(listOfStickersUsed);
    emit theseNoelshackImagesAreUsed(listOfNoelshackImagesUsed);

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
