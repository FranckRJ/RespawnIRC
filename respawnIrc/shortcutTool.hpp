#ifndef SHORTCUTTOOL_HPP
#define SHORTCUTTOOL_HPP

#include <QMap>

class shortcutToolClass
{
public:
    static void loadShortcutRule(QString ruleName, QString beforeBase = "", QString afterBase = "", QString beforeNew = "", QString afterNew = "");
    static void transformMessage(QString* thisMessage, QString ruleName);
    static QString transformMessage(QString thisMessage, QString ruleName);
private:
    static QMap<QString, QMap<QString, QString> > listOfShortcutRules;
};

#endif
