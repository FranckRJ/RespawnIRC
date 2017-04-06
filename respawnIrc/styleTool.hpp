#ifndef STYLETOOL_HPP
#define STYLETOOL_HPP

#include <QString>

struct modelInfoStruct
{
    QString quoteModel;
    QString blacklistModel;
    QString editModel;
    QString deleteModel;
    QString signatureModel;
    QString avatarModel;
    QString updateMessageForEditModel;
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

namespace styleTool
{
    QString getStyle(QString themeName);
    QString getModel(QString themeName);
    modelInfoStruct getModelInfo(QString themeName);
    const colorInfoForMessageAndOtherStruct& getColorInfo();
    QString getImagePathOfThemeIfExist(QString themeName);
    void loadThemeFont(QString themeName);
}

#endif
