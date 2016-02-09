#include "showTopicMessages.hpp"
#include "settingTool.hpp"

showTopicMessagesClass::showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) : QWidget(parent)
{
    networkManager = new QNetworkAccessManager(this);
    getTopicMessages = new getTopicMessagesClass();
    getTopicMessages->moveToThread(&threadForGetMessages);

    expForColorPseudo.setPatternOptions(QRegularExpression::CaseInsensitiveOption | QRegularExpression::OptimizeOnFirstUsageOption);
    messagesBox.setReadOnly(true);
    messagesBox.setOpenExternalLinks(false);
    messagesBox.setOpenLinks(false);
    messagesBox.setSearchPaths(QStringList(QCoreApplication::applicationDirPath()));

    updateSettingInfo();
    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    listOfColorPseudo = newListOfColorPseudo;
    firstMessageOfTopic.isFirstMessage = false;

    setNewTheme(currentThemeName);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&messagesBox);
    layout->setMargin(0);

    setLayout(layout);

    QObject::connect(&messagesBox, &QTextBrowser::anchorClicked, this, &showTopicMessagesClass::linkClicked);

    QObject::connect(&threadForGetMessages, &QThread::finished, getTopicMessages, &QObject::deleteLater);
    QObject::connect(getTopicMessages, &getTopicMessagesClass::newMessagesAreAvailable, this, &showTopicMessagesClass::analyzeMessages);
    QObject::connect(getTopicMessages, &getTopicMessagesClass::newMessageStatus, this, &showTopicMessagesClass::setMessageStatus);
    QObject::connect(getTopicMessages, &getTopicMessagesClass::newNumberOfConnectedAndMP, this, &showTopicMessagesClass::setNumberOfConnectedAndMP);
    QObject::connect(getTopicMessages, &getTopicMessagesClass::newNameForTopic, this, &showTopicMessagesClass::setTopicName);
    QObject::connect(getTopicMessages, &getTopicMessagesClass::newCookiesHaveToBeSet, this, &showTopicMessagesClass::setCookiesFromRequest);

    threadForGetMessages.start();
}

showTopicMessagesClass::~showTopicMessagesClass()
{
    threadForGetMessages.quit();
    threadForGetMessages.wait();
}

void showTopicMessagesClass::startGetMessage()
{
    QMetaObject::invokeMethod(getTopicMessages, "startGetMessage", Qt::QueuedConnection);
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
    for(const pseudoWithColorStruct& thisColor : *listOfColorPseudo)
    {
        if(thisColor.pseudo == pseudo)
        {
            return "rgb(" + QString::number(thisColor.red) + ", " + QString::number(thisColor.green) + ", " + QString::number(thisColor.blue) + ")";
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
    newPseudoOfUser.replace("[", "\\[").replace("]", "\\]");
    expForColorPseudo.setPattern(newPseudoOfUser + "(?![^<]*</a>)");
    listOfInput.clear();

    if(networkManager != nullptr)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
        errorLastTime = false;
    }

    QMetaObject::invokeMethod(getTopicMessages, "setNewCookies", Qt::QueuedConnection,
                              Q_ARG(QList<QNetworkCookie>, newCookies), Q_ARG(QString, pseudoOfUser), Q_ARG(bool, updateMessages));
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
            messagesBox.clear();
            firstMessageOfTopic.isFirstMessage = false;
            setMessageStatus("Erreur, topic invalide.");
            setNumberOfConnectedAndMP("", "", true);
            messageBox.warning(this, "Erreur", "Le topic n'existe pas.");
            QMetaObject::invokeMethod(getTopicMessages, "setNewTopic", Qt::QueuedConnection, Q_ARG(QString, topicLink), Q_ARG(bool, getFirstMessageOfTopic));
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
}

void showTopicMessagesClass::updateSettingInfo()
{
    showQuoteButton = settingToolClass::getThisBoolOption("showQuoteButton");
    showBlacklistButton = settingToolClass::getThisBoolOption("showBlacklistButton");
    showEditButton = settingToolClass::getThisBoolOption("showEditButton");
    ignoreNetworkError = settingToolClass::getThisBoolOption("ignoreNetworkError");
    colorModoAndAdminPseudo = settingToolClass::getThisBoolOption("colorModoAndAdminPseudo");
    colorPEMT = settingToolClass::getThisBoolOption("colorPEMT");
    colorUserPseudoInMessages = settingToolClass::getThisBoolOption("colorUserPseudoInMessages");
    numberOfMessageShowedFirstTime = settingToolClass::getThisIntOption("numberOfMessageShowedFirstTime");
    getFirstMessageOfTopic = settingToolClass::getThisBoolOption("getFirstMessageOfTopic");
    warnWhenEdit = settingToolClass::getThisBoolOption("warnWhenEdit");

    QMetaObject::invokeMethod(getTopicMessages, "settingsChanged", Qt::QueuedConnection,
                              Q_ARG(bool, settingToolClass::getThisBoolOption("loadTwoLastPage")),
                              Q_ARG(int, settingToolClass::getThisIntOption("updateTopicTime")),
                              Q_ARG(bool, settingToolClass::getThisBoolOption("showStickers")),
                              Q_ARG(int, settingToolClass::getThisIntOption("stickersSize")));
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
    lastDate.clear();
    firstMessageOfTopic.isFirstMessage = false;
    topicLink = parsingToolClass::getFirstPageOfTopic(newTopic);
    firstTimeGetMessages = true;
    errorMode = false;
    errorLastTime = false;
    idOfLastMessageOfUser = 0;
    oldIdOfLastMessageOfUser = 0;
    needToGetMessages = false;
    oldUseMessageEdit = false;
    QMetaObject::invokeMethod(getTopicMessages, "setNewTopic", Qt::QueuedConnection, Q_ARG(QString, newTopic), Q_ARG(bool, getFirstMessageOfTopic));
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
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, pseudoOfUser, false);
    }

    if(ajaxInfo.isEmpty() == false && pseudoOfUser.isEmpty() == false && idOfLastMessageOfUser != 0)
    {
        if(replyForEditInfo == nullptr)
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
                networkManager->deleteLater();
                networkManager = nullptr;
            }

            return true;
        }
    }

    return false;
}

void showTopicMessagesClass::getQuoteInfo(QString idOfMessageQuoted)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, pseudoOfUser, false);
    }

    if(ajaxInfo.isEmpty() == false && replyForQuoteInfo == nullptr)
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
            networkManager->deleteLater();
            networkManager = nullptr;
        }
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Erreur, impossible de citer ce message, réessayez.");
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

    for(const QPair<QString, QString>& thisInput : listOfEditInput)
    {
        dataToSend += "&" + thisInput.first + "=" + thisInput.second;
    }

    emit setEditInfo(oldIdOfLastMessageOfUser, message, dataToSend, oldUseMessageEdit);

    replyForEditInfo = nullptr;
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
    replyForQuoteInfo = nullptr;

    emit quoteThisMessage(messageQuote);
}

void showTopicMessagesClass::analyzeMessages(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString> > newListOfInput,
                                             QString newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty)
{
    QString colorOfPseudo;
    QString colorOfDate;
    bool appendHrAtEndOfFirstMessage = false;

    if(parsingToolClass::getFirstPageOfTopic(fromThisTopic) != topicLink)
    {
        return;
    }

    if(listOfNewMessages.isEmpty() == true && listIsReallyEmpty == true)
    {
        if(errorLastTime == true)
        {
            setTopicToErrorMode();
        }
        else
        {
            errorLastTime = true;
        }
        return;
    }
    else
    {
        errorMode = false;
    }

    if(firstTimeGetMessages == true)
    {
        if(listOfNewMessages.isEmpty() == false)
        {
            if(getFirstMessageOfTopic == true && listOfNewMessages.first().isFirstMessage == true)
            {
                firstMessageOfTopic = listOfNewMessages.first();
                listOfNewMessages.pop_front();
            }
        }
    }

    if(messagesBox.toPlainText().isEmpty() == true)
    {
        while(listOfNewMessages.size() > numberOfMessageShowedFirstTime)
        {
            listOfNewMessages.pop_front();
        }

        if(listOfNewMessages.isEmpty() == false && getFirstMessageOfTopic == true && firstMessageOfTopic.isFirstMessage == true)
        {
            listOfNewMessages.push_front(firstMessageOfTopic);
            firstMessageOfTopic.isFirstMessage = false;
            appendHrAtEndOfFirstMessage = true;
        }
    }

    for(messageStruct& currentMessage : listOfNewMessages)
    {
        QString newMessageToAppend = baseModel;
        colorOfPseudo = getColorOfThisPseudo(currentMessage.pseudoInfo.pseudoName.toLower());

        if(colorOfPseudo.isEmpty() == true)
        {
            if(pseudoOfUser.toLower() == currentMessage.pseudoInfo.pseudoName.toLower())
            {
                colorOfPseudo = baseModelInfo.userPseudoColor;
            }
            else
            {
                if(colorModoAndAdminPseudo == true)
                {
                    if(currentMessage.pseudoInfo.pseudoType == "user")
                    {
                        colorOfPseudo = baseModelInfo.normalPseudoColor;
                    }
                    else if(currentMessage.pseudoInfo.pseudoType == "modo")
                    {
                        colorOfPseudo = baseModelInfo.modoPseudoColor;
                    }
                    else if(currentMessage.pseudoInfo.pseudoType == "admin" || currentMessage.pseudoInfo.pseudoType == "staff")
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

        if(currentMessage.isAnEdit == true)
        {
            colorOfDate = baseModelInfo.editDateColor;
        }
        else
        {
            if(colorPEMT == true && lastDate == currentMessage.date)
            {
                colorOfDate = baseModelInfo.pemtDateColor;
            }
            else
            {
                colorOfDate = baseModelInfo.normalDateColor;
            }

            if(currentMessage.isFirstMessage == false)
            {
                lastDate = currentMessage.date;
            }

            if(pseudoOfUser.toLower() == currentMessage.pseudoInfo.pseudoName.toLower())
            {
                idOfLastMessageOfUser = currentMessage.idOfMessage;
            }
        }

        if(showQuoteButton == true)
        {
            newMessageToAppend.replace("<%BUTTON_QUOTE%>", baseModelInfo.quoteModel);
        }

        if(pseudoOfUser.toLower() == currentMessage.pseudoInfo.pseudoName.toLower())
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

        if(colorUserPseudoInMessages == true)
        {
            parsingToolClass::replaceWithCapNumber(currentMessage.message, expForColorPseudo, 0,
                                                   "<span style=\"color: " + baseModelInfo.userPseudoColor + ";\">", "</span>");
        }

        newMessageToAppend.replace("<%DATE_COLOR%>", colorOfDate);
        newMessageToAppend.replace("<%PSEUDO_LOWER%>", currentMessage.pseudoInfo.pseudoName.toLower());
        newMessageToAppend.replace("<%ID_MESSAGE%>", QString::number(currentMessage.idOfMessage));
        newMessageToAppend.replace("<%DATE_MESSAGE%>", currentMessage.date);
        newMessageToAppend.replace("<%PSEUDO_COLOR%>", colorOfPseudo);
        newMessageToAppend.replace("<%PSEUDO_PSEUDO%>", currentMessage.pseudoInfo.pseudoName);
        newMessageToAppend.replace("<%MESSAGE_MESSAGE%>", currentMessage.message);

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

        if(pseudoOfUser.toLower() != currentMessage.pseudoInfo.pseudoName.toLower())
        {
            if(warnWhenEdit == true || (warnWhenEdit == false && currentMessage.isAnEdit == false))
            {
                emit newMessagesAvailable();
            }
        }
    }

    if(pseudoOfUser.isEmpty() == false)
    {
        ajaxInfo = newAjaxInfo;
        listOfInput = newListOfInput;

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
                    setNumberOfConnectedAndMP(numberOfConnected, "");
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
}

void showTopicMessagesClass::setMessageStatus(QString newStatus)
{
    messagesStatus = newStatus;
    emit newMessageStatus();
}

void showTopicMessagesClass::setNumberOfConnectedAndMP(QString newNumberConnected, QString newNumberMP, bool forceSet)
{
    QString newMessageToShow = newNumberConnected;

    if(newNumberMP.isEmpty() == false)
    {
        newMessageToShow += " - " + newNumberMP;
    }

    numberOfConnected = newNumberConnected;

    if(newMessageToShow.isEmpty() == false || forceSet == true)
    {
        if(newMessageToShow != numberOfConnectedAndMP)
        {
            numberOfConnectedAndMP = newMessageToShow;
            emit newNumberOfConnectedAndMP();
        }
    }
}

void showTopicMessagesClass::setTopicName(QString newTopicName)
{
    if(newTopicName.size() >= 48)
    {
        topicName = newTopicName.left(45) + "...";
    }
    else
    {
        topicName = newTopicName;
    }

    emit newNameForTopic(topicName);
}

void showTopicMessagesClass::setCookiesFromRequest(QList<QNetworkCookie> newListOfCookies, QString currentPseudoOfUser)
{
    if(currentPseudoOfUser == pseudoOfUser)
    {
        setNewCookies(newListOfCookies, pseudoOfUser, false);
        emit newCookiesHaveToBeSet();
    }
}
