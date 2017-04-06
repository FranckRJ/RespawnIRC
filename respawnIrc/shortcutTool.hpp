#ifndef SHORTCUTTOOL_HPP
#define SHORTCUTTOOL_HPP

#include <QString>
#include <QRegularExpression>
#include <QList>

struct shortcutInfosStruct
{
    QString base;
    QRegularExpression regexBase;
    QString replacement;
};

struct shortcutRuleStruct
{
    QList<shortcutInfosStruct> listOfShortcuts;
    bool needToUseRegex = false;
};

namespace shortcutToolClass
{
    void initializeAllShortcutsRules();
    void loadShortcutRule(QString ruleName, QString beforeBase = "", QString afterBase = "", QString beforeNew = "", QString afterNew = "", bool useRegex = false);
    void setThisShortcutRule(QString ruleName, const shortcutRuleStruct& newShortcutRule);
    void transformMessage(QString* thisMessage, QString ruleName);
    QString transformMessage(QString thisMessage, QString ruleName);
    const QList<shortcutInfosStruct>* getListOfShortcutsForRule(QString ruleName);
}

#endif
