#ifndef LOGTOOL_HPP
#define LOGTOOL_HPP

#include <QLoggingCategory>
#include <QString>

/* Catégories de log utilisées dans le programme. Elles sont toutes désactivées
 * par défaut pour le niveau debug, et activables :
 *  - en lançant le programme avec la variable d'environnement RESPAWNIRC_DEBUG=1
 *    (active tout et écrit dans un fichier de log),
 *  - ou plus finement avec QT_LOGGING_RULES, par exemple :
 *    QT_LOGGING_RULES="respawnirc.parsing.debug=true" ./RespawnIRC
 */
Q_DECLARE_LOGGING_CATEGORY(logNetwork)
Q_DECLARE_LOGGING_CATEGORY(logParsing)
Q_DECLARE_LOGGING_CATEGORY(logTopic)
Q_DECLARE_LOGGING_CATEGORY(logForum)

namespace logTool
{
    /* À appeler une fois au démarrage, avant tout autre usage. Installe le
     * handler de messages et ouvre le fichier de log si les logs y sont activés. */
    void initialize();

    bool logToFileIsEnabled();
    void setLogToFileIsEnabled(bool newValue);

    /* Chemin du fichier de log (à côté de l'exécutable), même si les logs fichier
     * sont désactivés. */
    QString logFilePath();

    /* Sauvegarde une page dont l'analyse a échoué, pour pouvoir comprendre après
     * coup ce que le site a renvoyé. Retourne le chemin du fichier créé, ou une
     * chaîne vide si rien n'a été écrit (sauvegarde désactivée ou échec).
     * `nameHint` sert à nommer le fichier, il est nettoyé avant usage. */
    QString dumpFailedPage(const QString& nameHint, const QString& pageSource);

    /* Une page ratée sur deux cents ne sert à rien : la sauvegarde n'a lieu que si
     * les logs fichier sont activés. */
    bool pageDumpIsEnabled();
}

#endif
