#include "showTopicMessages.hpp"
#include "parsingTool.hpp"
#include "settingTool.hpp"

showTopicMessagesClass::showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) : QWidget(parent)
{
    messagesBox.setReadOnly(true);
    messagesBox.setOpenExternalLinks(false);
    messagesBox.setOpenLinks(false);
    timerForGetMessage.setTimerType(Qt::CoarseTimer);
    updateSettingInfo(false);
    timerForGetMessage.stop();
    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    listOfColorPseudo = newListOfColorPseudo;
    messagesStatus = "Rien.";
    replyForFirstPage = 0;
    replyForSecondPage = 0;
    replyForEditInfo = 0;
    replyForQuoteInfo = 0;
    firstTimeGetMessages = true;
    retrievesMessage = false;
    idOfLastMessage = 0;
    idOfLastMessageOfUser = 0;
    linkHasChanged = false;
    errorMode = false;
    secondPageLoading = false;
    needToGetMessages = false;
    errorLastTime = false;

    networkManager = new QNetworkAccessManager(this);

    setNewTheme(currentThemeName);

    QSplitter* splitter = new QSplitter;
    splitter->addWidget(&messagesBox);
    splitter->addWidget(&showListOfTopic);
    splitter->setStretchFactor(0, 1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(splitter);
    layout->setMargin(0);

    setLayout(layout);

    QObject::connect(&timerForGetMessage, &QTimer::timeout, this, &showTopicMessagesClass::getMessages);
    QObject::connect(&messagesBox, &QTextBrowser::anchorClicked, this, &showTopicMessagesClass::linkClicked);
    QObject::connect(&showListOfTopic, &showListOfTopicClass::openThisTopic, this, &showTopicMessagesClass::topicNeedChanged);
    QObject::connect(&showListOfTopic, &showListOfTopicClass::openThisTopicInNewTab, this, &showTopicMessagesClass::openThisTopicInNewTab);
}

void showTopicMessagesClass::startGetMessage()
{
    needToGetMessages = false;
    if(topicLink.isEmpty() == false)
    {
        if(retrievesMessage == false)
        {
            timerForGetMessage.start();
            getMessages();
        }
        else
        {
            needToGetMessages = true;
        }
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

QString showTopicMessagesClass::getNumberOfConnectedAndMP()
{
    return numberOfConnectedAndMP;
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

void showTopicMessagesClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool updateMessagesAndList)
{
    currentCookieList = newCookies;
    pseudoOfUser = newPseudoOfUser;
    if(networkManager != 0)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
        errorLastTime = false;

        if(updateMessagesAndList == true)
        {
            showListOfTopic.setNewCookies(newCookies);
            startGetMessage();
        }
    }
}

void showTopicMessagesClass::setMessageStatus(QString newStatus)
{
    messagesStatus = newStatus;
    emit newMessageStatus();
}

void showTopicMessagesClass::setNumberOfConnectedAndMP(QString newNumber, bool forceSet)
{
    if(newNumber.isEmpty() == false || forceSet == true)
    {
        if(newNumber != numberOfConnectedAndMP)
        {
            numberOfConnectedAndMP = newNumber;
            emit newNumberOfConnectedAndMP();
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
            setNumberOfConnectedAndMP("", true);
            messageBox.warning(this, "Erreur", "Le topic n'existe pas.");
        }
        else
        {
            setMessageStatus("Erreur, impossible de récupérer les messages.");
            if(ignoreNetworkError == false)
            {
                messageBox.warning(this, "Erreur sur " + topicName, "Le programme n'a pas réussi à récupérer les messages cette fois ci, mais il continuera à essayer tant que l'onglet est ouvert.");
            }
        }
    }
    else
    {
        setMessageStatus("Erreur, impossible de récupérer les messages.");
    }
    retrievesMessage = false;
}

void showTopicMessagesClass::updateSettingInfo(bool showListOfTopicIfNeeded)
{
    showQuoteButton = settingToolClass::getThisBoolOption("showQuoteButton");
    showBlacklistButton = settingToolClass::getThisBoolOption("showBlacklistButton");
    showEditButton = settingToolClass::getThisBoolOption("showEditButton");
    showStickers = settingToolClass::getThisBoolOption("showStickers");
    loadTwoLastPage = settingToolClass::getThisBoolOption("loadTwoLastPage");
    ignoreNetworkError = settingToolClass::getThisBoolOption("ignoreNetworkError");
    timerForGetMessage.setInterval(settingToolClass::getThisIntOption("updateTopicTime"));
    numberOfMessageShowedFirstTime = settingToolClass::getThisIntOption("numberOfMessageShowedFirstTime");
    stickersSize = settingToolClass::getThisIntOption("stickersSize");

    if(settingToolClass::getThisBoolOption("showListOfTopic") == true)
    {
        if(showListOfTopic.isVisible() == false)
        {
            showListOfTopic.setForumLink(parsingToolClass::getForumOfTopic(topicLink));
            if(showListOfTopicIfNeeded == true)
            {
                showListOfTopic.setVisible(true);
            }
        }
    }
    else
    {
        if(showListOfTopic.isVisible() == true)
        {
            showListOfTopic.setForumLink("");
        }
        showListOfTopic.setVisible(false);
    }

    showListOfTopic.updateSettings();
}

void showTopicMessagesClass::setNewTheme(QString newThemeName)
{
    baseModel = styleToolClass::getModel(newThemeName);
    baseModelInfo = styleToolClass::getModelInfo(newThemeName);
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
    errorLastTime = false;
    idOfLastMessage = 0;
    idOfLastMessageOfUser = 0;

    showListOfTopic.setForumLink(parsingToolClass::getForumOfTopic(topicLink));

    if(retrievesMessage == false)
    {
        setMessageStatus("Nouveau topic.");
    }
    setNumberOfConnectedAndMP("", true);
    startGetMessage();
}

void showTopicMessagesClass::linkClicked(const QUrl &link)
{
    QString linkInString = link.toDisplayString();

    if(linkInString.startsWith("quote"))
    {
        lastMessageQuoted.clear();
        linkInString.remove(0, linkInString.indexOf(':') + 1);
        lastMessageQuoted = linkInString.mid(linkInString.indexOf(':') + 1);
        getQuoteInfo(linkInString.left(linkInString.indexOf(':')));
    }
    else if(linkInString.startsWith("blacklist"))
    {
        emit addToBlacklist(linkInString.remove(0, linkInString.indexOf(':') + 1));
    }
    else if(linkInString.startsWith("edit"))
    {
        emit editThisMessage(linkInString.remove(0, linkInString.indexOf(':') + 1).toInt(), true);
    }
    else
    {
        QDesktopServices::openUrl(link);
    }
}

bool showTopicMessagesClass::getEditInfo(int idOfMessageToEdit, bool useMessageEdit)
{
    if(networkManager == 0)
    {
        return false;
    }
    if(networkManager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        return false;
    }

    if(ajaxInfo.isEmpty() == false && pseudoOfUser.isEmpty() == false && idOfLastMessageOfUser != 0)
    {
        if(replyForEditInfo == 0)
        {
            QString urlToGet;
            QNetworkRequest requestForEditInfo;

            if(idOfMessageToEdit == 0)
            {
                oldIdOfLastMessageOfUser = idOfLastMessageOfUser;
            }
            else
            {
                oldIdOfLastMessageOfUser = idOfMessageToEdit;
            }

            urlToGet = "http://www.jeuxvideo.com/forums/ajax_edit_message.php?id_message=" + QString::number(oldIdOfLastMessageOfUser) + "&" + ajaxInfo + "&action=get";
            requestForEditInfo = parsingToolClass::buildRequestWithThisUrl(urlToGet);
            oldAjaxInfo = ajaxInfo;
            ajaxInfo.clear();
            oldUseMessageEdit = useMessageEdit;
            replyForEditInfo = networkManager->get(requestForEditInfo);
            QObject::connect(replyForEditInfo, &QNetworkReply::finished, this, &showTopicMessagesClass::analyzeEditInfo);

            return true;
        }
    }

    return false;
}

void showTopicMessagesClass::getQuoteInfo(QString idOfMessageQuoted)
{
    if(networkManager == 0)
    {
        return;
    }
    if(networkManager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        return;
    }

    if(ajaxInfo.isEmpty() == false && replyForQuoteInfo == 0)
    {
        QNetworkRequest requestForQuoteInfo = parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com/forums/ajax_citation.php");
        QString dataForQuote = "id_message=" + idOfMessageQuoted + "&" + ajaxInfo;
        replyForQuoteInfo = networkManager->post(requestForQuoteInfo, dataForQuote.toLatin1());
        QObject::connect(replyForQuoteInfo, &QNetworkReply::finished, this, &showTopicMessagesClass::analyzeQuoteInfo);
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Erreur, impossible de citer ce message, réessayez.");
    }
}

void showTopicMessagesClass::getMessages()
{
    bool itsNewManager = false;

    if(networkManager == 0)
    {
        itsNewManager = true;
        networkManager = new QNetworkAccessManager(this);
    }

    if(retrievesMessage == false)
    {
        if(replyForFirstPage == 0)
        {
            if(networkManager->networkAccessible() != QNetworkAccessManager::Accessible)
            {
                setMessageStatus("Pas de connexion internet.");
                delete networkManager;
                networkManager = 0;
                return;
            }

            if(itsNewManager == true)
            {
                setNewCookies(currentCookieList, pseudoOfUser, false);
            }

            retrievesMessage = true;

            QString beforeLastPage = parsingToolClass::getBeforeLastPageOfTopic(topicLink);
            QNetworkRequest requestForFirstPage = parsingToolClass::buildRequestWithThisUrl(topicLink);
            setMessageStatus("Récupération des messages en cours...");
            secondPageLoading = false;
            linkHasChanged = false;
            replyForFirstPage = networkManager->get(requestForFirstPage);
            QObject::connect(replyForFirstPage, &QNetworkReply::finished, this, &showTopicMessagesClass::loadFirstPageFinish);

            if(loadTwoLastPage == true && beforeLastPage.isEmpty() == false)
            {
                QNetworkRequest requestForSecondPage = parsingToolClass::buildRequestWithThisUrl(beforeLastPage);
                secondPageLoading = true;
                replyForSecondPage = networkManager->get(requestForSecondPage);
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

void showTopicMessagesClass::analyzeEditInfo()
{
    QString message;
    QString dataToSend = oldAjaxInfo + "&action=post";
    QList<QPair<QString, QString> > listOfEditInput;
    QString source;
    if(replyForEditInfo->isReadable())
    {
        source = replyForEditInfo->readAll();
    }
    replyForEditInfo->deleteLater();

    message = parsingToolClass::getMessageEdit(source);
    parsingToolClass::getListOfHiddenInputFromThisForm(source, "form-post-topic", listOfEditInput);

    for(int i = 0; i < listOfEditInput.size(); ++i)
    {
        dataToSend += "&" + listOfEditInput.at(i).first + "=" + listOfEditInput.at(i).second;
    }

    emit setEditInfo(oldIdOfLastMessageOfUser, message, dataToSend, parsingToolClass::getCaptchaLink(source), oldUseMessageEdit);

    replyForEditInfo = 0;
}

void showTopicMessagesClass::analyzeQuoteInfo()
{
    QString messageQuote;
    QString source;
    if(replyForQuoteInfo->isReadable())
    {
        source = replyForQuoteInfo->readAll();
    }
    replyForQuoteInfo->deleteLater();

    messageQuote = parsingToolClass::getMessageQuote(source);

    messageQuote = ">" + QUrl::fromPercentEncoding(lastMessageQuoted.toLatin1()) + "\n>" + messageQuote;
    replyForQuoteInfo = 0;

    emit quoteThisMessage(messageQuote);
}

void showTopicMessagesClass::analyzeMessages()
{
    QString newTopicLink;
    QString colorOfPseudo;
    QString colorOfDate;
    QString buttonString;
    QString sourceFirst;
    QString sourceSecond;
    QString numberOfConnected;

    if(replyForFirstPage == 0)
    {
        retrievesMessage = false;
        setMessageStatus("Erreur.");
        return;
    }

    if(replyForFirstPage->isReadable())
    {
        sourceFirst = replyForFirstPage->readAll();
    }
    replyForFirstPage->deleteLater();
    replyForFirstPage = 0;

    if(loadTwoLastPage == true && secondPageLoading == true)
    {
        if(replyForSecondPage->isReadable())
        {
            sourceSecond = replyForSecondPage->readAll();
        }
        replyForSecondPage->deleteLater();
        replyForSecondPage = 0;
    }

    setMessageStatus("Récupération des messages terminée !");

    if(linkHasChanged == true)
    {
        retrievesMessage = false;
        startGetMessage();
        return;
    }

    if(pseudoOfUser.isEmpty() == false)
    {
        numberOfConnected = parsingToolClass::getNumberOfConnected(sourceFirst);
        setNumberOfConnectedAndMP(numberOfConnected + " - " + parsingToolClass::getNumberOfMp(sourceFirst));
    }
    else
    {
        setNumberOfConnectedAndMP(parsingToolClass::getNumberOfConnected(sourceFirst));
    }

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
            listOfEntireMessage = parsingToolClass::getListOfEntireMessages(sourceSecond, showStickers, stickersSize);
        }

        listOfEntireMessage.append(parsingToolClass::getListOfEntireMessages(sourceFirst, showStickers, stickersSize));

        if(listOfEntireMessage.size() == 0)
        {
            if(errorLastTime == true)
            {
                setTopicToErrorMode();
            }
            else
            {
                errorLastTime = true;
            }
            retrievesMessage = false;
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
                QString newMessageToAppend = baseModel;
                buttonString.clear();
                colorOfPseudo.clear();
                colorOfPseudo = getColorOfThisPseudo(listOfEntireMessage.at(i).pseudo.toLower());

                if(colorOfPseudo.isEmpty() == true)
                {
                    if(pseudoOfUser.toLower() == listOfEntireMessage.at(i).pseudo.toLower())
                    {
                        colorOfPseudo = baseModelInfo.userPseudoColor;
                    }
                    else
                    {
                        colorOfPseudo = baseModelInfo.normalPseudoColor;
                    }
                }

                if(listOfEditIterator != listOfEdit.end() && listOfEditIterator.value() != listOfEntireMessage.at(i).lastTimeEdit)
                {
                    colorOfDate = baseModelInfo.editDateColor;
                }
                else
                {
                    colorOfDate = baseModelInfo.normalDateColor;
                    idOfLastMessage = listOfEntireMessage.at(i).idOfMessage;

                    if(pseudoOfUser.toLower() == listOfEntireMessage.at(i).pseudo.toLower())
                    {
                        idOfLastMessageOfUser = idOfLastMessage;
                    }
                }

                if(showQuoteButton == true)
                {
                    newMessageToAppend.replace("<%BUTTON_QUOTE%>", baseModelInfo.quoteModel);
                }

                if(pseudoOfUser.toLower() == listOfEntireMessage.at(i).pseudo.toLower())
                {
                    if(showEditButton == true)
                    {
                        newMessageToAppend.replace("<%BUTTON_EDIT%>", baseModelInfo.editModel);
                    }
                }
                else if(showBlacklistButton == true)
                {
                    newMessageToAppend.replace("<%BUTTON_BLACKLIST%>", baseModelInfo.blacklistModel);
                }

                newMessageToAppend.replace("<%DATE_COLOR%>", colorOfDate);
                newMessageToAppend.replace("<%PSEUDO_LOWER%>", listOfEntireMessage.at(i).pseudo.toLower());
                newMessageToAppend.replace("<%ID_MESSAGE%>", QString::number(listOfEntireMessage.at(i).idOfMessage));
                newMessageToAppend.replace("<%DATE_MESSAGE%>", listOfEntireMessage.at(i).date);
                newMessageToAppend.replace("<%PSEUDO_COLOR%>", colorOfPseudo);
                newMessageToAppend.replace("<%PSEUDO_PSEUDO%>", listOfEntireMessage.at(i).pseudo);
                newMessageToAppend.replace("<%MESSAGE_MESSAGE%>", listOfEntireMessage.at(i).message);

                messagesBox.append(newMessageToAppend);
                if(messagesBox.verticalScrollBar()->value() >= messagesBox.verticalScrollBar()->maximum())
                {
                    messagesBox.verticalScrollBar()->updateGeometry();
                    messagesBox.verticalScrollBar()->setValue(messagesBox.verticalScrollBar()->maximum());
                }
                listOfEdit[listOfEntireMessage.at(i).idOfMessage] = listOfEntireMessage.at(i).lastTimeEdit;
                if(pseudoOfUser.toLower() != listOfEntireMessage.at(i).pseudo.toLower())
                {
                    emit newMessagesAvailable();
                }
            }
        }

        while(listOfEdit.size() > 40)
        {
            listOfEdit.erase(listOfEdit.begin());
        }
    }

    if(pseudoOfUser.isEmpty() == false)
    {
        ajaxInfo = parsingToolClass::getAjaxInfo(sourceFirst);
        listOfInput.clear();
        parsingToolClass::getListOfHiddenInputFromThisForm(sourceFirst, "form-post-topic", listOfInput);
        captchaLink = parsingToolClass::getCaptchaLink(sourceFirst);

        if(listOfInput.isEmpty() == true)
        {
            if(errorLastTime == true)
            {
                if(ignoreNetworkError == false)
                {
                    QMessageBox messageBox;
                    messageBox.warning(this, "Erreur sur " + topicName + " avec " + pseudoOfUser,
                                       "Le compte semble invalide, si tel est vraiment le cas veuillez supprimer celui-ci de la liste des comptes et vous reconnecter avec.");
                    pseudoOfUser.clear();
                    setNumberOfConnectedAndMP(numberOfConnected);
                }
            }
            else
            {
                errorLastTime = true;
            }
        }
        else
        {
            errorLastTime = false;
        }
    }
    else
    {
        errorLastTime = false;
    }

    firstTimeGetMessages = false;
    retrievesMessage = false;

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
