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

En plus de Hunspell, RespawnIRC a besoin de zlib pour décompresser les pages de jeuxvideo.com. La plupart des toolchains MinGW, dont celle livrée avec Qt, en contiennent déjà une : dans ce cas il n'y a rien à faire. Sinon (avec MSVC notamment), compilez zlib et placez-la dans un dossier `zlib` à la racine du dépôt, à côté de `hunspell`, avec les en-têtes dans `zlib\include` et la bibliothèque dans `zlib\lib`. Si la bibliothèque obtenue ne porte pas le nom attendu (`zlib` avec MSVC, `z` ailleurs), son nom peut être précisé au moment de la configuration, par exemple `qmake ZLIB_LIB_NAME=zlibstatic`.

### Linux

Pour Linux, installez les paquets `qtbase5-dev qtmultimedia5-dev libhunspell-dev qtwebengine5-dev zlib1g-dev`. Les noms des paquets sont ceux pour Debian, si vous utilisez une autre distribution ils peuvent changer.

Rendez-vous ensuite dans le dossier `respawnIrc` :

    cd respawnIrc

Et exécutez ces commandes :

    qmake
    make

---

Un fichier `RespawnIRC` devrait être créé dans le répertoire courant, déplacez-le dans la racine du projet (là où se trouvent les dossiers `resources` 
et `themes`) et exécutez-le :

    mv RespawnIRC ..
    cd ..
    ./RespawnIRC

Tout ceci en une ligne :

    cd respawnIrc; qmake; make; mv RespawnIRC ..; cd ..; ./RespawnIRC
