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

La cible est Windows 7 SP1 ou plus récent, en 64 bits. Tout se fait en ligne de commande, Qt Creator n'est pas nécessaire.

**Le compilateur doit être MSVC, pas MinGW.** RespawnIRC utilise QtWebEngine, dont le moteur est Chromium, et Chromium ne se compile pas avec MinGW : les binaires officiels de Qt ne fournissent QtWebEngine que pour MSVC, et le module est purement et simplement absent des versions MinGW. C'est la seule contrainte forte de la compilation sous Windows, tout le reste en découle.

En revanche aucune modification des fichiers `.pro` n'est nécessaire : toute la configuration passe par deux variables données à `qmake`.

#### Les outils

Les Build Tools de Visual Studio suffisent, l'IDE complet est inutile. L'installation se fait sans interface :

    vs_BuildTools.exe --quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended

Qt 5.15.2 est la dernière version dont les binaires sont librement téléchargeables. [aqtinstall](https://github.com/miurahr/aqtinstall) les récupère sans demander de compte Qt, et publie un exécutable autonome qui évite d'installer Python :

    aqt.exe install-qt windows desktop 5.15.2 win64_msvc2019_64 -m qtwebengine --outputdir C:\Qt

`qtmultimedia` fait partie de l'installation de base, seul `qtwebengine` doit être demandé en plus.

#### Hunspell et zlib

Rien n'est fourni par le système sous Windows, il faut donc compiler les deux. Ce sont deux petites bibliothèques sans dépendance, et les compiler à la main prend une quinzaine de secondes : c'est la méthode recommandée. Récupérez les sources, [Hunspell 1.7.3](https://github.com/hunspell/hunspell/releases) et [zlib 1.3.1](https://github.com/madler/zlib/releases), décompressez-les, puis depuis une invite de commandes où `vcvars64.bat` a été exécuté :

    cd hunspell-1.7.3\src\hunspell
    cl /nologo /c /O2 /MD /EHsc /DHUNSPELL_STATIC *.cxx
    lib /nologo /OUT:hunspell.lib *.obj

    cd zlib-1.3.1
    cl /nologo /c /O2 /MD *.c
    lib /nologo /OUT:zs.lib *.obj

`/MD` est indispensable : c'est la bibliothèque C++ dynamique, celle qu'utilise Qt. Avec `/MT` l'édition de liens échouerait.

Placez ensuite le résultat à la racine du dépôt, dans la disposition attendue par les `.pro` : les cinq en-têtes de `src\hunspell` (`hunspell.hxx`, `hunspell.h`, `hunvisapi.h`, `atypes.hxx`, `w_char.hxx`) dans `hunspell\include\hunspell` et `hunspell.lib` dans `hunspell\lib` ; `zlib.h` et `zconf.h` dans `zlib\include` et `zs.lib` dans `zlib\lib`.

`HUNSPELL_STATIC` doit être défini à la compilation de Hunspell **et** à celle de RespawnIRC : sans lui les en-têtes de Hunspell déclarent tout en `__declspec(dllimport)` et l'édition de liens échoue. C'est le `DEFINES+=HUNSPELL_STATIC` des commandes plus bas.

##### Avec vcpkg à la place

[vcpkg](https://github.com/microsoft/vcpkg) fait la même chose sans avoir à savoir quels fichiers compiler, au prix d'une installation nettement plus lourde :

    vcpkg install hunspell:x64-windows-static-md zlib:x64-windows-static-md

Le triplet `x64-windows-static-md` donne des bibliothèques statiques avec la bibliothèque C++ dynamique, comme Qt ; `x64-windows-static` tout court utiliserait `/MT` et entrerait en conflit avec Qt. Les fichiers se recopient dans la même disposition que ci-dessus, à deux différences près : la bibliothèque s'appelle `hunspell-1.7.lib`, il faut donc `HUNSPELL_LIB_NAME=hunspell-1.7`, et son `hunvisapi.h` est engendré avec le test déjà figé, ce qui rend `DEFINES+=HUNSPELL_STATIC` inutile — mais inoffensif, les commandes plus bas marchent dans les deux cas.

Pour mémoire, mesuré sur une même machine : la compilation à la main demande 2,4 Mo de téléchargement, une quinzaine de secondes et 44 Mo sur le disque, contre une dizaine de minutes et 912 Mo pour vcpkg, qui télécharge au passage CMake, 7zip, PowerShell Core et un environnement MSYS2 complet. Les deux tiers de ces dix minutes vont à libiconv, une dépendance du paquet vcpkg de Hunspell dont RespawnIRC n'a pas l'usage : la bibliothèque compilée à la main s'en passe et le programme fonctionne à l'identique, tests compris. vcpkg reste intéressant si vous utilisez déjà son cache binaire, ou pour suivre les mises à jour de Hunspell — qui sort une version tous les deux à quatre ans.

Vérifiez dans tous les cas les noms de bibliothèques réellement obtenus plutôt que de recopier ceux d'ici, ils changent avec les versions : zlib ne s'appelle `zs` que depuis la 1.3.2.

#### OpenSSL

**Sans OpenSSL, le programme démarre mais ne peut joindre aucune page.** Qt 5.15.2 est compilé contre OpenSSL 1.1.1 et charge `libssl-1_1-x64.dll` et `libcrypto-1_1-x64.dll` à l'exécution pour tout ce qui est HTTPS ; en leur absence `QSslSocket::supportsSsl()` est faux et toutes les requêtes échouent, sans message clair. `windeployqt` ne les copie pas, et Qt ne les distribue plus : son dépôt ne contient plus que `tools_opensslv3_x64`, dont l'interface binaire est incompatible avec ce que Qt 5.15.2 va chercher.

Il faut donc les récupérer ailleurs et les poser dans un dossier `openssl\bin` à la racine du dépôt, à côté de `hunspell` et `zlib`. La version utilisée pour la distribution actuelle est celle de [FireDaemon](https://firedaemon.com/download-firedaemon-openssl), signée et accompagnée d'une empreinte SHA-256 à vérifier.

Attention : **OpenSSL 1.1.1 n'est plus maintenu depuis septembre 2023**. C'est un choix assumé faute d'alternative simple, Qt 5.15.2 ne sachant pas parler à OpenSSL 3. S'en affranchir demanderait de recompiler Qt depuis les sources avec `-schannel`, pour utiliser le TLS natif de Windows.

#### Compiler

La compilation se fait hors des sources, dans `build\`, contrairement à Linux et macOS : le dossier de sources reste propre et il n'y a rien à ignorer dedans. Depuis une invite de commandes où `vcvars64.bat` a été exécuté et où le `bin` de Qt est dans le `PATH` :

    mkdir build\respawnIrc
    cd build\respawnIrc
    qmake ..\..\respawnIrc\respawnIrc.pro HUNSPELL_LIB_NAME=hunspell ZLIB_LIB_NAME=zs DEFINES+=HUNSPELL_STATIC
    nmake release

Avec un Hunspell venant de vcpkg, remplacez `HUNSPELL_LIB_NAME=hunspell` par `HUNSPELL_LIB_NAME=hunspell-1.7`.

L'exécutable est dans `build\respawnIrc\release\RespawnIRC.exe`, mais **il ne peut pas être lancé depuis là**, trois choses lui manquant à cet endroit : les DLL de Qt, celles d'OpenSSL, et surtout les dossiers `resources\` et `themes\` que `pathTool::dataDirPath()` va chercher à côté de l'exécutable. Inutile pour autant de passer par l'archive de `dist-windows.ps1` : il suffit de recopier l'exécutable à la racine du dépôt, qui contient déjà ces dossiers, et d'avoir Qt et OpenSSL dans le `PATH`. C'est ce que fait `run-windows.ps1` :

    .\run-windows.ps1 -QtDir C:\Qt\5.15.2\msvc2019_64

Avec `-Logs`, il active `RESPAWNIRC_DEBUG` et le journal atterrit dans `logs\respawnirc.log` (voir plus bas).

Ne remplacez pas cette recopie par une jonction vers `resources\` depuis `build\` : `dist-windows.ps1` supprime `build\respawnIrc` récursivement, et PowerShell 5.1 suit les jonctions en supprimant — le vrai `resources\` du dépôt y passerait, stickers téléchargés compris.

Les tests se compilent de la même façon :

    mkdir build\tests
    cd build\tests
    qmake ..\..\tests\tests.pro ZLIB_LIB_NAME=zs
    nmake release
    release\respawnIrcTests.exe

#### Fabriquer une version distribuable

    .\dist-windows.ps1 -QtDir C:\Qt\5.15.2\msvc2019_64

Le script compile, appelle `windeployqt`, allège le résultat, ajoute les bibliothèques d'exécution nécessaires et fabrique `dist\RespawnIRC-<version>-windows.zip`. Sans argument, il utilise le Qt dont le `qmake` est dans le `PATH` ; il retrouve tout seul l'environnement MSVC avec `vswhere`, il n'a donc pas besoin d'être lancé depuis une invite de commandes développeur.

L'archive contient un unique dossier `RespawnIRC` avec l'application **et** les dossiers `resources` et `themes` : c'est ce dossier entier qu'il faut décompresser quelque part. L'application et ses données ne peuvent pas être séparées, parce que le programme écrit dedans — les stickers, notamment, sont téléchargés dans `resources\stickers\`. À noter que `windeployqt` crée lui aussi un dossier `resources` pour QtWebEngine : les deux contenus cohabitent dans le même dossier, aucun nom de fichier ne se chevauchant.

Trois dossiers sont allégés parce que `windeployqt` copie tout par défaut : les traductions de QtWebEngine sont réduites au français et à l'anglais qui lui sert de repli, celles de Qt au seul français, et les outils de développement de Chromium sont retirés. Cela représente une vingtaine de mégaoctets. En revanche `opengl32sw.dll` est conservé malgré ses 20 Mo : c'est le rendu OpenGL logiciel, seul recours sur une machine sans pilote OpenGL utilisable, ce qui est courant sur les vieilles configurations et les machines virtuelles visées par une cible Windows 7. L'essentiel du poids restant est incompressible, `Qt5WebEngineCore.dll` pesant à lui seul près de 100 Mo.

#### Ce que Windows 7 change

Trois choses que Windows 10 fournit et que Windows 7 n'a pas sont embarquées dans l'archive, sans quoi le programme ne démarre pas du tout sur certaines machines. C'est la raison d'être de la pile de DLL qui accompagne le programme depuis toujours, et non un excès de prudence :

- l'**Universal CRT** est un composant du système à partir de Windows 10, alors que Windows 7 ne l'obtient que par une mise à jour facultative (KB2999226). L'archive embarque donc `ucrtbase.dll` et la quarantaine de petites DLL de redirection `api-ms-win-crt-*`, prises dans `Windows Kits\10\Redist\ucrt\DLLs\x64`. Sous Windows 10 la copie du système est utilisée de toute façon, ces fichiers n'y servent à rien mais ne gênent pas. Le redistribuable exige Windows 7 **SP1**, pas la version d'origine ;
- les **bibliothèques C++ de MSVC** (`vcruntime140.dll`, `vcruntime140_1.dll`, `msvcp140.dll`), absentes de toute machine où le redistribuable n'a jamais été installé, quel que soit le Windows ;
- **`D3Dcompiler_47.dll`**, présent dans le système sous Windows 10 mais généralement pas sous Windows 7, dont Qt a besoin pour le rendu via ANGLE.

Une limite honnête : **rien de tout cela n'a été vérifié sur un vrai Windows 7**, faute d'une machine sous la main. Le contenu de l'archive suit les règles documentées par Microsoft et le programme a été lancé avec succès depuis l'archive sous Windows 10, mais la validation sous Windows 7 reste à faire.

### Linux

Pour Linux, installez les paquets `qtbase5-dev qtmultimedia5-dev libhunspell-dev qtwebengine5-dev zlib1g-dev`. Les noms des paquets sont ceux pour Debian, si vous utilisez une autre distribution ils peuvent changer.

Rendez-vous ensuite dans le dossier `respawnIrc` :

    cd respawnIrc

Et exécutez ces commandes :

    qmake
    make

### macOS

Homebrew fournit Hunspell, et zlib vient du système, mais son paquet `qt@5` est livré **sans QtWebEngine** (retiré parce que son Chromium a des failles non corrigées) alors que RespawnIRC en a besoin. Il faut donc le Qt 5.15.2 officiel, que [aqtinstall](https://github.com/miurahr/aqtinstall) récupère sans demander de compte Qt :

    brew install hunspell
    python3 -m venv ~/.aqtenv && ~/.aqtenv/bin/pip install aqtinstall
    ~/.aqtenv/bin/aqt install-qt mac desktop 5.15.2 clang_64 -m qtwebengine --outputdir ~/Qt

Ces binaires sont en x86_64 : sur un Mac Apple Silicon ils tournent via Rosetta 2. Le Chromium de Qt 5.15.2 est ancien, mieux vaut ne pas s'en servir comme navigateur généraliste.

Rendez-vous ensuite dans le dossier `respawnIrc` et compilez :

    export PATH="$HOME/Qt/5.15.2/clang_64/bin:$PATH"
    cd respawnIrc
    qmake CONFIG+=sdk_no_version_check
    make -j4

`CONFIG+=sdk_no_version_check` fait taire l'avertissement de Qt 5.15.2, qui n'a été testé qu'avec le SDK 10.15 alors que Xcode en fournit un bien plus récent.

Contrairement à Linux c'est un bundle `RespawnIRC.app` qui est produit, et non un exécutable simple : le système de fichiers de macOS ne fait pas la différence entre majuscules et minuscules, un exécutable nommé `RespawnIRC` ne peut donc pas être posé à la racine du dépôt à côté du dossier de sources `respawnIrc`. Déplacez le bundle à la racine et lancez-le, ou double-cliquez dessus depuis le Finder :

    mv RespawnIRC.app ..
    cd ..
    ./RespawnIRC.app/Contents/MacOS/RespawnIRC

Le programme cherche `resources/`, `themes/`, `config.ini` et `logs/` **à côté du bundle** et non dedans (voir `pathTool::dataDirPath`), le bundle doit donc rester à la racine du dépôt.

#### Fabriquer une version distribuable

Le bundle produit ci-dessus embarque les chemins du Qt de la machine qui l'a compilé : il n'est utilisable ailleurs qu'après un passage par `macdeployqt`, ce dont s'occupe `dist-macos.sh` :

    ./dist-macos.sh ~/Qt/5.15.2/clang_64

Le script compile, copie Qt et QtWebEngine dans le bundle, le signe, et fabrique `dist/RespawnIRC-<version>-macos.dmg` (environ 100 Mo pour un bundle de 200 Mo). Sans argument, il utilise le Qt dont le `qmake` est dans le `PATH`.

L'image disque contient un dossier `RespawnIRC` avec l'application **et** les dossiers `resources` et `themes` : c'est ce dossier entier qu'il faut glisser dans les Applications, ou n'importe où ailleurs. L'application et ses données ne peuvent pas être séparées, ni les données enfermées dans le bundle, parce que le programme écrit dedans — les stickers, notamment, sont téléchargés dans `resources/stickers/`.

Trois limites de cette distribution :

- l'application est en x86_64 seulement, parce que le Qt 5.15.2 officiel n'existe qu'en x86_64. Sur un Mac Apple Silicon elle tourne donc via Rosetta 2. Une version arm64 demanderait de compiler Qt et QtWebEngine depuis les sources, ou de passer à Qt 6 ;
- la signature est ad hoc et l'application n'est pas notarisée : au premier lancement macOS la refusera. Il faut passer par un clic droit sur l'application puis « Ouvrir », ou retirer la mise en quarantaine avec `xattr -dr com.apple.quarantine /chemin/vers/RespawnIRC`. Cette signature ad hoc est bien celle qu'exigerait Apple Silicon pour du code arm64 ; elle ne dispense pas de la notarisation, seul un certificat Developer ID en dispenserait ;
- l'icône vient du `.ico` de Windows, qui plafonne à 128 pixels, et paraît donc un peu molle dans les grands affichages du Finder.

---

Sous Linux, un fichier `RespawnIRC` devrait être créé dans le répertoire courant, déplacez-le dans la racine du projet (là où se trouvent les dossiers `resources` 
et `themes`) et exécutez-le :

    mv RespawnIRC ..
    cd ..
    ./RespawnIRC

Tout ceci en une ligne :

    cd respawnIrc; qmake; make; mv RespawnIRC ..; cd ..; ./RespawnIRC
