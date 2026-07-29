#include <QCoreApplication>
#include <QApplication>
#include <QSettings>
#include <QList>
#include <QPair>
#include <QString>
#include <QTime>
#include <QtGlobal>

#include "mainWindow.hpp"
#include "logTool.hpp"
#include "settingTool.hpp"
#include "parsingTool.hpp"
#include "getTopicMessages.hpp"
#include "shortcutTool.hpp"
#include "styleTool.hpp"
#include "pathTool.hpp"

int main(int argc, char* argv[])
{
    QCoreApplication::addLibraryPath("./");

    QApplication app(argc, argv);

    /* Tous les dossiers rendus par QStandardPaths en dépendent, il faut donc le définir avant le
     * premier appel à pathTool. L'organisation est laissée vide exprès : elle ajouterait un niveau
     * de dossier au-dessus de RespawnIRC sans rien apporter. */
    QCoreApplication::setApplicationName("RespawnIRC");

    /* Avant toute lecture : les versions antérieures écrivaient à côté de l'exécutable. */
    pathTool::migrateOldUserDataIfNeeded();

    QSettings setting(pathTool::configFilePath(), QSettings::IniFormat);

    logTool::initialize();

    settingTool::setSettings(&setting);
    settingTool::initializeDefaultListsOption();
    shortcutTool::initializeAllShortcutsRules();
    styleTool::getModelInfo("");

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
