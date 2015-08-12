#include "respawnIrc.hpp"
#include "connectWindow.hpp"
#include "selectTopicWindow.hpp"
#include "parsingTool.hpp"

respawnIrcClass::respawnIrcClass(QWidget* parent) : QWidget(parent), setting("config.ini", QSettings::IniFormat)
{
    QPushButton* sendButton = new QPushButton("Envoyer", this);

    messagesBox.setReadOnly(true);
    messagesBox.setOpenExternalLinks(true);
    messageLine.setTabChangesFocus(true);
    messageLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    messageLine.setMaximumHeight(65);
    messageLine.setAcceptRichText(false);
    timerForGetMessage.setInterval(4000);
    timerForGetMessage.stop();
    messagesStatus.setText("Rien.");
    sendButton->setAutoDefault(true);
    reply = 0;
    replyForSendMessage = 0;
    firstTimeGetMessages = true;
    retrievesMessage = false;
    idOfLastMessage = 0;
    isConnected = false;
    linkHasChanged = false;

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&messagesBox, 0, 0, 1, 2);
    mainLayout->addWidget(&messageLine, 1, 0);
    mainLayout->addWidget(sendButton, 1, 1);
    mainLayout->addWidget(&messagesStatus, 2, 0, 1, 2);

    setLayout(mainLayout);

    connect(&timerForGetMessage, SIGNAL(timeout()), SLOT(getMessages()));
    connect(sendButton, SIGNAL(pressed()), SLOT(postMessage()));

    loadSettings();
}

void respawnIrcClass::warnUser()
{
    QApplication::alert(this);
}

void respawnIrcClass::loadSettings()
{
    pseudoOfUser = setting.value("pseudo", "").toString();
    topicLink = setting.value("topicLink", "").toString();

    if(topicLink.isEmpty() == false)
    {
        setNewTopic(topicLink);
    }

    if(setting.value("dlrowolleh", "").toString().isEmpty() == false && setting.value("coniunctio", "").toString().isEmpty() == false)
    {
        QList<QNetworkCookie> newCookies;
        newCookies.append(QNetworkCookie(QByteArray("dlrowolleh"), setting.value("dlrowolleh").toByteArray()));
        newCookies.append(QNetworkCookie(QByteArray("coniunctio"), setting.value("coniunctio").toByteArray()));

        networkManager.cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
        isConnected = true;
    }
}

void respawnIrcClass::startGetMessage()
{
    if(retrievesMessage == false)
    {
        timerForGetMessage.start();
        getMessages();
    }
}

void respawnIrcClass::showConnect()
{
    connectWindowClass* myConnectWindow = new connectWindowClass(this);
    connect(myConnectWindow, SIGNAL(newCookiesAvailable(QList<QNetworkCookie>,QString,bool)), this, SLOT(setNewCookies(QList<QNetworkCookie>,QString,bool)));
    myConnectWindow->exec();
}

void respawnIrcClass::showSelectTopic()
{
    selectTopicWindow* mySelectTopicWindow = new selectTopicWindow(topicLink, this);
    connect(mySelectTopicWindow, SIGNAL(newTopicSelected(QString)), this, SLOT(setNewTopic(QString)));
    mySelectTopicWindow->exec();
}

void respawnIrcClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo)
{
    networkManager.setCookieJar(new QNetworkCookieJar(this)); //fuite ?
    networkManager.cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
    pseudoOfUser = newPseudoOfUser;
    isConnected = true;

    startGetMessage();

    if(saveInfo == true)
    {
        for(int i = 0; i < newCookies.size(); ++i)
        {
            setting.setValue(newCookies.at(i).name(), newCookies.at(i).value());
        }
        setting.setValue("pseudo", pseudoOfUser);
    }
}

void respawnIrcClass::setNewTopic(QString newTopic)
{
    topicLink = newTopic;
    messagesBox.clear();
    firstTimeGetMessages = true;
    linkHasChanged = true;
    idOfLastMessage = 0;

    startGetMessage();

    setting.setValue("topicLink", topicLink);
}

void respawnIrcClass::getMessages()
{
    if(retrievesMessage == false)
    {
        retrievesMessage = true;

        if(reply == 0)
        {
            QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl(topicLink);
            messagesStatus.setText("Récupération des messages en cours...");
            linkHasChanged = false;
            reply = networkManager.get(request);
            connect(reply, SIGNAL(finished()), this, SLOT(analyzeMessages()));
        }
        else
        {
            retrievesMessage = false;
        }
    }
}

void respawnIrcClass::analyzeMessages()
{
    QString newTopicLink;
    QString colorOfPseudo;
    QString source = reply->readAll();
    reply->deleteLater();
    reply = 0;

    if(linkHasChanged == true)
    {
        retrievesMessage = false;
        return;
    }

    messagesStatus.setText("Récupération des messages terminé !");

    newTopicLink = parsingToolClass::getLastPageOfTopic(source);
//a changer // enfin j'sais pas
    if(firstTimeGetMessages == false || newTopicLink.isEmpty() == true)
    {
        QList<int> listOfMessageID = parsingToolClass::getListOfMessageID(source);
        QList<QString> listOfPseudo = parsingToolClass::getListOfPseudo(source);
        QList<QString> listOfDate = parsingToolClass::getListOfDate(source);
        QList<QString> listOfMessage = parsingToolClass::getListOfMessage(source);

        if((listOfMessageID.size() == listOfPseudo.size() && listOfPseudo.size() == listOfDate.size() && listOfDate.size() == listOfMessage.size()) == false || listOfDate.size() == 0)
        {
            QMessageBox messageBox;
            topicLink.clear();
            timerForGetMessage.stop();
            messagesBox.clear();
            messagesStatus.setText("Erreur.");
            messageBox.warning(this, "Erreur", "Un problème est survenu lors de la récupération des messages.");
            retrievesMessage = false;
            return;
        }

        for(int i = 0; i < listOfMessageID.size(); ++i)
        {
            if(listOfMessageID.at(i) > idOfLastMessage)
            {
                if(pseudoOfUser.toLower() == listOfPseudo.at(i).toLower())
                {
                    colorOfPseudo = "blue";
                }
                else
                {
                    colorOfPseudo = "dimgrey";
                }

                messagesBox.append("<table><tr><td>[" + listOfDate.at(i) + "] &lt;<span style=\"color: " + colorOfPseudo + ";\">" + listOfPseudo.at(i) + "</span>&gt;</td><td>" + listOfMessage.at(i) + "</td></tr></table>");
                messagesBox.verticalScrollBar()->updateGeometry();
                messagesBox.verticalScrollBar()->setValue(messagesBox.verticalScrollBar()->maximum());
                idOfLastMessage = listOfMessageID.at(i);
                warnUser();
            }
        }
    }
//fin
    if(isConnected == true)
    {
        listOfInput.clear();
        parsingToolClass::getListOfHiddenInputFromThisForm(source, "form-post-topic form-post-message", listOfInput);
    }

    firstTimeGetMessages = false;
    retrievesMessage = false;

    if(newTopicLink.isEmpty() == false)
    {
        topicLink = newTopicLink;
        startGetMessage();
    }
}

void respawnIrcClass::postMessage()
{
    if(replyForSendMessage == 0 && isConnected == true && topicLink.isEmpty() == false)
    {
        QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl(topicLink);
        QString data;

        for(int i = 0; i < listOfInput.size(); ++i)
        {
            data += listOfInput.at(i).first + "=" + listOfInput.at(i).second + "&";
        }

        data += "message_topic=" + messageLine.toPlainText().replace("&", "%26").replace("+", "%2B") + "&form_alias_rang=1";

        replyForSendMessage = networkManager.post(request, data.toAscii());
        connect(replyForSendMessage, SIGNAL(finished()), this, SLOT(deleteReplyForSendMessage()));
        messageLine.clear();
    }
}

void respawnIrcClass::deleteReplyForSendMessage()
{
    replyForSendMessage->deleteLater();
    replyForSendMessage = 0;
    startGetMessage();
}

void respawnIrcClass::focusInEvent(QFocusEvent* event)
{
    (void)event;
    messageLine.setFocus();
}
