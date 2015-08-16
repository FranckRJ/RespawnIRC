#ifndef RESPAWNIRC_HPP
#define RESPAWNIRC_HPP

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

#include "showTopicMessages.hpp"
#include "multiTypeTextBox.hpp"

class respawnIrcClass : public QWidget
{
    Q_OBJECT
public:
    respawnIrcClass(QWidget* parent = 0);
    void loadSettings();
    showTopicMessagesClass* getCurrentWidget();
    QString buildDataWithThisListOfInput(const QList<QPair<QString, QString> >& listOfInput);
    void addButtonToButtonLayout();
    void setButtonInButtonLayoutVisible(bool visible);
public slots:
    void addBold();
    void addItalic();
    void addUnderLine();
    void addStrike();
    void addUList();
    void addOListe();
    void addQuote();
    void addCode();
    void addSpoil();
    void showConnect();
    void showSelectTopic();
    void showIgnoreListWindow();
    void addNewTab();
    void removeTab(int index);
    void goToCurrentTopic();
    void goToCurrentForum();
    void setShowTextDecorationButton(bool newVal);
    void setMultilineEdit(bool newVal);
    void setNewCookies(QList<QNetworkCookie> newCookies, QString newPseudoOfUser, bool saveInfo);
    void setNewTopic(QString newTopic);
    void setCodeForCaptcha(QString code);
    void setNewMessageStatus();
    void setNewNumberOfConnectedAndPseudoUsed();
    void setNewTopicName(QString topicName);
    void saveListOfIgnoredPseudo();
    void warnUserForNewMessages();
    void currentTabChanged(int newIndex);
    void postMessage();
    void deleteReplyForSendMessage();
    void clipboardChanged();
protected:
    void focusInEvent(QFocusEvent * event);
private:
    QGridLayout* mainLayout;
    QHBoxLayout* buttonLayout;
    QTabWidget tabList;
    QList<showTopicMessagesClass*> listOfShowTopicMessages;
    QList<QString> listOfTopicLink;
    QList<QString> listOfIgnoredPseudo;
    QList<QPair<QString, QString> > oldListOfInput;
    multiTypeTextBoxClass messageLine;
    QNetworkReply* replyForSendMessage;
    QNetworkAccessManager networkManager;
    QLabel messagesStatus;
    QLabel numberOfConnectedAndPseudoUsed;
    QPushButton sendButton;
    QString pseudoOfUser;
    QString captchaCode;
    QPixmap alertImage;
    bool isConnected;
};

#endif
