#ifndef STYLETOOL_HPP
#define STYLETOOL_HPP

#include <QtCore>

struct modelInfoStruct
{
    QString quoteModel;
    QString blacklistModel;
    QString editModel;
    QString normalDateColor;
    QString editDateColor;
    QString normalPseudoColor;
    QString userPseudoColor;
};

struct colorInfoForMessageStruct
{
    QString linkColor;
    QString spoilColor;
    QString tableBorderColor;
};

class styleToolClass
{
public:
    static QString getStyle(QString themeName);
    static QString getModel(QString themeName);
    static modelInfoStruct getModelInfo(QString themeName);
    static const colorInfoForMessageStruct& getColorInfo();
private:
    static colorInfoForMessageStruct colorInfoForMessage;
};

#endif // STYLETOOL_HPP
