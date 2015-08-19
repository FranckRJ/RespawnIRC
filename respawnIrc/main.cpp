#include <QApplication>

#include "mainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    mainWindowClass mainWindow;
    mainWindow.show();

    return app.exec();
}
