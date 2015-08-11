#include <QApplication>

#include "mainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    mainWindowClass mainWindow;
    mainWindow.show();

    return app.exec();
}
