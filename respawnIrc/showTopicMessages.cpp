#include <QCoreApplication>
#include <QVBoxLayout>
#include <QNetworkCookieJar>
#include <QMessageBox>
#include <QDesktopServices>
#include <QNetworkRequest>
#include <QScrollBar>
#include <QMetaObject>
#include <QMutableListIterator>
#include <QTextCursor>

#include "showTopicMessages.hpp"
#include "settingTool.hpp"
#include "configDependentVar.hpp"

QThread showTopicMessagesClass::threadForGetMessages;

showTopicMessagesClass::showTopicMessagesClass(QList<QString>* newListOfIgnoredPseudo, QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) : QWidget(parent)
{
    networkManager = new QNetworkAccessManager(this);
    getTopicMessages = new getTopicMessagesClass();
    getTopicMessages->moveToThread(&threadForGetMessages);

    expForColorPseudo.setPatternOptions(QRegularExpression::CaseInsensitiveOption | configDependentVar::regexpBaseOptions);
    messagesBox.setReadOnly(true);
    messagesBox.setOpenExternalLinks(false);
    messagesBox.setOpenLinks(false);
    messagesBox.setSearchPaths(QStringList(QCoreApplication::applicationDirPath()));

    updateSettingInfo();
    currentTypeOfEdit = realTypeOfEdit;
    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    listOfColorPseudo = newListOfColorPseudo;
    firstMessageOfTopic.isFirstMessage = false;

    setNewTheme(currentThemeName);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(&messagesBox);
    layout->setMargin(0);

    setLayout(layout);

    connect(&messagesBox, &QTextBrowser::anchorClicked, this, &showTopicMessagesClass::linkClicked);

    connect(getTopicMessages, &getTopicMessagesClass::newMessagesAreAvailable, this, &showTopicMessagesClass::analyzeMessages);
    connect(getTopicMessages, &getTopicMessagesClass::newMessageStatus, this, &showTopicMessagesClass::setMessageStatus);
    connect(getTopicMessages, &getTopicMessagesClass::newNumberOfConnectedAndMP, this, &showTopicMessagesClass::setNumberOfConnectedAndMP);
    connect(getTopicMessages, &getTopicMessagesClass::newNameForTopic, this, &showTopicMessagesClass::setTopicName);
    connect(getTopicMessages, &getTopicMessagesClass::newCookiesHaveToBeSet, this, &showTopicMessagesClass::setCookiesFromRequest);
    connect(getTopicMessages, &getTopicMessagesClass::theseStickersAreUsed, this, &showTopicMessagesClass::downloadTheseStickersIfNeeded);
    connect(getTopicMessages, &getTopicMessagesClass::theseNoelshackImagesAreUsed, this, &showTopicMessagesClass::downloadTheseNoelshackImagesIfNeeded);
    connect(getTopicMessages, &getTopicMessagesClass::newLinkForTopic, this, &showTopicMessagesClass::setUpdatedTopicLink);
}

showTopicMessagesClass::~showTopicMessagesClass()
{
    QMetaObject::invokeMethod(getTopicMessages, "deleteLater", Qt::QueuedConnection);
}

void showTopicMessagesClass::startThread()
{
    threadForGetMessages.start();
}

void showTopicMessagesClass::stopThread()
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

QString showTopicMessagesClass::getTopicLinkLastPage()
{
    return topicLinkLastPage;
}

QString showTopicMessagesClass::getTopicLinkFirstPage()
{
    return topicLinkFirstPage;
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

void showTopicMessagesClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newWebsiteOfCookies, QString newPseudoOfUser, bool updateMessages)
{
    currentCookieList = newCookies;
    websiteOfCookies = newWebsiteOfCookies;
    pseudoOfUser = newPseudoOfUser;
    expForColorPseudo.setPattern("");
    newPseudoOfUser.replace("[", "\\[").replace("]", "\\]");
    if(newPseudoOfUser.isEmpty() == false)
    {
        expForColorPseudo.setPattern(newPseudoOfUser + "(?![^<>]*(>|</a>))");
    }
    listOfInput.clear();

    if(networkManager != nullptr)
    {
        networkManager->clearAccessCache();
        networkManager->setCookieJar(new QNetworkCookieJar(this));
        networkManager->cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://" + websiteOfCookies));
        currentErrorStreak = 0;
    }

    QMetaObject::invokeMethod(getTopicMessages, "setNewCookies", Qt::QueuedConnection,
                              Q_ARG(QList<QNetworkCookie>, newCookies), Q_ARG(QString, websiteOfCookies), Q_ARG(QString, pseudoOfUser), Q_ARG(bool, updateMessages));
}

void showTopicMessagesClass::setTopicToErrorMode()
{
    if(errorMode == false)
    {
        errorMode = true;
        if(firstTimeGetMessages == true)
        {
            topicLinkLastPage.clear();
            topicName.clear();
            messagesBox.clear();
            firstMessageOfTopic.isFirstMessage = false;
            setMessageStatus("Erreur, topic invalide.");
            setNumberOfConnectedAndMP("", "", true);
            QMessageBox::warning(this, "Erreur", "Le topic n'existe pas.");
            QMetaObject::invokeMethod(getTopicMessages, "setNewTopic", Qt::QueuedConnection, Q_ARG(QString, ""), Q_ARG(bool, getFirstMessageOfTopic));
        }
        else
        {
            setMessageStatus("Erreur, impossible de récupérer les messages.");
            if(ignoreNetworkError == false)
            {
                QMessageBox::warning(this, "Erreur sur " + topicName, "Le programme n'a pas réussi à récupérer les messages cette fois ci, mais il continuera à essayer tant que l'onglet est ouvert.\n\n"
                                     "Vous pouvez désactiver ce message en cochant l'option \"Ignorer les erreurs réseau\" dans le menu Configuration > Préférences > Général > Avancé.");
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
    intSettingStruct updateTopicTimeSetting = settingToolClass::getThisIntOption("updateTopicTime");
    settingsForMessageParsingStruct settingsForMessageParsing;

    showQuoteButton = settingToolClass::getThisBoolOption("showQuoteButton");
    disableSelfQuoteButton = settingToolClass::getThisBoolOption("disableSelfQuoteButton");
    showBlacklistButton = settingToolClass::getThisBoolOption("showBlacklistButton");
    showEditButton = settingToolClass::getThisBoolOption("showEditButton");
    showDeleteButton = settingToolClass::getThisBoolOption("showDeleteButton");
    showSignatures = settingToolClass::getThisBoolOption("showSignatures");
    showAvatars = settingToolClass::getThisBoolOption("showAvatars");
    ignoreNetworkError = settingToolClass::getThisBoolOption("ignoreNetworkError");
    colorModoAndAdminPseudo = settingToolClass::getThisBoolOption("colorModoAndAdminPseudo");
    colorPEMT = settingToolClass::getThisBoolOption("colorPEMT");
    colorUserPseudoInMessages = settingToolClass::getThisBoolOption("colorUserPseudoInMessages");
    numberOfMessageShowedFirstTime = settingToolClass::getThisIntOption("numberOfMessageShowedFirstTime").value;
    getFirstMessageOfTopic = settingToolClass::getThisBoolOption("getFirstMessageOfTopic");
    warnWhenEdit = settingToolClass::getThisBoolOption("warnWhenEdit");
    numberOfErrorsBeforeWarning = settingToolClass::getThisIntOption("numberOfErrorsBeforeWarning").value;
    warnOnFirstTime = settingToolClass::getThisBoolOption("warnOnFirstTime");
    realTypeOfEdit = settingToolClass::getThisIntOption("typeOfEdit").value;

    timeoutForEditInfo.updateTimeoutTime();
    timeoutForQuoteInfo.updateTimeoutTime();
    timeoutForDeleteInfo.updateTimeoutTime();

    settingsForMessageParsing.loadTwoLastPage = settingToolClass::getThisBoolOption("loadTwoLastPage");
    settingsForMessageParsing.numberOfPagesToLoad = settingToolClass::getThisIntOption("numberOfPagesToLoad").value;
    settingsForMessageParsing.infoForMessageParsing.showStickers = settingToolClass::getThisBoolOption("showStickers");
    settingsForMessageParsing.infoForMessageParsing.stickersSize = settingToolClass::getThisIntOption("stickersSize").value;
    settingsForMessageParsing.timeoutTime = settingToolClass::getThisIntOption("timeoutInSecond").value;
    settingsForMessageParsing.infoForMessageParsing.nbMaxQuote = settingToolClass::getThisIntOption("maxNbOfQuotes").value;
    settingsForMessageParsing.infoForMessageParsing.stickerToSmiley = settingToolClass::getThisBoolOption("stickersToSmiley");
    settingsForMessageParsing.infoForMessageParsing.betterQuote = settingToolClass::getThisBoolOption("betterQuote");
    settingsForMessageParsing.downloadMissingStickers = settingToolClass::getThisBoolOption("downloadMissingStickers");
    settingsForMessageParsing.downloadNoelshackImages = settingToolClass::getThisBoolOption("downloadNoelshackImages");
    settingsForMessageParsing.infoForMessageParsing.noelshackImageWidth = settingToolClass::getThisIntOption("noelshackImageWidth").value;
    settingsForMessageParsing.infoForMessageParsing.noelshackImageHeight = settingToolClass::getThisIntOption("noelshackImageHeight").value;
    settingsForMessageParsing.timerTime = updateTopicTimeSetting.value;

    if(settingsForMessageParsing.timerTime < updateTopicTimeSetting.minValue)
    {
        settingsForMessageParsing.timerTime = updateTopicTimeSetting.minValue;
    }

    QMetaObject::invokeMethod(getTopicMessages, "settingsChanged", Qt::QueuedConnection, Q_ARG(settingsForMessageParsingStruct, settingsForMessageParsing));
}

void showTopicMessagesClass::addSearchPath(QString newSearchPath)
{
    QStringList currentSearchPaths = messagesBox.searchPaths();

    if(currentSearchPaths.indexOf(newSearchPath) == -1)
    {
        currentSearchPaths.append(newSearchPath);
        messagesBox.setSearchPaths(currentSearchPaths);
    }
}

void showTopicMessagesClass::relayoutDocumentHack()
{
    messagesBox.setLineWrapColumnOrWidth(messagesBox.lineWrapColumnOrWidth());
}

void showTopicMessagesClass::addMessageToTheEndOfMessagesBox(const QString& newMessage, long messageID)
{
    messageInfoForEditStruct newInfos;
    int baseSizeOfDocument = messagesBox.document()->characterCount();

    messagesBox.append(newMessage);

    if(currentTypeOfEdit > 0)
    {
        if(currentTypeOfEdit == 1)
        {
            newInfos.messageContent = newMessage;
        }

        newInfos.realPosition = messagesBox.document()->characterCount() - 1;
        newInfos.messageSize = newInfos.realPosition - baseSizeOfDocument;

        listOfInfosForEdit.push_back(QPair<long, messageInfoForEditStruct>(messageID, newInfos));
    }
}

void showTopicMessagesClass::editThisMessageOfMessagesBox(QString newMessage, long messageID)
{
    QMutableListIterator<QPair<long, messageInfoForEditStruct> > ite(listOfInfosForEdit);

    ite.toBack();

    while(ite.hasPrevious() == true)
    {
        if(ite.peekPrevious().first == messageID)
        {
            break;
        }
        ite.previous();
    }

    if(ite.hasPrevious() == false)
    {
        if(currentTypeOfEdit != 1)
        {
            addMessageToTheEndOfMessagesBox(newMessage, messageID);
        }
    }
    else
    {
        int baseSizeOfDocument = messagesBox.document()->characterCount();
        int newSizeOfDocument;
        QTextCursor currentCurs = messagesBox.textCursor();

        if(currentTypeOfEdit == 1)
        {
            newMessage.replace("<%MESSAGE_TO_UPDATE%>", ite.peekPrevious().second.messageContent);
        }

        currentCurs.setPosition(ite.peekPrevious().second.realPosition);
        currentCurs.setPosition(ite.peekPrevious().second.realPosition - ite.peekPrevious().second.messageSize, QTextCursor::KeepAnchor);
        currentCurs.insertHtml(newMessage);

        newSizeOfDocument = messagesBox.document()->characterCount() - baseSizeOfDocument;
        ite.peekPrevious().second.messageSize += newSizeOfDocument;

        ite.previous();
        while(ite.hasNext())
        {
            ite.peekNext().second.realPosition += newSizeOfDocument;
            ite.next();
        }
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
    lastDate.clear();
    listOfInfosForEdit.clear();
    currentTypeOfEdit = realTypeOfEdit;
    firstMessageOfTopic.isFirstMessage = false;
    topicLinkLastPage = newTopic;
    topicLinkFirstPage = parsingToolClass::getFirstPageOfTopic(newTopic);
    websiteOfTopic = parsingToolClass::getWebsite(topicLinkLastPage);
    firstTimeGetMessages = true;
    errorMode = false;
    currentErrorStreak = 0;
    idOfLastMessageOfUser = 0;
    oldIdOfLastMessageOfUser = 0;
    needToGetMessages = false;
    oldUseMessageEdit = false;
    QMetaObject::invokeMethod(getTopicMessages, "setNewTopic", Qt::QueuedConnection, Q_ARG(QString, newTopic), Q_ARG(bool, getFirstMessageOfTopic));
}

void showTopicMessagesClass::linkClicked(const QUrl& link)
{
    QString linkInString = link.toDisplayString();

    if(linkInString.startsWith("quote"))
    {
        QString messageQuoted;
        linkInString.remove(0, linkInString.indexOf(':') + 1);
        messageQuoted = linkInString.mid(linkInString.indexOf(':') + 1);
        getQuoteInfo(linkInString.left(linkInString.indexOf(':')), messageQuoted);
    }
    else if(linkInString.startsWith("blacklist"))
    {
        emit addToBlacklist(linkInString.remove(0, linkInString.indexOf(':') + 1));
    }
    else if(linkInString.startsWith("edit"))
    {
        emit editThisMessage(linkInString.remove(0, linkInString.indexOf(':') + 1).toLong(), true);
    }
    else if(linkInString.startsWith("delete"))
    {
        linkInString.remove(0, linkInString.indexOf(':') + 1);
        deleteMessage(linkInString);
    }
    else
    {
        QDesktopServices::openUrl(link);
    }
}

bool showTopicMessagesClass::getEditInfo(long idOfMessageToEdit, bool useMessageEdit)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, websiteOfCookies, pseudoOfUser, false);
    }

    if(ajaxInfo.list.isEmpty() == false && pseudoOfUser.isEmpty() == false && idOfLastMessageOfUser != 0)
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

            urlToGet = "http://" + websiteOfTopic + "/forums/ajax_edit_message.php?id_message=" + QString::number(oldIdOfLastMessageOfUser) + "&" + ajaxInfo.list + "&action=get";
            requestForEditInfo = parsingToolClass::buildRequestWithThisUrl(urlToGet);
            oldAjaxInfo = ajaxInfo;
            ajaxInfo.list.clear();
            oldUseMessageEdit = useMessageEdit;
            replyForEditInfo = timeoutForEditInfo.resetReply(networkManager->get(requestForEditInfo));

            if(replyForEditInfo->isOpen() == true)
            {
                connect(replyForEditInfo, &QNetworkReply::finished, this, &showTopicMessagesClass::analyzeEditInfo);
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

void showTopicMessagesClass::getQuoteInfo(QString idOfMessageQuoted, QString messageQuoted)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, websiteOfCookies, pseudoOfUser, false);
    }

    if(ajaxInfo.list.isEmpty() == false && replyForQuoteInfo == nullptr)
    {
        QNetworkRequest requestForQuoteInfo = parsingToolClass::buildRequestWithThisUrl("http://" + websiteOfTopic + "/forums/ajax_citation.php");
        QString dataForQuote = "id_message=" + idOfMessageQuoted + "&" + ajaxInfo.list;
        lastMessageQuoted = messageQuoted;
        replyForQuoteInfo = timeoutForQuoteInfo.resetReply(networkManager->post(requestForQuoteInfo, dataForQuote.toLatin1()));

        if(replyForQuoteInfo->isOpen() == true)
        {
            connect(replyForQuoteInfo, &QNetworkReply::finished, this, &showTopicMessagesClass::analyzeQuoteInfo);
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
        QMessageBox::warning(this, "Erreur", "Erreur, impossible de citer ce message, réessayez.");
    }
}

void showTopicMessagesClass::deleteMessage(QString idOfMessageDeleted)
{
    if(networkManager == nullptr)
    {
        networkManager = new QNetworkAccessManager(this);
        setNewCookies(currentCookieList, websiteOfCookies, pseudoOfUser, false);
    }

    if(ajaxInfo.mod.isEmpty() == false && replyForDeleteInfo == nullptr)
    {
        QNetworkRequest requestForDeleteInfo = parsingToolClass::buildRequestWithThisUrl("http://" + websiteOfTopic + "/forums/modal_del_message.php?tab_message[]=" + idOfMessageDeleted + "&type=delete&" + ajaxInfo.mod);
        replyForDeleteInfo = timeoutForDeleteInfo.resetReply(networkManager->get(requestForDeleteInfo));

        if(replyForDeleteInfo->isOpen() == true)
        {
            connect(replyForDeleteInfo, &QNetworkReply::finished, this, &showTopicMessagesClass::analyzeDeleteInfo);
        }
        else
        {
            analyzeDeleteInfo();
            networkManager->deleteLater();
            networkManager = nullptr;
        }
    }
    else
    {
        QMessageBox::warning(this, "Erreur", "Erreur, impossible de supprimer ce message, réessayez.");
    }
}

void showTopicMessagesClass::analyzeEditInfo()
{
    QString message;
    QString dataToSend = oldAjaxInfo.list + "&action=post";
    QList<QPair<QString, QString> > listOfEditInput;
    QString source;

    timeoutForEditInfo.resetReply();

    if(replyForEditInfo->isReadable())
    {
        source = replyForEditInfo->readAll();
    }
    replyForEditInfo->deleteLater();

    message = parsingToolClass::getMessageEditAndChangeSource(source);
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

    messageQuote = ">" + QUrl::fromPercentEncoding(lastMessageQuoted.toUtf8()) + "\n>" + messageQuote;
    replyForQuoteInfo = nullptr;

    emit quoteThisMessage(messageQuote);
}

void showTopicMessagesClass::analyzeDeleteInfo()
{
    QString source;

    timeoutForDeleteInfo.resetReply();

    if(replyForDeleteInfo->isReadable())
    {
        source = replyForDeleteInfo->readAll();
    }
    replyForDeleteInfo->deleteLater();

    replyForDeleteInfo = nullptr;

    if(source.startsWith("{\"erreur\":[]}") == false)
    {
        source.remove(0, source.indexOf("[") + 2);
        source.remove(source.lastIndexOf("]") - 1, 3);
        QMessageBox::warning(this, "Erreur", source);
    }
}

void showTopicMessagesClass::analyzeMessages(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString> > newListOfInput,
                                             ajaxInfoStruct newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty)
{
    QString colorOfPseudo;
    QString colorOfDate;
    QStringList listOfAvatarsUsed;
    bool appendHrAtEndOfFirstMessage = false;
    bool errorHappen = false;
    bool firstTimeAddMessages = false;
    bool needToScrollDown = messagesBox.verticalScrollBar()->value() >= messagesBox.verticalScrollBar()->maximum();

    if(parsingToolClass::getFirstPageOfTopic(fromThisTopic) != topicLinkFirstPage)
    {
        startGetMessage();
        return;
    }

    topicLinkLastPage = fromThisTopic;

    if(listOfNewMessages.isEmpty() == true && listIsReallyEmpty == true)
    {
        if(currentErrorStreak >= (numberOfErrorsBeforeWarning - 1))
        {
            setTopicToErrorMode();
        }
        else
        {
            ++currentErrorStreak;
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

        firstTimeAddMessages = true;
    }

    for(messageStruct& currentMessage : listOfNewMessages)
    {
        QString newMessageToAppend = baseModel;
        colorOfPseudo = getColorOfThisPseudo(currentMessage.pseudoInfo.pseudoName.toLower());

        if(listOfIgnoredPseudo->indexOf(currentMessage.pseudoInfo.pseudoName.toLower()) != -1)
        {
            appendHrAtEndOfFirstMessage = false;
            continue;
        }

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
            if(colorPEMT == true && lastDate == currentMessage.wholeDate)
            {
                colorOfDate = baseModelInfo.pemtDateColor;
            }
            else
            {
                colorOfDate = baseModelInfo.normalDateColor;
            }

            if(currentMessage.isFirstMessage == false)
            {
                lastDate = currentMessage.wholeDate;
            }

            if(pseudoOfUser.toLower() == currentMessage.pseudoInfo.pseudoName.toLower())
            {
                idOfLastMessageOfUser = currentMessage.idOfMessage;
            }
        }

        if(showQuoteButton == true && (disableSelfQuoteButton == false || pseudoOfUser.toLower() != currentMessage.pseudoInfo.pseudoName.toLower()))
        {
            newMessageToAppend.replace("<%BUTTON_QUOTE%>", baseModelInfo.quoteModel);
        }

        if(pseudoOfUser.toLower() == currentMessage.pseudoInfo.pseudoName.toLower())
        {
            if(showEditButton == true)
            {
                newMessageToAppend.replace("<%BUTTON_EDIT%>", baseModelInfo.editModel);
            }
            if(showDeleteButton == true)
            {
                newMessageToAppend.replace("<%BUTTON_DELETE%>", baseModelInfo.deleteModel);
            }
        }
        else if(showBlacklistButton == true)
        {
            newMessageToAppend.replace("<%BUTTON_BLACKLIST%>", baseModelInfo.blacklistModel);
        }

        if(showSignatures == true && currentMessage.signature.isEmpty() == false)
        {
            newMessageToAppend.replace("<%SIGNATURE_MODEL%>", baseModelInfo.signatureModel);
        }

        if(showAvatars == true && currentMessage.avatarLink.isEmpty() == false)
        {
            newMessageToAppend.replace("<%AVATAR_MODEL%>", baseModelInfo.avatarModel);
        }

        if(colorUserPseudoInMessages == true && pseudoOfUser.isEmpty() == false)
        {
            parsingToolClass::replaceWithCapNumber(currentMessage.message, expForColorPseudo, 0,
                                                   "<span style=\"color: " + baseModelInfo.userPseudoColor + ";\">", "</span>");
        }

        newMessageToAppend.replace("<%DATE_COLOR%>", colorOfDate);
        newMessageToAppend.replace("<%PSEUDO_LOWER%>", currentMessage.pseudoInfo.pseudoName.toLower());
        newMessageToAppend.replace("<%ID_MESSAGE%>", QString::number(currentMessage.idOfMessage));
        newMessageToAppend.replace("<%DATE_MESSAGE%>", currentMessage.date);
        newMessageToAppend.replace("<%DATE_STRING%>", currentMessage.wholeDate);
        newMessageToAppend.replace("<%PSEUDO_COLOR%>", colorOfPseudo);
        newMessageToAppend.replace("<%PSEUDO_PSEUDO%>", currentMessage.pseudoInfo.pseudoName);
        newMessageToAppend.replace("<%MESSAGE_MESSAGE%>", currentMessage.message);

        if(showSignatures == true && currentMessage.signature.isEmpty() == false)
        {
            newMessageToAppend.replace("<%SIGNATURE_SIGNATURE%>", currentMessage.signature);
        }

        if(showAvatars == true && currentMessage.avatarLink.isEmpty() == false)
        {
            newMessageToAppend.replace("<%AVATAR_LINK%>", "vtr/" + currentMessage.avatarLink);
            listOfAvatarsUsed.append(currentMessage.avatarLink);
        }

        if(appendHrAtEndOfFirstMessage == true)
        {
            newMessageToAppend.append("<hr><span style=\"font-size: 1px;\"><br></span>");
            appendHrAtEndOfFirstMessage = false;
        }

        if(currentMessage.isAnEdit == false || currentTypeOfEdit <= 0)
        {
            addMessageToTheEndOfMessagesBox(newMessageToAppend, currentMessage.idOfMessage);
        }
        else
        {
            if(currentTypeOfEdit == 1)
            {
                editThisMessageOfMessagesBox(baseModelInfo.updateMessageForEditModel, currentMessage.idOfMessage);
                addMessageToTheEndOfMessagesBox(newMessageToAppend, currentMessage.idOfMessage);
            }
            else
            {
                editThisMessageOfMessagesBox(newMessageToAppend, currentMessage.idOfMessage);
            }
        }

        if(pseudoOfUser.toLower() != currentMessage.pseudoInfo.pseudoName.toLower() && (warnOnFirstTime == true || firstTimeAddMessages == false))
        {
            if(warnWhenEdit == true || currentMessage.isAnEdit == false)
            {
                emit newMessagesAvailable();
            }
        }
    }

    if(needToScrollDown == true)
    {
        messagesBox.verticalScrollBar()->updateGeometry();
        messagesBox.verticalScrollBar()->setValue(messagesBox.verticalScrollBar()->maximum());
    }

    while(listOfInfosForEdit.size() > 40)
    {
        listOfInfosForEdit.pop_front();
    }

    if(pseudoOfUser.isEmpty() == false)
    {
        ajaxInfo = newAjaxInfo;
        listOfInput = newListOfInput;

        if(listOfInput.isEmpty() == true)
        {
            if(currentErrorStreak >= (numberOfErrorsBeforeWarning - 1))
            {
                if(ignoreNetworkError == false)
                {
                    QString oldPseudo = pseudoOfUser;
                    setNewCookies(QList<QNetworkCookie>(), websiteOfCookies, "");
                    QMessageBox::warning(this, "Erreur sur " + topicName + " avec " + oldPseudo,
                                       "Le compte semble invalide, veuillez vous déconnecter de l'onglet puis vous y reconnecter (sans supprimer le compte de la liste des comptes).\n"
                                       "Si le problème persiste, redémarrez RespawnIRC ou supprimez le pseudo de la liste des comptes et ajoutez-le à nouveau.\n\n"
                                       "Vous pouvez désactiver ce message en cochant l'option \"Ignorer les erreurs réseau\" dans le menu Configuration > Préférences > Général > Avancé.");
                }
            }
            else
            {
                ++currentErrorStreak;
            }

            errorHappen = true;
        }
    }


    if(errorHappen == false)
    {
        currentErrorStreak = 0;
    }

    firstTimeGetMessages = false;

    emit downloadTheseAvatarsIfNeeded(listOfAvatarsUsed);
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
    if(newTopicName.size() >= (topicNameMaxSize + 3))
    {
        topicName = newTopicName.left(topicNameMaxSize) + "...";
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
        setNewCookies(newListOfCookies, websiteOfCookies, pseudoOfUser, false);
        emit newCookiesHaveToBeSet();
    }
}

void showTopicMessagesClass::setUpdatedTopicLink(QString newTopicLink)
{
    topicLinkFirstPage = parsingToolClass::getFirstPageOfTopic(newTopicLink);
}
