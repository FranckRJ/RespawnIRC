#ifndef SHORTCUTTOOL_HPP
#define SHORTCUTTOOL_HPP

#include <QtCore>

class shortcutToolClass
{
public:
    static void loadShortcuts();
    static QString transformMessage(QString thisMessage);
private:
    static QMap<QString, QString> listOfShortcut;
};

#endif
