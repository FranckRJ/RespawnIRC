#ifndef SHORTCUTTOOL_HPP
#define SHORTCUTTOOL_HPP

class QString;

namespace shortcutToolClass
{
    void loadShortcutRule(QString ruleName, QString beforeBase = "", QString afterBase = "", QString beforeNew = "", QString afterNew = "");
    void transformMessage(QString* thisMessage, QString ruleName);
    QString transformMessage(QString thisMessage, QString ruleName);
}

#endif
