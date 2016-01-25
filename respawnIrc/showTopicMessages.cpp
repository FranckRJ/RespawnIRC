#include "showTopicMessages.hpp"
#include "settingTool.hpp"

showTopicMessagesClass::showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) : QWidget(parent)
{
    messagesBox.setReadOnly(true);
    messagesBox.setOpenExternalLinks(false);
    messagesBox.setOpenLinks(false);
    messagesBox.setSearchPaths(QStringList(QCoreApplication::applicationDirPath()));
    timerForGetMessage.setTimerType(Qt::CoarseTimer);
    updateSettingInfo();
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
    oldIdOfLastMessageOfUser = 0;
    linkHasChanged = false;
    errorMode = false;
    secondPageLoading = false;
    needToGetMessages = false;
    errorLastTime = false;
    needToSetCookies = false;
    oldUseMessageEdit = false;

    networkManager = new QNetworkAccessManager(this);

    setNewTheme(currentThemeName);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&messagesBox);
    layout->setMargin(0);

    setLayout(layout);

    QObject::connect(&timerForGetMessage, &QTimer::timeout, this, &showTopicMessagesClass::getMessages);
    QObject::connect(&messagesBox, &QTextBrowser::anchorClicked, this, &showTopicMessagesClass::linkClicked);
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

void showTopicMessagesClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool updateMessages)
{
    if(newPseudoOfUser == ".")
    {
        newPseudoOfUser.clear();
    }

    currentCookieList = newCookies;
    pseudoOfUser = newPseudoOfUser;
    listOfInput.clear();

    if(networkManager != 0)
    {
        if(retrievesMessage == false)
        {
            networkManager->clearAccessCache();
            networkManager->setCookieJar(new QNetworkCookieJar(this));
            networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
            errorLastTime = false;

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

void showTopicMessagesClass::updateSettingInfo()
{
    showQuoteButton = settingToolClass::getThisBoolOption("showQuoteButton");
    showBlacklistButton = settingToolClass::getThisBoolOption("showBlacklistButton");
    showEditButton = settingToolClass::getThisBoolOption("showEditButton");
    showStickers = settingToolClass::getThisBoolOption("showStickers");
    loadTwoLastPage = settingToolClass::getThisBoolOption("loadTwoLastPage");
    ignoreNetworkError = settingToolClass::getThisBoolOption("ignoreNetworkError");
    colorModoAndAdminPseudo = settingToolClass::getThisBoolOption("colorModoAndAdminPseudo");
    colorPEMT = settingToolClass::getThisBoolOption("colorPEMT");
    timerForGetMessage.setInterval(settingToolClass::getThisIntOption("updateTopicTime"));
    numberOfMessageShowedFirstTime = settingToolClass::getThisIntOption("numberOfMessageShowedFirstTime");
    stickersSize = settingToolClass::getThisIntOption("stickersSize");
    getFirstMessageOfTopic = settingToolClass::getThisBoolOption("getFirstMessageOfTopic");
    warnWhenEdit = settingToolClass::getThisBoolOption("warnWhenEdit");

    if(getFirstMessageOfTopic == false)
    {
        firstMessageOfTopic.pseudoInfo.pseudoName.clear();
    }
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
    lastDate.clear();

    if(getFirstMessageOfTopic == true)
    {
        topicLink = parsingToolClass::getFirstPageOfTopic(newTopic);
    }
    else
    {
        topicLink = newTopic;
    }

    linkHasChanged = true;
    firstTimeGetMessages = true;
    errorMode = false;
    errorLastTime = false;
    idOfLastMessage = 0;
    idOfLastMessageOfUser = 0;
    oldIdOfLastMessageOfUser = 0;
    needToGetMessages = false;
    oldUseMessageEdit = false;

    if(retrievesMessage == false)
    {
        setMessageStatus("Nouveau topic.");
    }
    else
    {
        if(replyForFirstPage != 0)
        {
            if(replyForFirstPage->isRunning())
            {
                replyForFirstPage->abort();
            }
        }

        if(replyForSecondPage != 0)
        {
            if(replyForSecondPage->isRunning())
            {
                replyForSecondPage->abort();
            }
        }
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
            replyForEditInfo = timeoutForEditInfo.resetReply(networkManager->get(requestForEditInfo));

            if(replyForEditInfo->isOpen() == true)
            {
                QObject::connect(replyForEditInfo, &QNetworkReply::finished, this, &showTopicMessagesClass::analyzeEditInfo);
            }
            else
            {
                analyzeEditInfo();
            }

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

    if(ajaxInfo.isEmpty() == false && replyForQuoteInfo == 0)
    {
        QNetworkRequest requestForQuoteInfo = parsingToolClass::buildRequestWithThisUrl("http://www.jeuxvideo.com/forums/ajax_citation.php");
        QString dataForQuote = "id_message=" + idOfMessageQuoted + "&" + ajaxInfo;
        replyForQuoteInfo = timeoutForQuoteInfo.resetReply(networkManager->post(requestForQuoteInfo, dataForQuote.toLatin1()));

        if(replyForQuoteInfo->isOpen() == true)
        {
            QObject::connect(replyForQuoteInfo, &QNetworkReply::finished, this, &showTopicMessagesClass::analyzeQuoteInfo);
        }
        else
        {
            analyzeQuoteInfo();
        }
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
    bool errorWhenTryToGetMessages = false;

    if(networkManager == 0)
    {
        itsNewManager = true;
        networkManager = new QNetworkAccessManager(this);
    }

    if(retrievesMessage == false)
    {
        if(replyForFirstPage == 0)
        {
            if(itsNewManager == true || needToSetCookies == true)
            {
                setNewCookies(currentCookieList, pseudoOfUser, false);
                needToSetCookies = false;
            }

            retrievesMessage = true;

            QString beforeLastPage = parsingToolClass::getBeforeLastPageOfTopic(topicLink);
            QNetworkRequest requestForFirstPage = parsingToolClass::buildRequestWithThisUrl(topicLink);
            setMessageStatus("Récupération des messages en cours...");
            secondPageLoading = false;
            linkHasChanged = false;
            replyForFirstPage = timeoutForFirstPage.resetReply(networkManager->get(requestForFirstPage));
            if(replyForFirstPage->isOpen() == true)
            {
                QObject::connect(replyForFirstPage, &QNetworkReply::finished, this, &showTopicMessagesClass::loadFirstPageFinish);
            }
            else
            {
                errorWhenTryToGetMessages = true;
            }

            if(loadTwoLastPage == true && beforeLastPage.isEmpty() == false)
            {
                QNetworkRequest requestForSecondPage = parsingToolClass::buildRequestWithThisUrl(beforeLastPage);
                secondPageLoading = true;
                replyForSecondPage = timeoutForSecondPage.resetReply(networkManager->get(requestForSecondPage));
                if(replyForSecondPage->isOpen() == true)
                {
                    QObject::connect(replyForSecondPage, &QNetworkReply::finished, this, &showTopicMessagesClass::loadSecondPageFinish);
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
                networkManager = 0;
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

    timeoutForEditInfo.resetReply();

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

    emit setEditInfo(oldIdOfLastMessageOfUser, message, dataToSend, oldUseMessageEdit);

    replyForEditInfo = 0;
}

void showTopicMessagesClass::analyzeQuoteInfo()
{
    QString messageQuote;
    QString source;

    timeoutForQuoteInfo.resetReply();

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
    bool appendHrAtEndOfFirstMessage = false;

    timeoutForFirstPage.resetReply();
    timeoutForSecondPage.resetReply();

    if(replyForFirstPage != 0)
    {
        if(replyForFirstPage->isReadable() && needToSetCookies == false)
        {
            bool cookiesChanged = false;
            QList<QNetworkCookie> newCookieList = qvariant_cast<QList<QNetworkCookie> >(replyForFirstPage->header(QNetworkRequest::SetCookieHeader));
            sourceFirst = replyForFirstPage->readAll();

            for(int i = 0; i < newCookieList.size(); ++i)
            {
                if(newCookieList.at(i).name() == "dlrowolleh" || newCookieList.at(i).name() == "coniunctio")
                {
                    for(int j = 0; j < currentCookieList.size(); ++j)
                    {
                        if(currentCookieList.at(j).name() == newCookieList.at(i).name())
                        {
                            if(currentCookieList.at(j).toRawForm() != newCookieList.at(i).toRawForm())
                            {
                                currentCookieList.replace(j, newCookieList.at(i));
                                cookiesChanged = true;
                            }
                            break;
                        }
                    }
                }
            }

            if(cookiesChanged == true)
            {
                emit newCookiesHaveToBeSet();
            }
        }
        replyForFirstPage->deleteLater();
        replyForFirstPage = 0;
    }

    if(loadTwoLastPage == true && secondPageLoading == true)
    {
        if(replyForSecondPage != 0)
        {
            if(replyForSecondPage->isReadable())
            {
                sourceSecond = replyForSecondPage->readAll();
            }
            replyForSecondPage->deleteLater();
            replyForSecondPage = 0;
        }
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

        if(getFirstMessageOfTopic == true)
        {
            QList<messageStruct> tmpList = parsingToolClass::getListOfEntireMessagesWithoutMessagePars(sourceFirst);

            if(tmpList.isEmpty() == false)
            {
                firstMessageOfTopic = tmpList.first();
            }
        }
    }

    if(firstTimeGetMessages == false || newTopicLink.isEmpty() == true)
    {
        QList<messageStruct> listOfEntireMessage;

        if(sourceSecond.isEmpty() == false)
        {
            listOfEntireMessage = parsingToolClass::getListOfEntireMessagesWithoutMessagePars(sourceSecond);
        }

        listOfEntireMessage.append(parsingToolClass::getListOfEntireMessagesWithoutMessagePars(sourceFirst));

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

            if(getFirstMessageOfTopic == true && firstMessageOfTopic.pseudoInfo.pseudoName.isEmpty() == false)
            {
                listOfEntireMessage.push_front(firstMessageOfTopic);
                appendHrAtEndOfFirstMessage = true;
                firstMessageOfTopic.pseudoInfo.pseudoName.clear();
            }
        }

        for(int i = 0; i < listOfEntireMessage.size(); ++i)
        {
            QMap<int, QString>::const_iterator listOfEditIterator = listOfEdit.find(listOfEntireMessage.at(i).idOfMessage);
            QString valueOfEditIte = listOfEntireMessage.at(i).lastTimeEdit;

            if(listOfEditIterator != listOfEdit.end())
            {
                valueOfEditIte = listOfEditIterator.value();
            }

            if((listOfEntireMessage.at(i).idOfMessage > idOfLastMessage || (valueOfEditIte != listOfEntireMessage.at(i).lastTimeEdit))
                    && listOfIgnoredPseudo->indexOf(listOfEntireMessage.at(i).pseudoInfo.pseudoName.toLower()) == -1)
            {
                QString newMessageToAppend = baseModel;
                buttonString.clear();
                colorOfPseudo.clear();
                colorOfPseudo = getColorOfThisPseudo(listOfEntireMessage.at(i).pseudoInfo.pseudoName.toLower());

                if(colorOfPseudo.isEmpty() == true)
                {
                    if(pseudoOfUser.toLower() == listOfEntireMessage.at(i).pseudoInfo.pseudoName.toLower())
                    {
                        colorOfPseudo = baseModelInfo.userPseudoColor;
                    }
                    else
                    {
                        if(colorModoAndAdminPseudo == true)
                        {
                            if(listOfEntireMessage.at(i).pseudoInfo.pseudoType == "user")
                            {
                                colorOfPseudo = baseModelInfo.normalPseudoColor;
                            }
                            else if(listOfEntireMessage.at(i).pseudoInfo.pseudoType == "modo")
                            {
                                colorOfPseudo = baseModelInfo.modoPseudoColor;
                            }
                            else if(listOfEntireMessage.at(i).pseudoInfo.pseudoType == "admin" || listOfEntireMessage.at(i).pseudoInfo.pseudoType == "staff")
                            {
                                colorOfPseudo = baseModelInfo.adminPseudoColor;
                            }
                            else
                            {
                                colorOfPseudo = baseModelInfo.normalPseudoColor;
                            }
                        }
                        else
                        {
                            colorOfPseudo = baseModelInfo.normalPseudoColor;
                        }
                    }
                }

                if(valueOfEditIte != listOfEntireMessage.at(i).lastTimeEdit)
                {
                    colorOfDate = baseModelInfo.editDateColor;
                }
                else
                {
                    if(colorPEMT == true && lastDate == listOfEntireMessage.at(i).date)
                    {
                        colorOfDate = baseModelInfo.pemtDateColor;
                    }
                    else
                    {
                        colorOfDate = baseModelInfo.normalDateColor;
                    }
                    idOfLastMessage = listOfEntireMessage.at(i).idOfMessage;
                    lastDate = listOfEntireMessage.at(i).date;

                    if(pseudoOfUser.toLower() == listOfEntireMessage.at(i).pseudoInfo.pseudoName.toLower())
                    {
                        idOfLastMessageOfUser = idOfLastMessage;
                    }
                }

                if(showQuoteButton == true)
                {
                    newMessageToAppend.replace("<%BUTTON_QUOTE%>", baseModelInfo.quoteModel);
                }

                if(pseudoOfUser.toLower() == listOfEntireMessage.at(i).pseudoInfo.pseudoName.toLower())
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
                newMessageToAppend.replace("<%PSEUDO_LOWER%>", listOfEntireMessage.at(i).pseudoInfo.pseudoName.toLower());
                newMessageToAppend.replace("<%ID_MESSAGE%>", QString::number(listOfEntireMessage.at(i).idOfMessage));
                newMessageToAppend.replace("<%DATE_MESSAGE%>", listOfEntireMessage.at(i).date);
                newMessageToAppend.replace("<%PSEUDO_COLOR%>", colorOfPseudo);
                newMessageToAppend.replace("<%PSEUDO_PSEUDO%>", listOfEntireMessage.at(i).pseudoInfo.pseudoName);
                newMessageToAppend.replace("<%MESSAGE_MESSAGE%>", parsingToolClass::parsingMessages(listOfEntireMessage.at(i).message, showStickers, stickersSize));

                if(appendHrAtEndOfFirstMessage == true)
                {
                    newMessageToAppend.append("<hr><span style=\"font-size: 1px;\"><br></span>");
                    appendHrAtEndOfFirstMessage = false;
                }

                messagesBox.append(newMessageToAppend);
                if(messagesBox.verticalScrollBar()->value() >= messagesBox.verticalScrollBar()->maximum())
                {
                    messagesBox.verticalScrollBar()->updateGeometry();
                    messagesBox.verticalScrollBar()->setValue(messagesBox.verticalScrollBar()->maximum());
                }
                listOfEdit[listOfEntireMessage.at(i).idOfMessage] = listOfEntireMessage.at(i).lastTimeEdit;
                if(pseudoOfUser.toLower() != listOfEntireMessage.at(i).pseudoInfo.pseudoName.toLower())
                {
                    if(warnWhenEdit == true || (warnWhenEdit == false && valueOfEditIte == listOfEntireMessage.at(i).lastTimeEdit))
                    {
                        emit newMessagesAvailable();
                    }
                }
            }
        }

        while(listOfEdit.size() > 40)
        {
            listOfEdit.erase(listOfEdit.begin());
        }
    }

    if(pseudoOfUser.isEmpty() == false && needToSetCookies == false)
    {
        ajaxInfo = parsingToolClass::getAjaxInfo(sourceFirst);
        listOfInput.clear();
        parsingToolClass::getListOfHiddenInputFromThisForm(sourceFirst, "form-post-topic", listOfInput);

        if(listOfInput.isEmpty() == true)
        {
            if(errorLastTime == true)
            {
                if(ignoreNetworkError == false)
                {
                    QMessageBox messageBox;
                    messageBox.warning(this, "Erreur sur " + topicName + " avec " + pseudoOfUser,
                                       "Le compte semble invalide, veuillez vous déconnecter de l'onglet puis vous y reconnecter (sans supprimer le compte de la liste des comptes).\n"
                                       "Si le problème persiste, redémarrez RespawnIRC ou supprimez le pseudo de la liste des comptes et ajoutez-le à nouveau.");
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
