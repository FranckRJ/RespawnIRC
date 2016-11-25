#ifndef CONFIGDEPENDENTVAR_HPP
#define CONFIGDEPENDENTVAR_HPP

#include <QtGlobal>
#include <QRegularExpression>

namespace configDependentVar
{
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
    static const QRegularExpression::PatternOption regexpBaseOptions = QRegularExpression::NoPatternOption;
#else
    static const QRegularExpression::PatternOption regexpBaseOptions = QRegularExpression::OptimizeOnFirstUsageOption;
#endif
}

#endif
