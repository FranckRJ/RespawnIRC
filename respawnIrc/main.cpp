#include <QApplication>

#include "mainWindow.hpp"
#include "settingTool.hpp"
#include "parsingTool.hpp"
#include "shortcutTool.hpp"
#include "stickerToSmileyTool.hpp"
#include "styleTool.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSettings setting(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);

    QCoreApplication::addLibraryPath("./");
    settingToolClass::setSettings(&setting);
    settingToolClass::initializeDefaultListsOption();
    shortcutToolClass::loadShortcuts();
    stickerToSmileyToolClass::loadTransformInfo();
    styleToolClass::getModelInfo("");

    qRegisterMetaType<messageStruct>("messageStruct");
    qRegisterMetaType<QList<messageStruct> >("QList<messageStruct>");
    qRegisterMetaType<QPair<QString,QString> > ("QPair<QString,QString>");
    qRegisterMetaType<QList<QPair<QString,QString> > >("QList<QPair<QString,QString> >");

    mainWindowClass mainWindow;
    mainWindow.show();

    return app.exec();
}
