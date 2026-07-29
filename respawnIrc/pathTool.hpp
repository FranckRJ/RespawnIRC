#ifndef PATHTOOL_HPP
#define PATHTOOL_HPP

#include <QString>
#include <QStringList>

/* Les données livrées avec le programme (resources/, themes/) sont toujours à côté de l'exécutable
 * et ne sont jamais écrites. Ce que le programme écrit, lui, va là où le système l'attend, et se
 * répartit en trois rôles :
 *
 * - la configuration : config.ini ;
 * - les données de l'utilisateur : dictionnaire personnel, raccourcis ;
 * - le cache : stickers téléchargés, et les logs, tout aussi jetables.
 *
 * Sous Windows les trois se retrouvent dans un unique userdata/ à côté de l'exécutable : le
 * programme y est distribué en archive à décompresser où l'on veut, il reste donc portable, tout
 * tient dans son dossier et le déplacer emporte la configuration. Ailleurs on suit la convention du
 * système via QStandardPaths — ~/.config, ~/.local/share et ~/.cache sous Linux,
 * ~/Library/Application Support et ~/Library/Caches sous macOS.
 *
 * Les dossiers d'écriture reproduisent la disposition des données livrées
 * (resources/shortcut.txt, resources/stickers/...) : les mêmes chemins relatifs résolvent des deux
 * côtés, ce qui permet à pathForReading de se rabattre de l'un sur l'autre sans rien changer au
 * HTML des messages.
 *
 * Sous macOS l'exécutable est dans un bundle RespawnIRC.app et les données livrées restent à côté
 * du bundle, pas dans Contents/MacOS : c'est la seule disposition possible, un exécutable simple
 * nommé RespawnIRC ne pouvant pas être posé à la racine du dépôt là où le système de fichiers ne
 * distingue pas `RespawnIRC` du dossier de sources `respawnIrc`.
 */
namespace pathTool
{
    /* Dossier des données livrées avec le programme, à utiliser à la place de applicationDirPath. */
    QString dataDirPath();
    /* Dossier de la configuration, et chemin complet de config.ini. */
    QString configDirPath();
    QString configFilePath();
    /* Dossier des données écrites par l'utilisateur : dictionnaire personnel, raccourcis. */
    QString userDataDirPath();
    /* Dossier du cache : stickers téléchargés, qu'un retéléchargement suffit à refabriquer. */
    QString cacheDirPath();
    /* Dossier des logs et des pages sauvegardées, dans le cache car tout aussi jetables. */
    QString logDirPath();
    /* Tous les dossiers où lire, dans l'ordre où il faut les consulter et sans doublon. */
    QStringList dirPathsForReading();
    /* Où lire relativePath : la version écrite si elle existe, sinon celle livrée. */
    QString pathForReading(const QString& relativePath);
    /* Où écrire relativePath parmi les données de l'utilisateur, dossiers parents créés au passage. */
    QString pathForWriting(const QString& relativePath);
    /* Déplace ce que les versions antérieures laissaient à côté de l'exécutable. */
    void migrateOldUserDataIfNeeded();
}

#endif
