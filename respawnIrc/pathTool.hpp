#ifndef PATHTOOL_HPP
#define PATHTOOL_HPP

#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <QtGlobal>

/* Le programme range ses données (resources/, themes/, config.ini, logs/) à côté de son
 * exécutable. Sous macOS l'exécutable est dans un bundle RespawnIRC.app et les données restent
 * à côté du bundle, pas dans Contents/MacOS : c'est la seule disposition possible, un exécutable
 * simple nommé RespawnIRC ne pouvant pas être posé à la racine du dépôt là où le système de
 * fichiers ne distingue pas `RespawnIRC` du dossier de sources `respawnIrc`.
 */
namespace pathTool
{
    /* Dossier des données du programme, à utiliser à la place de applicationDirPath. */
    inline QString dataDirPath()
    {
#ifdef Q_OS_MACOS
        QDir dirOfApp(QCoreApplication::applicationDirPath());

        if(dirOfApp.absolutePath().endsWith(".app/Contents/MacOS") == true)
        {
            dirOfApp.cdUp(); /* Contents */
            dirOfApp.cdUp(); /* RespawnIRC.app */
            dirOfApp.cdUp(); /* dossier contenant le bundle */
            return dirOfApp.absolutePath();
        }
#endif

        return QCoreApplication::applicationDirPath();
    }
}

#endif
