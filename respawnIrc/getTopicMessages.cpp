#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QVector>

#include "getTopicMessages.hpp"

getTopicMessagesClass::getTopicMessagesClass(QObject* parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);

    timeoutForFirstPage = new autoTimeoutReplyClass(this);
    timeoutForSecondPage = new autoTimeoutReplyClass(this);

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

void getTopicMessagesClass::settingsChanged(settingsForMessageParsingStruct newSettings)
{
    settingsForMessageParsing = newSettings;

    timerForGetMessage->setInterval(settingsForMessageParsing.timerTime);

    timeoutForFirstPage->updateTimeoutTime(settingsForMessageParsing.timeoutTime);
    timeoutForSecondPage->updateTimeoutTime(settingsForMessageParsing.timeoutTime);
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
                connect(replyForFirstPage, &QNetworkReply::finished, this, &getTopicMessagesClass::loadFirstPageFinish);
            }
            else
            {
                errorWhenTryToGetMessages = true;
            }

            if(settingsForMessageParsing.loadTwoLastPage == true && beforeLastPage.isEmpty() == false)
            {
                QNetworkRequest requestForSecondPage = parsingToolClass::buildRequestWithThisUrl(beforeLastPage);
                secondPageLoading = true;
                replyForSecondPage = timeoutForSecondPage->resetReply(networkManager->get(requestForSecondPage));
                if(replyForSecondPage->isOpen() == true)
                {
                    connect(replyForSecondPage, &QNetworkReply::finished, this, &getTopicMessagesClass::loadSecondPageFinish);
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
    if(settingsForMessageParsing.loadTwoLastPage == true && secondPageLoading == true)
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
    QList<messageStruct> listOfNewMessages;
    QList<QPair<QString, QString> > listOfInput;
    QStringList listOfStickersUsed;
    QStringList listOfNoelshackImagesUsed;
    QVector<QString> listOfPageSource;
    QVector<QNetworkReply*> listOfPageReply;
    int firstValidePageNumber = -1;

    timeoutForFirstPage->resetReply();
    timeoutForSecondPage->resetReply();

    listOfPageReply.append(replyForFirstPage);
    if(settingsForMessageParsing.loadTwoLastPage == true && secondPageLoading == true)
    {
        listOfPageReply.append(replyForSecondPage);
    }

    listOfPageSource.resize(listOfPageReply.size());

    for(int i = 0; i < listOfPageReply.size(); ++i)
    {
        if(listOfPageReply[i] != nullptr)
        {
            if(listOfPageReply[i]->isReadable() == true)
            {
                listOfPageSource[i] = listOfPageReply[i]->readAll();

                if(firstValidePageNumber == -1 && listOfPageSource[i].isEmpty() == false)
                {
                    firstValidePageNumber = i;
                }
            }
            listOfPageReply[i]->deleteLater();
        }
    }
    replyForFirstPage = nullptr;
    replyForSecondPage = nullptr;

    emit newMessageStatus("Récupération des messages terminée !");

    if(linkHasChanged == true)
    {
        retrievesMessage = false;
        startGetMessage();
        return;
    }

    if(firstValidePageNumber == -1)
    {
        emit newMessagesAreAvailable(QList<messageStruct>(), listOfInput, ajaxInfo, topicLink, true);
        retrievesMessage = false;
        return;
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

    if(newTopicLink.isEmpty() == true && topicLink != listOfPageReply[firstValidePageNumber]->request().url().toDisplayString())
    {
        newTopicLink = listOfPageReply[firstValidePageNumber]->request().url().toDisplayString();
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
