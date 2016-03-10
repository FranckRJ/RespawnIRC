#ifndef STICKERTOSMILEYTOOL_HPP
#define STICKERTOSMILEYTOOL_HPP

#include <QtCore>

class stickerToSmileyToolClass
{
public:
    static void loadTransformInfo();
    static void transformMessage(QString& thisMessage);
private:
    static QMap<QString, QString> listOfTransform;
};

#endif
