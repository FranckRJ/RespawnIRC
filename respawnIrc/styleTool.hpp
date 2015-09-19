#ifndef STYLETOOL_HPP
#define STYLETOOL_HPP

#include <QtCore>

class styleToolClass
{
public:
    static void loadStyles();
    static QString getStyle(QString styleName);
private:
    static QMap<QString, QString> listOfStyle;
};

#endif // STYLETOOL_HPP
