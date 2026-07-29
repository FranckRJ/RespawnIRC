#ifndef PATHTOOL_HPP
#define PATHTOOL_HPP

#include <QString>
#include <QStringList>

/* Le programme range ses données à côté de son exécutable, en deux dossiers bien séparés :
 *
 * - ce qui est livré avec le programme (resources/, themes/) n'est jamais écrit ;
 * - tout ce que le programme écrit va dans userdata/, qui reproduit la même disposition :
 *   userdata/config.ini, userdata/logs/, userdata/resources/shortcut.txt,
 *   userdata/resources/stickers/...
 *
 * D'où pathForReading(), qui regarde d'abord dans userdata/ puis se rabat sur ce qui est livré,
 * et pathForWriting(), qui vise toujours userdata/. Cette séparation permet à dist-windows.ps1 et
 * dist-macos.sh de n'embarquer jamais les données de l'utilisateur, tout en gardant le programme
 * portable : rien ne sort de son dossier, et déplacer le dossier emporte la configuration avec lui.
 *
 * Sous macOS l'exécutable est dans un bundle RespawnIRC.app et les données restent à côté du
 * bundle, pas dans Contents/MacOS : c'est la seule disposition possible, un exécutable simple
 * nommé RespawnIRC ne pouvant pas être posé à la racine du dépôt là où le système de fichiers ne
 * distingue pas `RespawnIRC` du dossier de sources `respawnIrc`.
 */
namespace pathTool
{
    /* Dossier des données livrées avec le programme, à utiliser à la place de applicationDirPath. */
    QString dataDirPath();
    /* Dossier de tout ce que le programme écrit. */
    QString userDataDirPath();
    /* Les deux dossiers ci-dessus, dans l'ordre où il faut les consulter pour lire. */
    QStringList dirPathsForReading();
    /* Où lire relativePath : la version de userdata/ si elle existe, sinon celle livrée. */
    QString pathForReading(const QString& relativePath);
    /* Où écrire relativePath dans userdata/, dossiers parents créés au passage. */
    QString pathForWriting(const QString& relativePath);
    /* Déplace dans userdata/ ce que les versions antérieures laissaient à côté de l'exécutable. */
    void migrateOldUserDataIfNeeded();
}

#endif
