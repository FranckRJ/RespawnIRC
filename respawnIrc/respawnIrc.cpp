#include "respawnIrc.hpp"
#include "connectWindow.hpp"
#include "selectTopicWindow.hpp"
#include "captchaWindow.hpp"
#include "parsingTool.hpp"

respawnIrcClass::respawnIrcClass(QWidget* parent) : QWidget(parent), setting("config.ini", QSettings::IniFormat)
{
    tabList.setTabsClosable(true);
    messageLine.setTabChangesFocus(true);
    messageLine.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    messageLine.setMaximumHeight(65);
    messageLine.setAcceptRichText(false);
    sendButton.setText("Envoyer");
    sendButton.setAutoDefault(true);
    alertImage.load("ressources/alert.png");
    replyForSendMessage = 0;
    isConnected = false;

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(&tabList, 0, 0, 1, 2);
    mainLayout->addWidget(&messageLine, 1, 0);
    mainLayout->addWidget(&sendButton, 1, 1);
    mainLayout->addWidget(&messagesStatus, 2, 0, 1, 2);

    setLayout(mainLayout);

    connect(&sendButton, SIGNAL(pressed()), SLOT(postMessage()));
    connect(&tabList, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
    connect(&tabList, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTab(int)));

    loadSettings();
}

void respawnIrcClass::loadSettings()
{
    pseudoOfUser = setting.value("pseudo", "").toString();

    if(setting.value("dlrowolleh", "").toString().isEmpty() == false && setting.value("coniunctio", "").toString().isEmpty() == false)
    {
        QList<QNetworkCookie> newCookies;
        newCookies.append(QNetworkCookie(QByteArray("dlrowolleh"), setting.value("dlrowolleh").toByteArray()));
        newCookies.append(QNetworkCookie(QByteArray("coniunctio"), setting.value("coniunctio").toByteArray()));

        setNewCookies(newCookies, pseudoOfUser, false);
    }

    if(setting.value("listOfTopicLink", QList<QVariant>()).toList().isEmpty() == false)
    {
        listOfTopicLink = createListWithThisQVariantList(setting.value("listOfTopicLink").toList());

        for(int i = 0; i < listOfTopicLink.size(); ++i)
        {
            addNewTab();
            tabList.setCurrentIndex(i);
            setNewTopic(listOfTopicLink.at(i));
            tabList.setCurrentIndex(0);
        }
    }

    if(listOfShowTopicMessages.isEmpty() == true)
    {
        addNewTab();
    }

    messagesStatus.setText(getCurrentWidget()->getMessagesStatus());
}

showTopicMessagesClass* respawnIrcClass::getCurrentWidget()
{
    return listOfShowTopicMessages.at(tabList.currentIndex());
}

QString respawnIrcClass::buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput)
{
    QString data;

    for(int i = 0; i < listOfInput.size(); ++i)
    {
        data += listOfInput.at(i).first + "=" + listOfInput.at(i).second + "&";
    }

    data += "message_topic=" + messageLine.toPlainText().replace("&", "%26").replace("+", "%2B");

    if(captchaCode.isEmpty() == false)
    {
        data += "&fs_ccode=" + captchaCode;
    }

    data += "&form_alias_rang=1";

    return data;
}

QList<QVariant> respawnIrcClass::createQVariantListWithThisList(QList<QString> list)
{
    QList<QVariant> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        newList.push_back(list.at(i));
    }

    return newList;
}

QList<QString> respawnIrcClass::createListWithThisQVariantList(QList<QVariant> list)
{
    QList<QString> newList;

    for(int i = 0; i < list.size(); ++i)
    {
        newList.push_back(list.at(i).toString());
    }

    return newList;
}

void respawnIrcClass::showConnect()
{
    connectWindowClass* myConnectWindow = new connectWindowClass(this);
    connect(myConnectWindow, SIGNAL(newCookiesAvailable(QList<QNetworkCookie>,QString,bool)), this, SLOT(setNewCookies(QList<QNetworkCookie>,QString,bool)));
    myConnectWindow->exec();
}

void respawnIrcClass::showSelectTopic()
{
    selectTopicWindow* mySelectTopicWindow = new selectTopicWindow(getCurrentWidget()->getTopicLink(), this);
    connect(mySelectTopicWindow, SIGNAL(newTopicSelected(QString)), this, SLOT(setNewTopic(QString)));
    mySelectTopicWindow->exec();
}

void respawnIrcClass::addNewTab()
{
    listOfShowTopicMessages.push_back(new showTopicMessagesClass(this));

    if(listOfShowTopicMessages.size() > listOfTopicLink.size())
    {
        listOfTopicLink.push_back(QString());
    }

    if(isConnected == true)
    {
        listOfShowTopicMessages.back()->setNewCookies(networkManager.cookieJar()->cookiesForUrl(QUrl("http://www.jeuxvideo.com")), pseudoOfUser);
    }

    connect(listOfShowTopicMessages.back(), SIGNAL(newMessageStatus()), this, SLOT(setNewMessageStatus()));
    connect(listOfShowTopicMessages.back(), SIGNAL(newMessagesAvailable()), this, SLOT(warnUserForNewMessages()));
    connect(listOfShowTopicMessages.back(), SIGNAL(newNameForTopic(QString)), this, SLOT(setNewTopicName(QString)));
    tabList.addTab(listOfShowTopicMessages.back(), "Onglet " + QString::number(listOfShowTopicMessages.size()));
}

void respawnIrcClass::removeTab(int index)
{
    if(listOfShowTopicMessages.size() > 1)
    {
        tabList.removeTab(index);
        listOfTopicLink.removeAt(index);
        setting.setValue("listOfTopicLink", createQVariantListWithThisList(listOfTopicLink));
        delete listOfShowTopicMessages.takeAt(index);
    }
}

void respawnIrcClass::setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo)
{
    networkManager.setCookieJar(new QNetworkCookieJar(this)); //fuite ?
    networkManager.cookieJar()->setCookiesFromUrl(newCookies, QUrl("http://www.jeuxvideo.com"));
    pseudoOfUser = newPseudoOfUser;
    isConnected = true;

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        listOfShowTopicMessages.at(i)->setNewCookies(newCookies, newPseudoOfUser);
    }

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
    getCurrentWidget()->setNewTopic(newTopic);
    listOfTopicLink[tabList.currentIndex()] = newTopic;

    setting.setValue("listOfTopicLink", createQVariantListWithThisList(listOfTopicLink));
}

void respawnIrcClass::setCodeForCaptcha(QString code)
{
    captchaCode = code;
    postMessage();
}

void respawnIrcClass::setNewMessageStatus()
{
    messagesStatus.setText(getCurrentWidget()->getMessagesStatus());
}

void respawnIrcClass::setNewTopicName(QString topicName)
{
    QObject* senderObject = sender();

    for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
    {
        if(senderObject == listOfShowTopicMessages.at(i))
        {
            tabList.setTabText(i, topicName);
        }
    }
}

void respawnIrcClass::warnUserForNewMessages()
{
    QApplication::alert(this);
    QObject* senderObject = sender();

    if(senderObject != getCurrentWidget())
    {
        for(int i = 0; i < listOfShowTopicMessages.size(); ++i)
        {
            if(senderObject == listOfShowTopicMessages.at(i))
            {
                tabList.setTabIcon(i, QIcon(alertImage));
            }
        }
    }
}

void respawnIrcClass::currentTabChanged(int newIndex)
{
    setNewMessageStatus();
    tabList.setTabIcon(newIndex, QIcon());
}

void respawnIrcClass::postMessage()
{
    if(replyForSendMessage == 0 && isConnected == true && getCurrentWidget()->getTopicLink().isEmpty() == false)
    {
        QNetworkRequest request = parsingToolClass::buildRequestWithThisUrl(getCurrentWidget()->getTopicLink());
        QString data;

        if(getCurrentWidget()->getCaptchaLink().isEmpty() == false && captchaCode.isEmpty() == true)
        {
            captchaWindowClass* myCaptchaWindow = new captchaWindowClass(getCurrentWidget()->getCaptchaLink(), this);
            connect(myCaptchaWindow, SIGNAL(codeForCaptcha(QString)), this, SLOT(setCodeForCaptcha(QString)));
            oldListOfInput = getCurrentWidget()->getListOfInput();
            myCaptchaWindow->exec();
            return;
        }

        sendButton.setEnabled(false);

        if(captchaCode.isEmpty() == true)
        {
            data = buildDataWithThisListOfInput(getCurrentWidget()->getListOfInput());
        }
        else
        {
            data = buildDataWithThisListOfInput(oldListOfInput);
        }

        replyForSendMessage = networkManager.post(request, data.toAscii());
        connect(replyForSendMessage, SIGNAL(finished()), this, SLOT(deleteReplyForSendMessage()));
    }
}

void respawnIrcClass::deleteReplyForSendMessage()
{
    QString source = replyForSendMessage->readAll();
    replyForSendMessage->deleteLater();
    replyForSendMessage = 0;
    captchaCode.clear();

    if(source.size() == 0)
    {
        messageLine.clear();
        sendButton.setEnabled(true);
    }
    else if(source.contains("<div class=\"alert-row\"> Le code de confirmation est incorrect. </div>") == true)
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le code de confirmation est incorrect.");
        sendButton.setEnabled(true);
    }
    else
    {
        QMessageBox messageBox;
        messageBox.warning(this, "Erreur", "Le message n'a pas été envoyé.");
        sendButton.setEnabled(true);
    }

    getCurrentWidget()->startGetMessage();
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
