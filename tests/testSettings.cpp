#include <QByteArray>
#include <QFile>
#include <QList>
#include <QSettings>
#include <QString>
#include <QTemporaryDir>
#include <QVariant>

#include "testTool.hpp"

namespace
{
    /* Un config.ini tel que l'écrit la 3.2.0 compilée avec Qt 5 : les caractères non ASCII y sont
     * échappés en \xNN, et une liste d'un seul élément y devient un @Variant(...). Qt 6 écrit
     * autrement, mais doit relire ce fichier-là : c'est celui qu'il trouvera chez les utilisateurs,
     * avec leurs comptes et leurs favoris. */
    const char contentWrittenByQt5[] = R"ini([General]
favoriteName0=Topic caf\xe9 \xe0 l'\xe9t\xe9
listOfConnectCookie="@Variant(\0\0\0\t\0\0\0\x1\0\0\0\f\0\0\0\xf\x63=jeton; path=/)"
listOfPseudo=MembreDeTest, AutreMembre
listOfTopicLink=@Variant(\0\0\0\t\0\0\0\x1\0\0\0\n\0\0\0\x10\0l\0i\0\x65\0n\0-\0\xe9\0t\0\xe9)
windowGeometry=@ByteArray(\x1\xd9\xd0\xcb\0\x3)
)ini";

    const QByteArray windowGeometry("\x01\xd9\xd0\xcb\x00\x03", 6);

    /* Les valeurs sont relues comme settingTool les relit. */
    void checkValues(const QString& pathOfConfig)
    {
        QSettings setting(pathOfConfig, QSettings::IniFormat);
        QList<QVariant> listOfPseudo = setting.value("listOfPseudo").toList();
        QList<QVariant> listOfTopicLink = setting.value("listOfTopicLink").toList();
        QList<QVariant> listOfConnectCookie = setting.value("listOfConnectCookie").toList();

        testTool::checkEquals("nom de favori accentué", setting.value("favoriteName0").toString(), QString("Topic café à l'été"));
        testTool::checkEquals("liste de deux pseudos", static_cast<int>(listOfPseudo.size()), 2);
        testTool::checkEquals("second pseudo", listOfPseudo.value(1).toString(), QString("AutreMembre"));
        testTool::checkEquals("liste d'un seul lien", static_cast<int>(listOfTopicLink.size()), 1);
        testTool::checkEquals("lien accentué", listOfTopicLink.value(0).toString(), QString("lien-été"));
        testTool::checkEquals("liste d'un seul cookie", static_cast<int>(listOfConnectCookie.size()), 1);
        testTool::checkEquals("cookie", QString::fromLatin1(listOfConnectCookie.value(0).toByteArray()), QString("c=jeton; path=/"));
        testTool::checkEquals("géométrie de la fenêtre", QString::fromLatin1(setting.value("windowGeometry").toByteArray().toHex()),
                              QString::fromLatin1(windowGeometry.toHex()));
    }
}

void runSettingsTests()
{
    QTemporaryDir dirForSettings;

    if(dirForSettings.isValid() == false)
    {
        testTool::reportFailure("dossier temporaire", "un dossier", "échec");
        return;
    }

    testTool::startGroup("Configuration écrite par Qt 5");

    QFile fileWrittenByQt5(dirForSettings.filePath("qt5.ini"));

    if(fileWrittenByQt5.open(QIODevice::WriteOnly) == false)
    {
        testTool::reportFailure("écriture du fichier", "un fichier", "échec d'ouverture");
        return;
    }

    fileWrittenByQt5.write(contentWrittenByQt5);
    fileWrittenByQt5.close();
    checkValues(fileWrittenByQt5.fileName());

    testTool::startGroup("Configuration écrite puis relue");

    const QString pathOfRoundTrip = dirForSettings.filePath("aller-retour.ini");

    {
        QSettings setting(pathOfRoundTrip, QSettings::IniFormat);
        setting.setValue("favoriteName0", QString("Topic café à l'été"));
        setting.setValue("listOfPseudo", QList<QVariant>{QString("MembreDeTest"), QString("AutreMembre")});
        setting.setValue("listOfTopicLink", QList<QVariant>{QString("lien-été")});
        setting.setValue("listOfConnectCookie", QList<QVariant>{QByteArray("c=jeton; path=/")});
        setting.setValue("windowGeometry", windowGeometry);
    }

    checkValues(pathOfRoundTrip);
}
