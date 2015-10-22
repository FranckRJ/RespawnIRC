#include <QApplication>

#include "mainWindow.hpp"
#include "settingTool.hpp"
#include "styleTool.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    settingToolClass::initializeDefaultListsOption();
    styleToolClass::getModelInfo("");

    mainWindowClass mainWindow;
    mainWindow.show();

    return app.exec();
}
