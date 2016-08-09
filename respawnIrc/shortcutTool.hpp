#ifndef SHORTCUTTOOL_HPP
#define SHORTCUTTOOL_HPP

#include <QMap>

namespace shortcutToolClass
{
    void loadShortcutRule(QString ruleName, QString beforeBase = "", QString afterBase = "", QString beforeNew = "", QString afterNew = "");
    void transformMessage(QString* thisMessage, QString ruleName);
    QString transformMessage(QString thisMessage, QString ruleName);
}

#endif
