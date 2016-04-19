#include "getTopicMessages.hpp"

getTopicMessagesClass::getTopicMessagesClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);

    timeoutForFirstPage = new autoTimeoutReplyClass(this);
    timeoutForSecondPage = new autoTimeoutReplyClass(this);

    timerForGetMessage = new QTimer(this);

    timerForGetMessage->setTimerType(Qt::CoarseTimer);
    timerForGetMessage->setInterval(5000);

    QObject::connect(timerForGetMessage, &QTimer::timeout, this, &getTopicMessagesClass::getMessages);
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

    if(needToGetFirstMessage == true)
    {
        topicLink = parsingToolClass::getFirstPageOfTopic(newTopicLink);
    }
    else
    {
        topicLink = newTopicLink;
    }

    if(retrievesMessage == false)
    {
        emit newMessageStatus("Nouveau topic.");
    }
    else
    {
        if(replyForFirstPage != nullptr)
        {
            if(replyForFirstPage->isRunning())
            {
                replyForFirstPage->abort();
            }
        }

        if(replyForSecondPage != nullptr)
        {
            if(replyForSecondPage->isRunning())
            {
                replyForSecondPage->abort();
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

void getTopicMessagesClass::settingsChanged(bool getTwoLastPages, int timerTime, bool newShowStickers, int newStickerSize,
                                            int timeoutTime, int newMaxNbOfQuotes, bool newStickersToSmiley, bool newBetterQuote)
{
    loadTwoLastPage = getTwoLastPages;
    showStickers = newShowStickers;
    stickerToSmiley = newStickersToSmiley;
    stickersSize = newStickerSize;
    maxNbOfQuotes = newMaxNbOfQuotes;
    betterQuote = newBetterQuote;

    timerForGetMessage->setInterval(timerTime);

    timeoutForFirstPage->updateTimeoutTime(timeoutTime);
    timeoutForSecondPage->updateTimeoutTime(timeoutTime);
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
    bool errorWhenTryToGetMessages = false;

    if(networkManager == nullptr)
    {
        itsNewManager = true;
        networkManager = new QNetworkAccessManager(this);
    }

    if(retrievesMessage == false)
    {
        if(replyForFirstPage == nullptr)
        {
            if(itsNewManager == true || needToSetCookies == true)
            {
                setNewCookies(currentCookieList, pseudoOfUser, false);
                needToSetCookies = false;
            }

            retrievesMessage = true;

            QString beforeLastPage = parsingToolClass::getBeforeLastPageOfTopic(topicLink);
            QNetworkRequest requestForFirstPage = parsingToolClass::buildRequestWithThisUrl(topicLink);
            emit newMessageStatus("Récupération des messages en cours...");
            secondPageLoading = false;
            linkHasChanged = false;
            replyForFirstPage = timeoutForFirstPage->resetReply(networkManager->get(requestForFirstPage));
            if(replyForFirstPage->isOpen() == true)
            {
                QObject::connect(replyForFirstPage, &QNetworkReply::finished, this, &getTopicMessagesClass::loadFirstPageFinish);
            }
            else
            {
                errorWhenTryToGetMessages = true;
            }

            if(loadTwoLastPage == true && beforeLastPage.isEmpty() == false)
            {
                QNetworkRequest requestForSecondPage = parsingToolClass::buildRequestWithThisUrl(beforeLastPage);
                secondPageLoading = true;
                replyForSecondPage = timeoutForSecondPage->resetReply(networkManager->get(requestForSecondPage));
                if(replyForSecondPage->isOpen() == true)
                {
                    QObject::connect(replyForSecondPage, &QNetworkReply::finished, this, &getTopicMessagesClass::loadSecondPageFinish);
                }
                else
                {
                    errorWhenTryToGetMessages = true;
                }
            }

            if(errorWhenTryToGetMessages == true)
            {
                analyzeMessages();
                networkManager->deleteLater();
                networkManager = nullptr;
            }
        }
        else
        {
            retrievesMessage = false;
        }
    }
}


void getTopicMessagesClass::loadFirstPageFinish()
{
    if(loadTwoLastPage == true && secondPageLoading == true)
    {
        if(replyForSecondPage != nullptr)
        {
            if(replyForSecondPage->isFinished() == true)
            {
                analyzeMessages();
            }
        }
    }
    else
    {
        analyzeMessages();
    }
}

void getTopicMessagesClass::loadSecondPageFinish()
{
    if(replyForFirstPage != nullptr)
    {
        if(replyForFirstPage->isFinished() == true)
        {
            analyzeMessages();
        }
    }
}

void getTopicMessagesClass::analyzeMessages()
{
    QString newTopicLink;
    ajaxInfoStruct ajaxInfo;
    QString sourceFirst;
    QString sourceSecond;
    QString numberOfConnected;
    QList<messageStruct> listOfNewMessages;
    QList<QPair<QString, QString> > listOfInput;

    timeoutForFirstPage->resetReply();
    timeoutForSecondPage->resetReply();

    if(replyForFirstPage != nullptr)
    {
        if(replyForFirstPage->isReadable())
        {
            sourceFirst = replyForFirstPage->readAll();
            if(needToSetCookies == false)
            {
                bool cookiesChanged = false;
                QList<QNetworkCookie> newCookieList = qvariant_cast<QList<QNetworkCookie> >(replyForFirstPage->header(QNetworkRequest::SetCookieHeader));

                for(const QNetworkCookie& thisNewCookie : newCookieList)
                {
                    if(thisNewCookie.name() == "dlrowolleh" || thisNewCookie.name() == "coniunctio")
                    {
                        for(int j = 0; j < currentCookieList.size(); ++j)
                        {
                            if(currentCookieList.at(j).name() == thisNewCookie.name())
                            {
                                if(currentCookieList.at(j).expirationDate() <= QDateTime::currentDateTime().addDays(7))
                                {
                                    currentCookieList.replace(j, thisNewCookie);
                                    cookiesChanged = true;
                                }
                                break;
                            }
                        }
                    }
                }

                if(cookiesChanged == true)
                {
                    emit newCookiesHaveToBeSet(currentCookieList, pseudoOfUser);
                }
            }
        }
        replyForFirstPage->deleteLater();
        replyForFirstPage = nullptr;
    }

    if(loadTwoLastPage == true && secondPageLoading == true)
    {
        if(replyForSecondPage != nullptr)
        {
            if(replyForSecondPage->isReadable())
            {
                sourceSecond = replyForSecondPage->readAll();
            }
            replyForSecondPage->deleteLater();
            replyForSecondPage = nullptr;
        }
    }

    emit newMessageStatus("Récupération des messages terminée !");

    if(linkHasChanged == true)
    {
        retrievesMessage = false;
        startGetMessage();
        return;
    }

    if(currentCookieList.isEmpty() == false)
    {
        numberOfConnected = parsingToolClass::getNumberOfConnected(sourceFirst);
        emit newNumberOfConnectedAndMP(numberOfConnected, parsingToolClass::getNumberOfMp(sourceFirst), false);
    }
    else
    {
        emit newNumberOfConnectedAndMP(parsingToolClass::getNumberOfConnected(sourceFirst), "", false);
    }

    newTopicLink = parsingToolClass::getLastPageOfTopic(sourceFirst);

    if(firstTimeGetMessages == true)
    {
        QString topicName = parsingToolClass::getNameOfTopic(sourceFirst);

        if(topicName.isEmpty() == false)
        {
            emit newNameForTopic(topicName);
        }

        if(needToGetFirstMessage == true)
        {
            QList<messageStruct> tmpList = parsingToolClass::getListOfEntireMessagesWithoutMessagePars(sourceFirst);

            if(tmpList.isEmpty() == false)
            {
                messageStruct tmpMsg = tmpList.first();

                tmpMsg.isFirstMessage = true;
                tmpMsg.isAnEdit = false;
                tmpMsg.message = parsingToolClass::parsingMessages(tmpMsg.message, showStickers, stickerToSmiley, stickersSize, maxNbOfQuotes, betterQuote);

                listOfNewMessages.push_front(tmpMsg);
            }
        }
    }

    if(firstTimeGetMessages == false || newTopicLink.isEmpty() == true)
    {
        QList<messageStruct> listOfEntireMessages;
        if(sourceSecond.isEmpty() == false)
        {
            listOfEntireMessages = parsingToolClass::getListOfEntireMessagesWithoutMessagePars(sourceSecond);
        }

        listOfEntireMessages.append(parsingToolClass::getListOfEntireMessagesWithoutMessagePars(sourceFirst));

        if(listOfEntireMessages.isEmpty() == true)
        {
            emit newMessagesAreAvailable(listOfEntireMessages, listOfInput, ajaxInfo, topicLink, true);
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

                currentMessage.message = parsingToolClass::parsingMessages(currentMessage.message, showStickers, stickerToSmiley, stickersSize, maxNbOfQuotes, betterQuote);
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
        ajaxInfo = parsingToolClass::getAjaxInfo(sourceFirst);
        parsingToolClass::getListOfHiddenInputFromThisForm(sourceFirst, "form-post-topic", listOfInput);
    }
    firstTimeGetMessages = false;
    retrievesMessage = false;

    emit newMessagesAreAvailable(listOfNewMessages, listOfInput, ajaxInfo, topicLink, false);

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
