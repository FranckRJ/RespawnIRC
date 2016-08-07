# RespawnIRC

JVIrc-like pour respawn.

Pour plus d'infos, le site : http://pijon.fr/RespawnIRC/

## Compilation

Il est fortement recommandé d'utiliser les sources de la dernière release (https://github.com/LEpigeon888/RespawnIRC/releases/latest dans la partie download en bas) plutôt que les sources de la branche master pour compiler RespawnIRC.  
Pour connaître les modifications à apporter au programme selon votre compilateur/version de Qt, référez-vous au wiki : https://github.com/LEpigeon888/RespawnIRC/wiki/Compiler-avec-les-anciennes-versions-de-Qt-5

### Windows

Pour Windows le plus simple reste de télécharger la dernière version de Qt (http://www.qt.io/download-open-source/) contenant Qt Creator, les libs Qt et un compilateur, de se rendre dans le dossier `respawnIrc` et d'ouvrir le .pro avec Qt Creator puis de cliquer sur `compiler`. Un nouveau dossier devrait être créé à la racine du projet (là où se trouvent les dossiers `resources` et `themes`) et à l'intérieur de celui-ci se trouve un dossier `debug` ou `release` (selon comment vous avez compilé) contenant le .exe, déplacez-le dans la racine du projet et exécutez-le.

### Linux

Pour Linux, il vous suffit d'installer les paquets `qt5-default qtmultimedia5-dev libqt5webkit5-dev`. Ensuite, vous n'avez plus qu'à vous rendre dans le dossier `respawnIrc` qui contient le .pro et exécuter ces commandes :

    qmake
    make

Un fichier `respawnIrc` devrait être créé dans le répertoire courant, déplacez-le dans la racine du projet (là où se trouvent les dossiers `resources` et `themes`) et exécutez-le.