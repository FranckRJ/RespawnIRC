#include "respawnIrc.hpp"
#include "connectWindow.hpp"
#include "selectTopicWindow.hpp"
#include "parsingTool.hpp"

respawnIrcClass::respawnIrcClass(QWidget* parent) : QWidget(parent)
{
    QPushButton* sendButton = new QPushButton("Envoyer", this);

    messagesBox.setReadOnly(true);
    messagesBox.setOpenExternalLinks(true);
    messageLine.setTabChangesFocus(true);
    messageLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    messageLine.setMaximumHeight(65);
    networkManager.setCookieJar(new QNetworkCookieJar(this));
    timerForGetMessage.setInterval(5000);
    timerForGetMessage.stop();
    messagesStatus.setText("Rien.");
    sendButton->setAutoDefault(true);
    reply = 0;
    replyForSendMessage = 0;
    firstTimeGetMessages = true;
    idOfLastMessage = 0;

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&messagesBox, 0, 0, 1, 2);
    mainLayout->addWidget(&messageLine, 1, 0);
    mainLayout->addWidget(sendButton, 1, 1);
    mainLayout->addWidget(&messagesStatus, 2, 0, 1, 2);

    setLayout(mainLayout);

    connect(&timerForGetMessage, SIGNAL(timeout()), SLOT(getMessages()));
    connect(sendButton, SIGNAL(pressed()), SLOT(postMessage()));
}

void respawnIrcClass::warnUser()
{
    QApplication::alert(this);
}

void respawnIrcClass::showConnect()
{
    connectWindowClass* myConnectWindow = new connectWindowClass(this);
    connect(myConnectWindow, SIGNAL(newCookiesAvailable(QList<QNetworkCookie>)), this, SLOT(setNewCookies(QList<QNetworkCookie>)));
    myConnectWindow->exec();
}

void respawnIrcClass::showSelectTopic()
{
    selectTopicWindow* mySelectTopicWindow = new selectTopicWindow(topicLink, this);
    connect(mySelectTopicWindow, SIGNAL(newTopicSelected(QString)), this, SLOT(setNewTopic(QString)));
    mySelectTopicWindow->exec();
}

void respawnIrcClass::setNewCookies(QList<QNetworkCookie> newCookies)
{
    networkManager.cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
    isConnected = true;
}

void respawnIrcClass::setNewTopic(QString newTopic)
{
    topicLink = newTopic;
    messagesBox.clear();
    firstTimeGetMessages = true;
    idOfLastMessage = 0;

    if(timerForGetMessage.isActive() == false)
    {
        timerForGetMessage.start();
        getMessages();
    }
}

void respawnIrcClass::getMessages()
{
    if(reply == 0)
    {
        QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl(topicLink);
        messagesStatus.setText("Récupération des messages en cours...");
        reply = networkManager.get(request);
        connect(reply, SIGNAL(finished()), this, SLOT(analyzeMessages()));
    }
}

void respawnIrcClass::analyzeMessages()
{
    QString newTopicLink;
    QString source = reply->readAll();
    reply->deleteLater();
    reply = 0;

    messagesStatus.setText("Récupération des messages terminé !");

    newTopicLink = parsingToolClass::getLastPageOfTopic(source);
//a changer // enfin j'sais pas
    if(firstTimeGetMessages == false || newTopicLink.isEmpty() == true)
    {
        QList<int> listOfMessageID = parsingToolClass::getListOfMessageID(source);
        QList<QString> listOfPseudo = parsingToolClass::getListOfPseudo(source);
        QList<QString> listOfDate = parsingToolClass::getListOfDate(source);
        QList<QString> listOfMessage = parsingToolClass::getListOfMessage(source);

        for(int i = 0; i < listOfMessageID.size(); ++i)
        {
            if(listOfMessageID.at(i) > idOfLastMessage)
            {
                messagesBox.insertHtml("<table><tr><td>[" + listOfDate.at(i) + "] &lt;" + listOfPseudo.at(i) + "&gt;</td><td>" + listOfMessage.at(i) + "</td></tr></table>");
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

    if(newTopicLink.isEmpty() == false)
    {
        topicLink = newTopicLink;
        timerForGetMessage.start();
        getMessages();
    }

    firstTimeGetMessages = false;
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

        data += "message_topic=" + messageLine.toPlainText() + "&form_alias_rang=1";

        replyForSendMessage = networkManager.post(request, data.toAscii());
        connect(replyForSendMessage, SIGNAL(finished()), this, SLOT(deleteReplyForSendMessage()));
        messageLine.clear();
    }
}

void respawnIrcClass::deleteReplyForSendMessage()
{
    replyForSendMessage->deleteLater();
    replyForSendMessage = 0;
}

void respawnIrcClass::focusInEvent(QFocusEvent* event)
{
    (void)event;
    messageLine.setFocus();
}
