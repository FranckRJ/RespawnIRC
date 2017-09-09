#include <QCoreApplication>
#include <QVBoxLayout>
#include <QNetworkCookieJar>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QScrollBar>
#include <QMetaObject>
#include <QMutableListIterator>
#include <QTextCursor>
#include <QMenu>
#include <QAction>

#include "showTopic.hpp"
#include "settingTool.hpp"
#include "utilityTool.hpp"
#include "configDependentVar.hpp"

QThread* showTopicClass::threadForGetMessages;

showTopicClass::showTopicClass(const QList<QString>* newListOfIgnoredPseudo, const QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent) : QWidget(parent)
{
    getTopicMessages = new getTopicMessagesClass();
    messageActions = new messageActionsClass(this);
    messagesBox = new QTextBrowser(this);
    getTopicMessages->moveToThread(threadForGetMessages);

    expForColorPseudo.setPatternOptions(QRegularExpression::CaseInsensitiveOption | configDependentVar::regexpBaseOptions);
    messagesBox->setReadOnly(true);
    messagesBox->setOpenExternalLinks(false);
    messagesBox->setOpenLinks(false);
    messagesBox->setContextMenuPolicy(Qt::CustomContextMenu);
    resetSearchPath();

    updateSettingInfo();
    currentTypeOfEdit = realTypeOfEdit;
    listOfIgnoredPseudo = newListOfIgnoredPseudo;
    listOfColorPseudo = newListOfColorPseudo;
    firstMessageOfTopic.isFirstMessage = false;

    setNewTheme(currentThemeName);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(messagesBox);
    layout->setMargin(0);

    setLayout(layout);

    connect(messagesBox, &QTextBrowser::anchorClicked, this, &showTopicClass::linkClicked);
    connect(messagesBox, &QTextBrowser::customContextMenuRequested, this, &showTopicClass::createContextMenu);

    connect(getTopicMessages, &getTopicMessagesClass::newMessagesAreAvailable, this, &showTopicClass::analyzeMessages);
    connect(getTopicMessages, &getTopicMessagesClass::newMessageStatus, this, &showTopicClass::setMessageStatus);
    connect(getTopicMessages, &getTopicMessagesClass::newNumberOfConnectedAndMP, this, &showTopicClass::setNumberOfConnectedAndMP);
    connect(getTopicMessages, &getTopicMessagesClass::newNameForTopic, this, &showTopicClass::setTopicName);
    connect(getTopicMessages, &getTopicMessagesClass::newCookieHasToBeSet, this, &showTopicClass::setCookieFromRequest);
    connect(getTopicMessages, &getTopicMessagesClass::theseStickersAreUsed, this, &showTopicClass::downloadTheseStickersIfNeeded);
    connect(getTopicMessages, &getTopicMessagesClass::theseNoelshackImagesAreUsed, this, &showTopicClass::downloadTheseNoelshackImagesIfNeeded);
    connect(getTopicMessages, &getTopicMessagesClass::newLinkForTopic, this, &showTopicClass::setUpdatedTopicLink);
    connect(messageActions, &messageActionsClass::quoteThisMessage, this, &showTopicClass::quoteThisMessage);
    connect(messageActions, &messageActionsClass::setEditInfo, this, &showTopicClass::setEditInfo);
}

showTopicClass::~showTopicClass()
{
    QMetaObject::invokeMethod(getTopicMessages, "deleteLater", Qt::QueuedConnection);
}

void showTopicClass::createThread(QObject* parentForThread)
{
    threadForGetMessages = new QThread(parentForThread);
}

void showTopicClass::startThread()
{
    threadForGetMessages->start();
}

void showTopicClass::stopThread()
{
    threadForGetMessages->quit();
    threadForGetMessages->wait();
}

void showTopicClass::startGetMessage()
{
    QMetaObject::invokeMethod(getTopicMessages, "startGetMessage", Qt::QueuedConnection);
}

const QList<QPair<QString, QString>>& showTopicClass::getListOfInput() const
{
    return listOfInput;
}

QString showTopicClass::getTopicLinkLastPage() const
{
    return topicLinkLastPage;
}

QString showTopicClass::getTopicLinkFirstPage() const
{
    return topicLinkFirstPage;
}

QString showTopicClass::getTopicName() const
{
    return topicName;
}

QString showTopicClass::getMessagesStatus() const
{
    return messagesStatus;
}

QString showTopicClass::getNumberOfConnectedAndMP() const
{
    return numberOfConnectedAndMP;
}

QString showTopicClass::getPseudoUsed() const
{
    return pseudoOfUser;
}

const QNetworkCookie& showTopicClass::getConnectCookie() const
{
    return messageActions->getConnectCookie();
}

bool showTopicClass::getEditInfo(long idOfMessageToEdit, bool useMessageEdit)
{
    if(pseudoOfUser.isEmpty() == false && idOfLastMessageOfUser != 0)
    {
        long idOfMessageToUse;

        if(idOfMessageToEdit == 0)
        {
            idOfMessageToUse = idOfLastMessageOfUser;
        }
        else
        {
            idOfMessageToUse = idOfMessageToEdit;
        }

        return messageActions->getEditInfo(idOfMessageToUse, useMessageEdit);
    }

    return false;
}

void showTopicClass::setNewCookie(QNetworkCookie newConnectCookie, QString newWebsiteOfCookie, QString newPseudoOfUser, bool updateMessages)
{
    websiteOfCookie = newWebsiteOfCookie;
    pseudoOfUser = newPseudoOfUser;
    expForColorPseudo.setPattern("");
    newPseudoOfUser.replace("[", "\\[").replace("]", "\\]");
    if(newPseudoOfUser.isEmpty() == false)
    {
        expForColorPseudo.setPattern(newPseudoOfUser + "(?![^<>]*(>|</a>))");
    }
    listOfInput.clear();
    currentErrorStreak = 0;
    messageActions->setNewCookie(newConnectCookie, newWebsiteOfCookie);

    QMetaObject::invokeMethod(getTopicMessages, "setNewCookie", Qt::QueuedConnection,
                              Q_ARG(QNetworkCookie, newConnectCookie), Q_ARG(QString, websiteOfCookie), Q_ARG(QString, pseudoOfUser), Q_ARG(bool, updateMessages));
}

void showTopicClass::setNewTheme(QString newThemeName)
{
    baseModel = styleTool::getModel(newThemeName);
    baseModelInfo = styleTool::getModelInfo(newThemeName);
}

void showTopicClass::setNewTopic(QString newTopic)
{
    messagesBox->clear();
    topicName.clear();
    lastDate.clear();
    listOfInfosForEdit.clear();
    currentTypeOfEdit = realTypeOfEdit;
    firstMessageOfTopic.isFirstMessage = false;
    topicLinkLastPage = newTopic;
    topicLinkFirstPage = parsingTool::getFirstPageOfTopic(newTopic);
    firstTimeGetMessages = true;
    errorMode = false;
    currentErrorStreak = 0;
    idOfLastMessageOfUser = 0;
    needToGetMessages = false;
    messageActions->setNewTopic(newTopic);
    QMetaObject::invokeMethod(getTopicMessages, "setNewTopic", Qt::QueuedConnection, Q_ARG(QString, newTopic), Q_ARG(bool, getFirstMessageOfTopic));
}

void showTopicClass::updateSettingInfo()
{
    settingsForMessageParsingStruct settingsForMessageParsing;

    showQuoteButton = settingTool::getThisBoolOption("showQuoteButton");
    disableSelfQuoteButton = settingTool::getThisBoolOption("disableSelfQuoteButton");
    showBlacklistButton = settingTool::getThisBoolOption("showBlacklistButton");
    showEditButton = settingTool::getThisBoolOption("showEditButton");
    showDeleteButton = settingTool::getThisBoolOption("showDeleteButton");
    showSignatures = settingTool::getThisBoolOption("showSignatures");
    showAvatars = settingTool::getThisBoolOption("showAvatars");
    avatarSize = settingTool::getThisIntOption("avatarSize").value;
    ignoreNetworkError = settingTool::getThisBoolOption("ignoreNetworkError");
    colorModoAndAdminPseudo = settingTool::getThisBoolOption("colorModoAndAdminPseudo");
    colorPEMT = settingTool::getThisBoolOption("colorPEMT");
    colorUserPseudoInMessages = settingTool::getThisBoolOption("colorUserPseudoInMessages");
    numberOfMessageShowedFirstTime = settingTool::getThisIntOption("numberOfMessageShowedFirstTime").value;
    getFirstMessageOfTopic = settingTool::getThisBoolOption("getFirstMessageOfTopic");
    warnWhenEdit = settingTool::getThisBoolOption("warnWhenEdit");
    numberOfErrorsBeforeWarning = settingTool::getThisIntOption("numberOfErrorsBeforeWarning").value;
    warnOnFirstTime = settingTool::getThisBoolOption("warnOnFirstTime");
    realTypeOfEdit = settingTool::getThisIntOption("typeOfEdit").value;
    useInternalNavigatorForLinks = settingTool::getThisBoolOption("useInternalNavigatorForLinks");
    downloadHighDefAvatar = settingTool::getThisBoolOption("downloadHighDefAvatar");

    messageActions->updateSettingInfo();

    settingsForMessageParsing.numberOfMessagesForOptimizationStart = settingTool::getThisIntOption("numberOfMessagesForOptimizationStart").value;
    settingsForMessageParsing.infoForMessageParsing.showStickers = settingTool::getThisBoolOption("showStickers");
    settingsForMessageParsing.infoForMessageParsing.stickersSize = settingTool::getThisIntOption("stickersSize").value;
    settingsForMessageParsing.timeoutTime = settingTool::getThisIntOption("timeoutInSecond").value;
    settingsForMessageParsing.infoForMessageParsing.nbMaxQuote = settingTool::getThisIntOption("maxNbOfQuotes").value;
    settingsForMessageParsing.infoForMessageParsing.stickerToSmiley = settingTool::getThisBoolOption("stickersToSmiley");
    settingsForMessageParsing.infoForMessageParsing.betterQuote = settingTool::getThisBoolOption("betterQuote");
    settingsForMessageParsing.infoForMessageParsing.betterCodeTag = settingTool::getThisBoolOption("betterCodeTag");
    settingsForMessageParsing.downloadMissingStickers = settingTool::getThisBoolOption("downloadMissingStickers");
    settingsForMessageParsing.downloadNoelshackImages = settingTool::getThisBoolOption("downloadNoelshackImages");
    settingsForMessageParsing.infoForMessageParsing.noelshackImageWidth = settingTool::getThisIntOption("noelshackImageWidth").value;
    settingsForMessageParsing.infoForMessageParsing.noelshackImageHeight = settingTool::getThisIntOption("noelshackImageHeight").value;
    settingsForMessageParsing.infoForMessageParsing.hideUglyImages = settingTool::getThisBoolOption("hideUglyImages");
    if(settingTool::getThisBoolOption("fastModeEnbled") == false)
    {
        intSettingStruct updateTopicTimeSetting = settingTool::getThisIntOption("updateTopicTime");

        settingsForMessageParsing.timerTime = updateTopicTimeSetting.value;
        settingsForMessageParsing.isInOptimizedMode = (settingTool::getThisIntOption("numberOfPageToLoadForOpti").value == 1 ? true : false);

        if(settingsForMessageParsing.timerTime < updateTopicTimeSetting.minValue)
        {
            settingsForMessageParsing.timerTime = updateTopicTimeSetting.minValue;
        }
    }
    else
    {
        settingsForMessageParsing.timerTime = settingTool::fastModeSpeedRefresh;
        settingsForMessageParsing.isInOptimizedMode = false;
    }

    QMetaObject::invokeMethod(getTopicMessages, "settingsChanged", Qt::QueuedConnection, Q_ARG(settingsForMessageParsingStruct, settingsForMessageParsing));
}

void showTopicClass::addSearchPath(QString newSearchPath)
{
    if(newSearchPath.isEmpty() == false)
    {
        QStringList currentSearchPaths = messagesBox->searchPaths();

        if(currentSearchPaths.indexOf(newSearchPath) == -1)
        {
            currentSearchPaths.append(newSearchPath);
            messagesBox->setSearchPaths(currentSearchPaths);
        }
    }
}

void showTopicClass::resetSearchPath()
{
    messagesBox->setSearchPaths(QStringList(QCoreApplication::applicationDirPath()));
}

void showTopicClass::relayoutDocumentHack()
{
    messagesBox->setLineWrapColumnOrWidth(messagesBox->lineWrapColumnOrWidth());
}

void showTopicClass::addMessageToTheEndOfMessagesBox(const QString& newMessage, long messageID)
{
    messageInfoForEditStruct newInfos;
    int baseSizeOfDocument = messagesBox->document()->characterCount();

    messagesBox->append(newMessage);

    if(currentTypeOfEdit > 0)
    {
        if(currentTypeOfEdit == 1)
        {
            newInfos.messageContent = newMessage;
        }

        newInfos.realPosition = messagesBox->document()->characterCount() - 1;
        newInfos.messageSize = newInfos.realPosition - baseSizeOfDocument;

        listOfInfosForEdit.push_back(QPair<long, messageInfoForEditStruct>(messageID, newInfos));
    }
}

void showTopicClass::editThisMessageOfMessagesBox(QString newMessage, long messageID)
{
    QMutableListIterator<QPair<long, messageInfoForEditStruct>> ite(listOfInfosForEdit);

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
        int baseSizeOfDocument = messagesBox->document()->characterCount();
        int newSizeOfDocument;
        QTextCursor currentCurs = messagesBox->textCursor();

        if(currentTypeOfEdit == 1)
        {
            newMessage.replace("<%MESSAGE_TO_UPDATE%>", ite.peekPrevious().second.messageContent);
        }

        currentCurs.setPosition(ite.peekPrevious().second.realPosition);
        currentCurs.setPosition(ite.peekPrevious().second.realPosition - ite.peekPrevious().second.messageSize, QTextCursor::KeepAnchor);
        currentCurs.insertHtml(newMessage);

        newSizeOfDocument = messagesBox->document()->characterCount() - baseSizeOfDocument;
        ite.peekPrevious().second.messageSize += newSizeOfDocument;

        ite.previous();
        while(ite.hasNext())
        {
            ite.peekNext().second.realPosition += newSizeOfDocument;
            ite.next();
        }
    }
}

QString showTopicClass::getColorOfThisPseudo(QString pseudo)
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

void showTopicClass::setTopicToErrorMode()
{
    if(errorMode == false)
    {
        errorMode = true;
        if(firstTimeGetMessages == true)
        {
            topicLinkLastPage.clear();
            topicName.clear();
            messagesBox->clear();
            firstMessageOfTopic.isFirstMessage = false;
            setMessageStatus("Erreur, topic invalide.");
            setNumberOfConnectedAndMP("", -1, true);
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

void showTopicClass::linkClicked(const QUrl& link)
{
    QString linkInString = link.toDisplayString();

    if(linkInString.startsWith("quote"))
    {
        QString messageQuoted;
        linkInString.remove(0, linkInString.indexOf(':') + 1);
        messageQuoted = linkInString.mid(linkInString.indexOf(':') + 1);
        messageActions->getQuoteInfo(linkInString.left(linkInString.indexOf(':')), messageQuoted);
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
        messageActions->deleteMessage(linkInString);
    }
    else
    {
        utilityTool::openLinkInBrowser(this, useInternalNavigatorForLinks, link.toString(), messageActions->getConnectCookie());
    }
}

void showTopicClass::createContextMenu(const QPoint& thisPoint)
{
    QPoint docPoint = QPoint(thisPoint.x() + messagesBox->horizontalScrollBar()->value(), thisPoint.y() + messagesBox->verticalScrollBar()->value());
    QString anchorOfContextMenu = messagesBox->anchorAt(thisPoint);
    QMenu* contextMenu = messagesBox->createStandardContextMenu(docPoint);
    QList<QAction*> baseActions = contextMenu->actions();
    QAction* actionToAdd = new QAction(contextMenu);

    actionToAdd->setEnabled(anchorOfContextMenu.isEmpty() == false);

    if(useInternalNavigatorForLinks == true)
    {
        actionToAdd->setText("Open link in external browser");
    }
    else
    {
        actionToAdd->setText("Open link in RespawnIRC Navigator");
    }

    if(baseActions.isEmpty() == false)
    {
        QAction* separator = contextMenu->insertSeparator(baseActions.first());
        contextMenu->insertAction(separator, actionToAdd);
    }
    else
    {
        contextMenu->addAction(actionToAdd);
    }

    if(contextMenu->exec(messagesBox->viewport()->mapToGlobal(thisPoint)) == actionToAdd)
    {
        utilityTool::openLinkInBrowser(this, (useInternalNavigatorForLinks == false), anchorOfContextMenu, messageActions->getConnectCookie());
    }

    contextMenu->deleteLater();
}

void showTopicClass::analyzeMessages(QList<messageStruct> listOfNewMessages, QList<QPair<QString, QString>> newListOfInput,
                                             ajaxInfoStruct newAjaxInfo, QString fromThisTopic, bool listIsReallyEmpty)
{
    QString colorOfPseudo;
    QString colorOfDate;
    QStringList listOfAvatarsUsed;
    bool appendHrAtEndOfFirstMessage = false;
    bool errorHappen = false;
    bool firstTimeAddMessages = false;
    bool needToScrollDown = messagesBox->verticalScrollBar()->value() >= messagesBox->verticalScrollBar()->maximum();
    bool newMessagesAreAvailable = false;

    if(parsingTool::getFirstPageOfTopic(fromThisTopic) != topicLinkFirstPage)
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

    if(messagesBox->toPlainText().isEmpty() == true)
    {
        while(listOfNewMessages.size() > numberOfMessageShowedFirstTime)
        {
            listOfNewMessages.pop_front();
        }

        if(listOfNewMessages.isEmpty() == false && getFirstMessageOfTopic == true && firstMessageOfTopic.isFirstMessage == true)
        {
            //si le premier message récupéré est le premier message du topic,
            //on le supprime pour mettre à la place le 1er message du topic récupéré précédement (car pas exactement formaté pareil)
            if(listOfNewMessages.first().idOfMessage == firstMessageOfTopic.idOfMessage)
            {
                listOfNewMessages.pop_front();
            }
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
            parsingTool::replaceWithCapNumber(currentMessage.message, expForColorPseudo, 0,
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
            QString avatarLinkToUse = currentMessage.avatarLink;

            if(downloadHighDefAvatar == true)
            {
                avatarLinkToUse = parsingTool::normalAvatarLinkToHDLink(avatarLinkToUse);
            }

            newMessageToAppend.replace("<%AVATAR_LINK%>", "vtr/" + avatarLinkToUse);
            newMessageToAppend.replace("<%AVATAR_SIZE%>", QString::number(avatarSize));
            listOfAvatarsUsed.append(avatarLinkToUse);
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
                newMessagesAreAvailable = true;
            }
        }
    }

    if(newMessagesAreAvailable == true)
    {
        emit newMessagesAvailable();
    }

    if(needToScrollDown == true)
    {
        messagesBox->verticalScrollBar()->updateGeometry();
        messagesBox->verticalScrollBar()->setValue(messagesBox->verticalScrollBar()->maximum());
    }

    while(listOfInfosForEdit.size() > 40)
    {
        listOfInfosForEdit.pop_front();
    }

    if(pseudoOfUser.isEmpty() == false)
    {
        messageActions->setNewAjaxInfo(newAjaxInfo);
        listOfInput = newListOfInput;

        if(listOfInput.isEmpty() == true)
        {
            if(currentErrorStreak >= (numberOfErrorsBeforeWarning - 1))
            {
                if(ignoreNetworkError == false)
                {
                    QString oldPseudo = pseudoOfUser;
                    setNewCookie(QNetworkCookie(), websiteOfCookie, "");
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

void showTopicClass::setMessageStatus(QString newStatus)
{
    messagesStatus = newStatus;
    emit newMessageStatus();
}

void showTopicClass::setNumberOfConnectedAndMP(QString newNumberConnected, int newNumberMP, bool forceSet)
{
    QString newMessageToShow = newNumberConnected;

    if(newNumberMP >= 0)
    {
        QString newNumberOfMPInString;

        emit newMPAreAvailables(newNumberMP, pseudoOfUser);

        if(newNumberMP == 0)
        {
            newNumberOfMPInString = QString::number(newNumberMP) + " MP";
        }
        else
        {
            newNumberOfMPInString = "<b>" + QString::number(newNumberMP) + " MP</b>";
        }

        newMessageToShow += " - " + newNumberOfMPInString;
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

void showTopicClass::setTopicName(QString newTopicName)
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

void showTopicClass::setCookieFromRequest(QNetworkCookie newConnectCookie, QString currentPseudoOfUser)
{
    if(currentPseudoOfUser == pseudoOfUser)
    {
        setNewCookie(newConnectCookie, websiteOfCookie, pseudoOfUser, false);
        emit newCookieHasToBeSet();
    }
}

void showTopicClass::setUpdatedTopicLink(QString newTopicLink)
{
    topicLinkFirstPage = parsingTool::getFirstPageOfTopic(newTopicLink);
}
