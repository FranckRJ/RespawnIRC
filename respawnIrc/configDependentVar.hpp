#ifndef CONFIGDEPENDENTVAR_HPP
#define CONFIGDEPENDENTVAR_HPP

#include <QtGlobal>
#include <QRegularExpression>
#include <QString>

namespace configDependentVar
{
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
    static const QRegularExpression::PatternOption regexpBaseOptions = QRegularExpression::NoPatternOption;
#else
    static const QRegularExpression::PatternOption regexpBaseOptions = QRegularExpression::OptimizeOnFirstUsageOption;
#endif

    /* Ce qui sépare deux mots pour le correcteur orthographique. Sans UseUnicodePropertiesOption,
     * \w et \b ne reconnaissent que l'ASCII et « café » serait coupé en deux. */
    static const QString expForWordSeparatorPattern = QStringLiteral(R"rgx([^\w'-])rgx");
    static const QRegularExpression::PatternOptions expForWordSeparatorOptions = QRegularExpression::UseUnicodePropertiesOption;
}

#endif
