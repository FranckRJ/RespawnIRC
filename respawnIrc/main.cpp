#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QList>
#include <QPair>
#include <QString>
#include <QTime>
#include <QtGlobal>

#include "mainWindow.hpp"
#include "settingTool.hpp"
#include "parsingTool.hpp"
#include "getTopicMessages.hpp"
#include "shortcutTool.hpp"
#include "styleTool.hpp"

int main(int argc, char* argv[])
{
    QCoreApplication::addLibraryPath("./");

    QApplication app(argc, argv);
    QSettings setting(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);

    qsrand(QTime::currentTime().msecsSinceStartOfDay());
    parsingToolClass::generateNewUserAgent();

    settingToolClass::setSettings(&setting);
    settingToolClass::initializeDefaultListsOption();
    shortcutToolClass::loadShortcutRule("shortcut");
    shortcutToolClass::loadShortcutRule("noLangageSticker", R"rgx(<img class="img-stickers" src="http://jv\.stkr\.fr/p[^/]*/)rgx", R"rgx("/>)rgx", "<img class=\"img-stickers\" src=\"http://jv.stkr.fr/p/", "\"/>", true);
    shortcutToolClass::loadShortcutRule("stickerToSmiley", R"rgx(<img class="img-stickers" src="http://jv\.stkr\.fr/p[^/]*/)rgx", R"rgx("/>)rgx", "<img src=\"resources/smileys/", "\"/>", true);
    styleToolClass::getModelInfo("");

    qRegisterMetaType<infoForMessageParsingStruct>("infoForMessageParsingStruct");
    qRegisterMetaType<settingsForMessageParsingStruct>("settingsForMessageParsingStruct");
    qRegisterMetaType<ajaxInfoStruct>("ajaxInfoStruct");
    qRegisterMetaType<messageStruct>("messageStruct");
    qRegisterMetaType<QList<messageStruct>>("QList<messageStruct>");
    qRegisterMetaType<QPair<QString, QString>> ("QPair<QString, QString>");
    qRegisterMetaType<QList<QPair<QString, QString>>>("QList<QPair<QString, QString>>");

    mainWindowClass mainWindow;
    mainWindow.show();

    return app.exec();
}
