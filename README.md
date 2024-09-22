# RespawnIRC

Logiciel offrant une interface de client IRC pour les forums de jeuxvideo.com.

Lien de téléchargement : https://github.com/FranckRJ/RespawnIRC/releases/latest.

Message de présentation sur jeuxvideo.com : http://www.jeuxvideo.com/forums/42-1000021-40573812-1-0-1-0-pc-android-respawnirc.htm#post_731107420.

Pour plus d'infos, le site : https://pijon.fr/RespawnIRC/.

## Compilation

Il est fortement recommandé d'utiliser les sources de la branche master pour compiler RespawnIRC, vous pouvez utiliser les sources de la branche dev mais celle-ci peut être moins stable, considérez-la comme une version bêta du logiciel.  
Pour connaître les modifications à apporter au programme selon votre compilateur/version de Qt, référez-vous au wiki : https://github.com/FranckRJ/RespawnIRC/wiki/Compiler-selon-sa-configuration.  
Pour compiler RespawnIRC vous devrez d'abord compiler Hunspell, pour ce faire référez-vous au wiki : https://github.com/FranckRJ/RespawnIRC/wiki/Compiler-Hunspell.

### Windows

Pour Windows le plus simple reste de télécharger la dernière version de Qt (http://www.qt.io/download-open-source/) contenant Qt Creator, les libs Qt et un compilateur, de se rendre dans le dossier `respawnIrc` et d'ouvrir le .pro avec Qt Creator puis de cliquer sur `compiler`. Un nouveau dossier devrait être créé à la racine du projet (là où se trouvent les dossiers `resources` et `themes`) et à l'intérieur de celui-ci se trouve un dossier `debug` ou `release` (selon comment vous avez compilé) contenant le .exe, déplacez-le dans la racine du projet et exécutez-le.

### Linux

Pour Linux, installez les paquets `qt5-default qtmultimedia5-dev libhunspell-dev` ainsi que `qtwebengine5-dev` si vous utilisez WebEngine (par défaut) ou `libqt5webkit5-dev` si vous utilisez WebKit (en utilisant les fichiers .old). Les noms des paquets sont ceux pour Debian, si vous utilisez une autre distribution ils peuvent changer.

Rendez-vous ensuite dans le dossier `respawnIrc` et exécutez ces commandes :

    qmake
    make

---

Si vous obtenez l’erreur suivante :
> /usr/bin/ld: cannot find -llibhunspell: No such file or directory
> /usr/bin/ld: note to link with /usr/lib/gcc/x86_64-linux-gnu/12/../../../x86_64-linux-gnu/libhunspell.a use -l:libhunspell.a or rename it to liblibhunspell.a

Changez `-llibhunspell` par `-lhunspell` dans `respawnIrc.pro`, puis réexécutez `qmake` et `make`.

---

Un fichier `RespawnIRC` devrait être créé dans le répertoire courant, déplacez-le dans la racine du projet (là où se trouvent les dossiers `resources` et `themes`) avec `mv RespawnIRC ..` et exécutez-le.
