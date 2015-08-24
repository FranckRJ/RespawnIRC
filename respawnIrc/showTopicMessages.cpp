#include "showTopicMessages.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

showTopicMessagesClass::showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QWidget* parent) : QWidget(parent)
{
    messagesBox.setReadOnly(true);
    messagesBox.setOpenExternalLinks(true);
    timerForGetMessage.setTimerType(Qt::CoarseTimer);
    timerForGetMessage.setInterval(settingToolClass::getUpdateTopicTime());
    timerForGetMessage.stop();
    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    listOfColorPseudo = newListOfColorPseudo;
    messagesStatus = "Rien.";
    replyForFirstPage = 0;
    replyForSecondPage = 0;
    firstTimeGetMessages = true;
    retrievesMessage = false;
    idOfLastMessage = 0;
    linkHasChanged = false;
    errorMode = false;
    loadTwoLastPage = settingToolClass::getLoadTwoLastPage();
    numberOfMessageShowedFirstTime = settingToolClass::getNumberOfMessageShowedFirstTime();
    secondPageLoading = false;

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&messagesBox);
    layout->setMargin(0);

    setLayout(layout);

    QObject::connect(&timerForGetMessage, &QTimer::timeout, this, &showTopicMessagesClass::getMessages);
}

void showTopicMessagesClass::startGetMessage()
{
    if(retrievesMessage == false && topicLink.isEmpty() == false)
    {
        timerForGetMessage.start();
        getMessages();
    }
}

const QList<QPair<QString, QString> >& showTopicMessagesClass::getListOfInput()
{
    return listOfInput;
}

QString showTopicMessagesClass::getTopicLink()
{
    return topicLink;
}

QString showTopicMessagesClass::getTopicName()
{
    return topicName;
}

QString showTopicMessagesClass::getCaptchaLink()
{
    return captchaLink;
}

QString showTopicMessagesClass::getMessagesStatus()
{
    return messagesStatus;
}

QString showTopicMessagesClass::getNumberOfConnected()
{
    return numberOfConnected;
}

QString showTopicMessagesClass::getPseudoUsed()
{
    return pseudoOfUser;
}

QString showTopicMessagesClass::getColorOfThisPseudo(QString pseudo)
{
    for(int i = 0; i < listOfColorPseudo->size(); ++i)
    {
        if(listOfColorPseudo->at(i).pseudo == pseudo)
        {
            return "rgb(" + QString::number(listOfColorPseudo->at(i).red) + ", " + QString::number(listOfColorPseudo->at(i).green) + ", " + QString::number(listOfColorPseudo->at(i).blue) + ")";
        }
    }

    return "";
}

const QList<QNetworkCookie>& showTopicMessagesClass::getListOfCookies()
{
    return currentCookieList;
}

void showTopicMessagesClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser)
{
    networkManager.setCookieJar(new QNetworkCookieJar(this));
    networkManager.cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
    currentCookieList = newCookies;
    pseudoOfUser = newPseudoOfUser;

    startGetMessage();
}

void showTopicMessagesClass::setNewTopic(QString newTopic)
{
    messagesBox.clear();
    topicName.clear();
    listOfEdit.clear();
    topicLink = newTopic;
    linkHasChanged = true;
    firstTimeGetMessages = true;
    errorMode = false;
    idOfLastMessage = 0;

    setMessageStatus("Nouveau topic.");
    setNumberOfConnected("", true);
    startGetMessage();
}

void showTopicMessagesClass::setMessageStatus(QString newStatus)
{
    messagesStatus = newStatus;
    emit newMessageStatus();
}

void showTopicMessagesClass::setNumberOfConnected(QString newNumber, bool forceSet)
{
    if(newNumber.isEmpty() == false || forceSet == true)
    {
        if(newNumber != numberOfConnected)
        {
            numberOfConnected = newNumber;
            emit newNumberOfConnected();
        }
    }
}

void showTopicMessagesClass::setTopicToErrorMode()
{
    if(errorMode == false)
    {
        QMessageBox messageBox;
        errorMode = true;
        if(firstTimeGetMessages == true)
        {
            topicLink.clear();
            topicName.clear();
            timerForGetMessage.stop();
            messagesBox.clear();
            listOfEdit.clear();
            setMessageStatus("Erreur, topic invalide.");
            setNumberOfConnected("", true);
            messageBox.warning(this, "Erreur", "Le topic n'existe pas.");
        }
        else
        {
            setMessageStatus("Erreur, impossible de récupérer les messages.");
            messageBox.warning(this, "Erreur sur " + topicName, "Le programme n'a pas réussi à récupérer les messages cette fois ci, mais il continuera à essayer tant que l'onglet est ouvert.");
        }
    }
    else
    {
        setMessageStatus("Erreur, impossible de récupérer les messages.");
    }
    retrievesMessage = false;
}

void showTopicMessagesClass::updateSettingInfo()
{
    loadTwoLastPage = settingToolClass::getLoadTwoLastPage();
    timerForGetMessage.setInterval(settingToolClass::getUpdateTopicTime());
    numberOfMessageShowedFirstTime = settingToolClass::getNumberOfMessageShowedFirstTime();
}

void showTopicMessagesClass::getMessages()
{
    if(retrievesMessage == false)
    {
        retrievesMessage = true;

        if(replyForFirstPage == 0)
        {
            QString beforeLastPage = parsingToolClass::getBeforeLastPageOfTopic(topicLink);
            QNetworkRequest requestForFirstPage = parsingToolClass::buildRequestWithThisUrl(topicLink);
            setMessageStatus("Récupération des messages en cours...");
            secondPageLoading = false;
            linkHasChanged = false;
            replyForFirstPage = networkManager.get(requestForFirstPage);
            QObject::connect(replyForFirstPage, &QNetworkReply::finished, this, &showTopicMessagesClass::loadFirstPageFinish);

            if(loadTwoLastPage == true && beforeLastPage.isEmpty() == false)
            {
                QNetworkRequest requestForSecondPage = parsingToolClass::buildRequestWithThisUrl(beforeLastPage);
                secondPageLoading = true;
                replyForSecondPage = networkManager.get(requestForSecondPage);
                QObject::connect(replyForSecondPage, &QNetworkReply::finished, this, &showTopicMessagesClass::loadSecondPageFinish);
            }
        }
        else
        {
            retrievesMessage = false;
        }
    }
}

void showTopicMessagesClass::loadFirstPageFinish()
{
    if(loadTwoLastPage == true && secondPageLoading == true)
    {
        if(replyForSecondPage != 0)
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

void showTopicMessagesClass::loadSecondPageFinish()
{
    if(replyForFirstPage != 0)
    {
        if(replyForFirstPage->isFinished() == true)
        {
            analyzeMessages();
        }
    }
}

void showTopicMessagesClass::analyzeMessages()
{
    QString newTopicLink;
    QString colorOfPseudo;
    QString colorOfDate;
    QString sourceFirst;
    QString sourceSecond;

    if(replyForFirstPage == 0)
    {
        return;
    }

    sourceFirst = replyForFirstPage->readAll();
    replyForFirstPage->deleteLater();
    replyForFirstPage = 0;

    if(loadTwoLastPage == true && secondPageLoading == true)
    {
        sourceSecond = replyForSecondPage->readAll();
        replyForSecondPage->deleteLater();
        replyForSecondPage = 0;
    }

    if(linkHasChanged == true)
    {
        retrievesMessage = false;
        return;
    }

    setMessageStatus("Récupération des messages terminé !");
    setNumberOfConnected(parsingToolClass::getNumberOfConnected(sourceFirst));

    newTopicLink = parsingToolClass::getLastPageOfTopic(sourceFirst);

    if(firstTimeGetMessages == true)
    {
        topicName = parsingToolClass::getNameOfTopic(sourceFirst);

        if(topicName.isEmpty() == false)
        {
            emit newNameForTopic(topicName);
        }
    }

    if(firstTimeGetMessages == false || newTopicLink.isEmpty() == true)
    {
        QList<messageStruct> listOfEntireMessage;

        if(sourceSecond.isEmpty() == false)
        {
            listOfEntireMessage = parsingToolClass::getListOfEntireMessages(sourceSecond);
        }

        listOfEntireMessage.append(parsingToolClass::getListOfEntireMessages(sourceFirst));

        if(listOfEntireMessage.size() == 0)
        {
            setTopicToErrorMode();
            return;
        }
        else
        {
            errorMode = false;
        }

        if(messagesBox.toPlainText().isEmpty() == true)
        {
            while(listOfEntireMessage.size() > numberOfMessageShowedFirstTime)
            {
                listOfEntireMessage.pop_front();
            }
        }

        for(int i = 0; i < listOfEntireMessage.size(); ++i)
        {
            QMap<int, QString>::const_iterator listOfEditIterator = listOfEdit.find(listOfEntireMessage.at(i).idOfMessage);
            if((listOfEntireMessage.at(i).idOfMessage > idOfLastMessage || (listOfEditIterator != listOfEdit.end() && listOfEditIterator.value() != listOfEntireMessage.at(i).lastTimeEdit))
                    && listOfIgnoredPseudo->indexOf(listOfEntireMessage.at(i).pseudo.toLower()) == -1)
            {
                colorOfPseudo.clear();
                colorOfPseudo = getColorOfThisPseudo(listOfEntireMessage.at(i).pseudo.toLower());

                if(colorOfPseudo.isEmpty() == true)
                {
                    if(pseudoOfUser.toLower() == listOfEntireMessage.at(i).pseudo.toLower())
                    {
                        colorOfPseudo = "blue";
                    }
                    else
                    {
                        colorOfPseudo = "dimgrey";
                    }
                }

                if(listOfEditIterator != listOfEdit.end() && listOfEditIterator.value() != listOfEntireMessage.at(i).lastTimeEdit)
                {
                    colorOfDate = "green";
                }
                else
                {
                    colorOfDate = "black";
                    idOfLastMessage = listOfEntireMessage.at(i).idOfMessage;
                }

                messagesBox.append("<table><tr><td>[<a style=\"color: " + colorOfDate + ";text-decoration: none\" href=\"http://www.jeuxvideo.com/" +
                                   listOfEntireMessage.at(i).pseudo.toLower() +
                                   "/forums/message/" + QString::number(listOfEntireMessage.at(i).idOfMessage) + "\">" + listOfEntireMessage.at(i).date +
                                   "</a>] &lt;<a href=\"http://www.jeuxvideo.com/profil/" + listOfEntireMessage.at(i).pseudo.toLower() +
                                   "?mode=infos\"><span style=\"color: " + colorOfPseudo + ";text-decoration: none\">" +
                                   listOfEntireMessage.at(i).pseudo + "</span></a>&gt;</td><td>" + listOfEntireMessage.at(i).message + "</td></tr></table>");
                messagesBox.verticalScrollBar()->updateGeometry();
                messagesBox.verticalScrollBar()->setValue(messagesBox.verticalScrollBar()->maximum());
                listOfEdit[listOfEntireMessage.at(i).idOfMessage] = listOfEntireMessage.at(i).lastTimeEdit;
                emit newMessagesAvailable();
            }
        }

        while(listOfEdit.size() > 40)
        {
            listOfEdit.erase(listOfEdit.begin());
        }
    }

    if(pseudoOfUser.isEmpty() == false)
    {
        listOfInput.clear();
        parsingToolClass::getListOfHiddenInputFromThisForm(sourceFirst, "form-post-topic form-post-message", listOfInput);
        captchaLink = parsingToolClass::getCaptchaLink(sourceFirst);

        if(listOfInput.isEmpty() == true)
        {
            QMessageBox messageBox;
            messageBox.warning(this, "Erreur sur " + topicName + " avec " + pseudoOfUser, "Le compte semble invalide, si tel est vraiment le cas veuillez supprimer celui-ci de la liste des comptes et vous reconnecter avec.");
            pseudoOfUser.clear();
        }
    }

    firstTimeGetMessages = false;
    retrievesMessage = false;

    if(newTopicLink.isEmpty() == false)
    {
        topicLink = newTopicLink;
        startGetMessage();
    }
}
