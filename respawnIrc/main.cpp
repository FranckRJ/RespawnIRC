#include <QApplication>

#include "mainWindow.hpp"
#include "settingTool.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    settingToolClass::initializeDefaultListsOption();

    mainWindowClass mainWindow;
    mainWindow.show();

    return app.exec();
}
