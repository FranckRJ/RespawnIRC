#ifndef STYLETOOL_HPP
#define STYLETOOL_HPP

#include <QString>

struct modelInfoStruct
{
    QString quoteModel;
    QString blacklistModel;
    QString editModel;
    QString deleteModel;
    QString normalDateColor;
    QString editDateColor;
    QString normalPseudoColor;
    QString userPseudoColor;
    QString modoPseudoColor;
    QString adminPseudoColor;
    QString pemtDateColor;
};

struct colorInfoForMessageAndOtherStruct
{
    QString linkColor;
    QString spoilColor;
    QString tableBorderColor;
    QString underlineColor;
    QString tableBackgroundColor;
    QString tableTextColor;
    QString editMessageColor;
};

class styleToolClass
{
public:
    static QString getStyle(QString themeName);
    static QString getModel(QString themeName);
    static modelInfoStruct getModelInfo(QString themeName);
    static const colorInfoForMessageAndOtherStruct& getColorInfo();
private:
    static colorInfoForMessageAndOtherStruct colorInfoForMessageAndOther;
};

#endif // STYLETOOL_HPP
