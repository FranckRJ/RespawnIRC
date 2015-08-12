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

                messagesBox.append("<table><tr><td>[" + listOfDate.at(i) +
                                   "] &lt;<a href=\"http://www.jeuxvideo.com/profil/" + listOfPseudo.at(i).toLower() +
                                   "?mode=infos\"><span style=\"color: " + colorOfPseudo + ";text-decoration: none\">" +
                                   listOfPseudo.at(i) + "</span></a>&gt;</td><td>" + listOfMessage.at(i) + "</td></tr></table>");
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

void respawnIrcClass::clipboardChanged()
{
    QClipboard* clipboard = QApplication::clipboard();
    const QMimeData* data = clipboard->mimeData();
    QString dataInHtml = data->html();

    if(dataInHtml.contains("<img src=\"smileys/"))
    {
        QTextDocument doc;
        QString newDataInHtml = dataInHtml;
        //a changer, si possible
        newDataInHtml.replace("<img src=\"smileys/1.gif\" />", ":)");
        newDataInHtml.replace("<img src=\"smileys/2.gif\" />", ":question:");
        newDataInHtml.replace("<img src=\"smileys/3.gif\" />", ":g)");
        newDataInHtml.replace("<img src=\"smileys/4.gif\" />", ":d)");
        newDataInHtml.replace("<img src=\"smileys/5.gif\" />", ":cd:");
        newDataInHtml.replace("<img src=\"smileys/6.gif\" />", ":globe:");
        newDataInHtml.replace("<img src=\"smileys/7.gif\" />", ":p)");
        newDataInHtml.replace("<img src=\"smileys/8.gif\" />", ":malade:");
        newDataInHtml.replace("<img src=\"smileys/9.gif\" />", ":pacg:");
        newDataInHtml.replace("<img src=\"smileys/10.gif\" />", ":pacd:");
        newDataInHtml.replace("<img src=\"smileys/11.gif\" />", ":noel:");
        newDataInHtml.replace("<img src=\"smileys/12.gif\" />", ":o))");
        newDataInHtml.replace("<img src=\"smileys/13.gif\" />", ":snif2:");
        newDataInHtml.replace("<img src=\"smileys/14.gif\" />", ":-(");
        newDataInHtml.replace("<img src=\"smileys/15.gif\" />", ":-((");
        newDataInHtml.replace("<img src=\"smileys/16.gif\" />", ":mac:");
        newDataInHtml.replace("<img src=\"smileys/17.gif\" />", ":gba:");
        newDataInHtml.replace("<img src=\"smileys/18.gif\" />", ":hap:");
        newDataInHtml.replace("<img src=\"smileys/19.gif\" />", ":nah:");
        newDataInHtml.replace("<img src=\"smileys/20.gif\" />", ":snif:");
        newDataInHtml.replace("<img src=\"smileys/21.gif\" />", ":mort:");
        newDataInHtml.replace("<img src=\"smileys/22.gif\" />", ":ouch:");
        newDataInHtml.replace("<img src=\"smileys/23.gif\" />", ":-)))");
        newDataInHtml.replace("<img src=\"smileys/24.gif\" />", ":content:");
        newDataInHtml.replace("<img src=\"smileys/25.gif\" />", ":nonnon:");
        newDataInHtml.replace("<img src=\"smileys/26.gif\" />", ":cool:");
        newDataInHtml.replace("<img src=\"smileys/27.gif\" />", ":sleep:");
        newDataInHtml.replace("<img src=\"smileys/28.gif\" />", ":doute:");
        newDataInHtml.replace("<img src=\"smileys/29.gif\" />", ":hello:");
        newDataInHtml.replace("<img src=\"smileys/30.gif\" />", ":honte:");
        newDataInHtml.replace("<img src=\"smileys/31.gif\" />", ":-p");
        newDataInHtml.replace("<img src=\"smileys/32.gif\" />", ":lol:");
        newDataInHtml.replace("<img src=\"smileys/33.gif\" />", ":non2:");
        newDataInHtml.replace("<img src=\"smileys/34.gif\" />", ":monoeil:");
        newDataInHtml.replace("<img src=\"smileys/35.gif\" />", ":non:");
        newDataInHtml.replace("<img src=\"smileys/36.gif\" />", ":ok:");
        newDataInHtml.replace("<img src=\"smileys/37.gif\" />", ":oui:");
        newDataInHtml.replace("<img src=\"smileys/38.gif\" />", ":rechercher:");
        newDataInHtml.replace("<img src=\"smileys/39.gif\" />", ":rire:");
        newDataInHtml.replace("<img src=\"smileys/40.gif\" />", ":-D:");
        newDataInHtml.replace("<img src=\"smileys/41.gif\" />", ":rire2:");
        newDataInHtml.replace("<img src=\"smileys/42.gif\" />", ":salut:");
        newDataInHtml.replace("<img src=\"smileys/43.gif\" />", ":sarcastic:");
        newDataInHtml.replace("<img src=\"smileys/44.gif\" />", ":up:");
        newDataInHtml.replace("<img src=\"smileys/45.gif\" />", ":(");
        newDataInHtml.replace("<img src=\"smileys/46.gif\" />", ":-)");
        newDataInHtml.replace("<img src=\"smileys/47.gif\" />", ":peur:");
        newDataInHtml.replace("<img src=\"smileys/48.gif\" />", ":bye:");
        newDataInHtml.replace("<img src=\"smileys/49.gif\" />", ":dpdr:");
        newDataInHtml.replace("<img src=\"smileys/50.gif\" />", ":fou:");
        newDataInHtml.replace("<img src=\"smileys/51.gif\" />", ":gne:");
        newDataInHtml.replace("<img src=\"smileys/52.gif\" />", ":dehors:");
        newDataInHtml.replace("<img src=\"smileys/53.gif\" />", ":fier:");
        newDataInHtml.replace("<img src=\"smileys/54.gif\" />", ":coeur:");
        newDataInHtml.replace("<img src=\"smileys/55.gif\" />", ":rouge:");
        newDataInHtml.replace("<img src=\"smileys/56.gif\" />", ":sors:");
        newDataInHtml.replace("<img src=\"smileys/57.gif\" />", ":ouch:");
        newDataInHtml.replace("<img src=\"smileys/58.gif\" />", ":merci:");
        newDataInHtml.replace("<img src=\"smileys/59.gif\" />", ":svp:");
        newDataInHtml.replace("<img src=\"smileys/60.gif\" />", ":ange:");
        newDataInHtml.replace("<img src=\"smileys/61.gif\" />", ":diable:");
        newDataInHtml.replace("<img src=\"smileys/62.gif\" />", ":gni:");
        newDataInHtml.replace("<img src=\"smileys/63.gif\" />", ":spoiler:");
        newDataInHtml.replace("<img src=\"smileys/64.gif\" />", ":hs:");
        newDataInHtml.replace("<img src=\"smileys/65.gif\" />", ":desole:");
        newDataInHtml.replace("<img src=\"smileys/66.gif\" />", ":fete:");
        newDataInHtml.replace("<img src=\"smileys/67.gif\" />", ":sournois:");
        newDataInHtml.replace("<img src=\"smileys/68.gif\" />", ":hum:");
        newDataInHtml.replace("<img src=\"smileys/69.gif\" />", ":bravo:");
        newDataInHtml.replace("<img src=\"smileys/70.gif\" />", ":banzai:");
        newDataInHtml.replace("<img src=\"smileys/71.gif\" />", ":bave:");
        newDataInHtml.replace("<img src=\"smileys/cimer.gif\" />", ":cimer:");
        newDataInHtml.replace("<img src=\"smileys/ddb.gif\" />", ":ddb:");
        newDataInHtml.replace("<img src=\"smileys/hapoelparty.gif\" />", ":hapoelparty:");
        newDataInHtml.replace("<img src=\"smileys/loveyou.gif\" />", ":loveyou:");
        newDataInHtml.replace("<img src=\"smileys/nyu.gif\" />", ":cute:");
        newDataInHtml.replace("<img src=\"smileys/objection.gif\" />", ":objection:");
        newDataInHtml.replace("<img src=\"smileys/pave.gif\" />", ":pave:");
        newDataInHtml.replace("<img src=\"smileys/pf.gif\" />", ":pf:");
        newDataInHtml.replace("<img src=\"smileys/play.gif\" />", ":play:");
        newDataInHtml.replace("<img src=\"smileys/siffle.gif\" />", ":siffle:");
        //fin
        doc.setHtml(newDataInHtml);
        QString text = doc.toPlainText();

        if(text != data->text())
        {
            QMimeData* newData = new QMimeData();
            newData->setHtml(dataInHtml);
            newData->setText(doc.toPlainText());
            clipboard->setMimeData(newData);
        }
    }
}
