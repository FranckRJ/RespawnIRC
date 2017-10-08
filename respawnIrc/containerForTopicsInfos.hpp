#ifndef CONTAINERFORTOPICSINFOS_HPP
#define CONTAINERFORTOPICSINFOS_HPP

#include <QWidget>
#include <QList>
#include <QString>
#include <QNetworkCookie>

#include "showTopic.hpp"
#include "showForum.hpp"
#include "colorPseudoListWindow.hpp"

enum class typeOfSaveForPseudo { DONT_REMEMBER, DEFAULT, REMEMBER };

class containerForTopicsInfosClass : public QWidget
{
    Q_OBJECT
public:
    explicit containerForTopicsInfosClass(const QList<QString>* newListOfIgnoredPseudo, const QList<pseudoWithColorStruct>* newListOfColorPseudo, QString currentThemeName, QWidget* parent = nullptr);
    const showTopicClass& getConstShowTopic() const;
    showTopicClass& getShowTopic();
    typeOfSaveForPseudo getPseudoTypeOfSave() const;
    QString getTopicLinkFirstPage() const;
    void setNewCookieForInfo(QNetworkCookie newConnectCookie, QString newPseudoOfUser, typeOfSaveForPseudo newTypeOfSave);
    void setBufferForTopicLinkFirstPage(QString newLink);
    void updateSettingsForInfo();
    void setNewThemeForInfo(QString newThemeName);
    void setNewTopicForInfo(QString newTopic);
signals:
    void topicNeedChanged(QString topicLink);
    void openThisTopicInNewTab(QString topicLink);
private:
    showTopicClass* showTopic;
    showForumClass* showForum;
    typeOfSaveForPseudo pseudoTypeOfSave = typeOfSaveForPseudo::DEFAULT;
    QString bufferForTopicLinkFirstPage;

};

#endif
