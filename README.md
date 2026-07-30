# RespawnIRC

Logiciel offrant une interface de client IRC pour les forums de jeuxvideo.com.

Lien de téléchargement : https://github.com/FranckRJ/RespawnIRC/releases/latest.

Message de présentation sur jeuxvideo.com : http://www.jeuxvideo.com/forums/42-1000021-40573812-1-0-1-0-pc-android-respawnirc.htm#post_731107420.

Pour plus d'infos, le site : https://pijon.fr/RespawnIRC/.

## Compilation

Il est fortement recommandé d'utiliser les sources de la branche master pour compiler RespawnIRC, vous pouvez utiliser les sources de la branche dev mais celle-ci peut être moins stable, considérez-la comme une version bêta du logiciel.  
Pour connaître les modifications à apporter au programme selon votre compilateur/version de Qt, référez-vous au wiki : https://github.com/FranckRJ/RespawnIRC/wiki/Compiler-selon-sa-configuration.  
Pour compiler RespawnIRC vous devrez d'abord compiler Hunspell, pour ce faire référez-vous au wiki : https://github.com/FranckRJ/RespawnIRC/wiki/Compiler-Hunspell.

Ces deux liens vers le wiki sont **antérieurs** aux sections par plateforme qui suivent, et n'ont pas été revérifiés depuis : en cas de désaccord, ce sont les sections ci-dessous qui font foi.

### Windows

La cible est Windows 10 ou plus récent, en 64 bits. Tout se fait en ligne de commande, Qt Creator n'est pas nécessaire.

**Le compilateur doit être MSVC, pas MinGW.** RespawnIRC utilise QtWebEngine, dont le moteur est Chromium, et Chromium ne se compile pas avec MinGW : les binaires officiels de Qt ne fournissent QtWebEngine que pour MSVC, et le module est purement et simplement absent des versions MinGW. C'est la seule contrainte forte de la compilation sous Windows, tout le reste en découle.

En revanche aucune modification des fichiers `.pro` n'est nécessaire : toute la configuration passe par deux variables données à `qmake`.

#### Tout installer d'un coup

Sur une machine vierge, `bootstrap-windows.ps1` fait tout ce que décrivent les sections suivantes. Un PowerShell **ordinaire** suffit, à la racine du dépôt :

    powershell -ExecutionPolicy Bypass -File .\bootstrap-windows.ps1

`-ExecutionPolicy Bypass` n'est pas une précaution de style : la stratégie d'exécution par défaut d'un Windows 10 est `Restricted`, et un `.\bootstrap-windows.ps1` lancé tel quel sur une machine neuve échoue avant d'afficher quoi que ce soit — vérifié, il est refusé par un `UnauthorizedAccess`. Si vous cherchez à le constater vous-même, sachez que c'est étonnamment difficile à voir : `Get-ExecutionPolicy -List` affiche `Undefined` partout et jamais le mot `Restricted`, qui n'est que le défaut implicite, et un `Bypass` de portée `Process` se transmet aux processus enfants par la variable d'environnement `PSExecutionPolicyPreference` — relancer un `powershell.exe` neuf depuis un shell déjà en `Bypass` ne montre donc rien. Il faut vider cette variable.

**Le dépôt doit être récupéré avec `git clone`**, et git est le seul prérequis que le script n'installe pas — il sert d'ailleurs à le récupérer. Une archive zip téléchargée depuis GitHub ne suffit pas : `dist-windows.ps1` extrait `resources/` et `themes/` avec `git archive HEAD`, et une archive décompressée n'est pas un dépôt. L'échec arriverait tard, après une compilation complète. Sur un zip il faudrait de plus lever la marque de provenance que Windows y met, sans quoi le script est bloqué même avec `Bypass` (`Unblock-File .\bootstrap-windows.ps1`) — mais seuls la compilation et `run-windows.ps1` fonctionneraient ensuite, pas la fabrication de l'archive.

Il installe les Build Tools, Qt 5.15.2 avec QtWebEngine, compile Hunspell et zlib, récupère OpenSSL en vérifiant son empreinte SHA-256, et pose le tout dans la disposition attendue par les `.pro`. Compter une trentaine de minutes et environ 4,5 Go, presque entièrement pour les Build Tools (3,3 Go) et Qt (0,9 Go).

Les cinq étapes ont maintenant tourné, l'installation des Build Tools comprise : elle a été exécutée par le script sur une machine virtuelle vierge, sans MSVC ni Qt, suivie de la compilation du programme, des tests et de la fabrication de l'archive. Elle avait longtemps été la seule branche jamais empruntée, faute d'une machine où désinstaller les Build Tools pour réessayer. Ce qui reste supposé et non constaté, c'est le seul traitement du code de retour 3010 — redémarrage conseillé — comme un succès : les deux installations observées ont rendu 0.

Seule l'installation des Build Tools a besoin des droits d'administrateur, et le script **élève cet installateur-là par une invite UAC** au lieu de réclamer d'être lancé élevé. L'invite n'apparaît que si les Build Tools manquent vraiment : sur une machine qui les a déjà, l'étape se saute sans rien demander. Un PowerShell administrateur reste accepté et ne fait alors apparaître aucune invite. Les quatre autres étapes n'écrivent que dans le dépôt et dans `C:\Qt`, et n'ont jamais besoin d'élévation — c'est aussi pourquoi seul l'installateur est élevé : ce que le script écrit ensuite appartient à l'utilisateur courant, et non à l'administrateur.

Cette élévation a maintenant été essayée pour de bon, depuis un PowerShell ordinaire sur une machine vierge : l'invite UAC est apparue, elle a été acceptée, l'installateur élevé a rendu 0, et les quatre étapes suivantes ont continué sans élévation dans le processus d'origine. Le résultat est celui recherché — `C:\Qt` et tout ce que le script écrit dans le dépôt appartiennent à l'utilisateur courant, seule l'installation des Build Tools appartient aux administrateurs. Reste non constaté le seul cas du refus de l'invite, qui demanderait une machine où désinstaller les Build Tools pour refuser volontairement.

À savoir avant d'essayer d'enchaîner l'amorçage et l'essai de l'archive sur la même machine : **installer les Build Tools pose `msvcp140.dll` et toute sa famille dans `System32`**. La machine amorcée n'est donc plus un témoin valable pour vérifier qu'une archive embarque bien tout ce qu'il lui faut — le chargeur y trouvera dans `System32` ce que l'archive aurait oublié. C'est précisément ce qui a laissé sortir une archive incomplète (voir « Les bibliothèques d'exécution »).

Le script est réentrant, chaque étape étant sautée si son résultat est déjà là — après un échec, on le relance et il reprend où il en était.

Les sections qui suivent décrivent les mêmes étapes à la main, et restent la référence : ce sont elles qu'il faut lire quand quelque chose ne se passe pas comme prévu, ou pour adapter une version.

#### Les outils

Les Build Tools de Visual Studio suffisent, l'IDE complet est inutile. L'installation se fait sans interface :

    vs_BuildTools.exe --quiet --wait --norestart --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows11SDK.26100

Cette installation ciblée pèse **3,3 Go** (1,7 Go de Build Tools et 1,7 Go de Windows SDK) et n'installe ni WebView2 ni Microsoft Edge. Elle a été vérifiée de bout en bout sur une machine sans MSVC ni Qt : compilation du programme, des tests, et fabrication de l'archive. Le numéro du SDK est à adapter, c'est celui qui était courant au moment où ces lignes ont été écrites.

Les **deux** composants sont nécessaires. `VC.Tools.x86.x64` seul pose bien `cl.exe` mais aucun `Windows Kits`, et rien ne compile : depuis Visual Studio 2015 les en-têtes de la bibliothèque C standard appartiennent au Windows SDK, pas au compilateur, et un simple `#include <stdio.h>` échoue.

La variante historique reste valable :

    vs_BuildTools.exe --quiet --wait --norestart --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended

Elle installe la même chose plus WebView2 et Microsoft Edge, dont la compilation de RespawnIRC n'a aucun besoin, pour environ 5 Go. Il n'y a pas de raison de la préférer.

Qt 5.15.2 est la dernière version dont les binaires sont librement téléchargeables. [aqtinstall](https://github.com/miurahr/aqtinstall) les récupère sans demander de compte Qt, et publie un exécutable autonome qui évite d'installer Python :

    aqt.exe install-qt windows desktop 5.15.2 win64_msvc2019_64 -m qtwebengine --outputdir C:\Qt

`qtmultimedia` fait partie de l'installation de base, seul `qtwebengine` doit être demandé en plus.

#### Hunspell et zlib

Rien n'est fourni par le système sous Windows, il faut donc compiler les deux. Ce sont deux petites bibliothèques sans dépendance, et les compiler à la main prend une quinzaine de secondes (12 s mesurées au dernier essai) : c'est la méthode recommandée. Récupérez les sources, [Hunspell 1.7.3](https://github.com/hunspell/hunspell/releases) et [zlib 1.3.1](https://github.com/madler/zlib/releases), décompressez-les, puis depuis une invite de commandes où `vcvars64.bat` a été exécuté :

    cd hunspell-1.7.3\src\hunspell
    cl /nologo /c /O2 /MD /EHsc /DHUNSPELL_STATIC *.cxx
    lib /nologo /OUT:hunspell.lib *.obj

    cd zlib-1.3.1
    cl /nologo /c /O2 /MD *.c
    lib /nologo /OUT:zs.lib *.obj

`/MD` est indispensable : c'est la bibliothèque C++ dynamique, celle qu'utilise Qt. Avec `/MT` l'édition de liens échouerait.

Cette recette ne produit qu'une bibliothèque *release*. Un `nmake debug` de RespawnIRC échouera donc en `LNK2038`, sur un désaccord de `RuntimeLibrary` et de `_ITERATOR_DEBUG_LEVEL` : il faudrait recompiler Hunspell une seconde fois avec `/MDd`, dans un `hunspelld.lib` à part. Rien n'en a eu besoin jusqu'ici, la distribution ne se faisant qu'en release.

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

Seuls les fichiers intermédiaires restent dans `build\` : l'exécutable, lui, est produit à la racine du dépôt, là où sont déjà `resources\` et `themes\` que `pathTool::dataDirPath()` va chercher à côté de lui. Il lui manque encore les DLL de Qt et celles d'OpenSSL, sans quoi il ne démarre pas ; inutile pour autant de passer par l'archive de `dist-windows.ps1`, il suffit de les avoir dans le `PATH`. C'est ce que fait `run-windows.ps1` :

    .\run-windows.ps1 -QtDir C:\Qt\5.15.2\msvc2019_64

Avec `-Logs`, il active `RESPAWNIRC_DEBUG` et le journal atterrit dans `userdata\logs\respawnirc.log` (voir plus bas).

Les tests se compilent de la même façon :

    mkdir build\tests
    cd build\tests
    qmake ..\..\tests\tests.pro ZLIB_LIB_NAME=zs
    nmake release
    ..\respawnIrcTests.exe

#### Fabriquer une version distribuable

    .\dist-windows.ps1 -QtDir C:\Qt\5.15.2\msvc2019_64

Le script compile, appelle `windeployqt`, allège le résultat, ajoute les bibliothèques d'exécution nécessaires et fabrique `dist\RespawnIRC-<version>-windows.zip`. Sans argument, il utilise le Qt dont le `qmake` est dans le `PATH` ; il retrouve tout seul l'environnement MSVC avec `vswhere`, il n'a donc pas besoin d'être lancé depuis une invite de commandes développeur.

L'archive contient un unique dossier `RespawnIRC` avec l'application **et** les dossiers `resources` et `themes` : c'est ce dossier entier qu'il faut décompresser quelque part. Ces deux dossiers ne sont jamais modifiés par le programme, qui écrit tout dans un `userdata` créé à côté de l'exécutable — l'ensemble reste donc portable et se déplace d'un bloc. À noter que `windeployqt` crée lui aussi un dossier `resources` pour QtWebEngine : les deux contenus cohabitent dans le même dossier, aucun nom de fichier ne se chevauchant.

Trois dossiers sont allégés parce que `windeployqt` copie tout par défaut : les traductions de QtWebEngine sont réduites au français et à l'anglais qui lui sert de repli, celles de Qt au seul français, et les outils de développement de Chromium sont retirés. Cela représente une vingtaine de mégaoctets. S'y ajoute `--no-compiler-runtime`, qui évite les 24 Mo de `vc_redist.x64.exe` : `windeployqt` l'embarque dès que `VCINSTALLDIR` est définie, alors que rien ne le lance jamais et que les DLL du runtime sont déjà copiées une à une. S'y ajoute enfin `opengl32sw.dll` et ses 20 Mo, longtemps conservé pour une raison qui s'est révélée fausse : voir plus bas. L'essentiel du poids restant est incompressible, `Qt5WebEngineCore.dll` pesant à lui seul près de 100 Mo.

#### Les bibliothèques d'exécution

Une seule chose manque encore à une machine vierge, et l'archive l'embarque : les **bibliothèques C++ de MSVC**. Elles ne font partie d'aucun Windows, elles arrivent avec le redistribuable Visual C++, et sans elles le programme ne démarre pas du tout. C'est facile à vérifier sur une machine neuve : `ucrtbase.dll` est bien dans `System32`, `msvcp140.dll` et `vcruntime140.dll` n'y sont pas. Un piège en le faisant : `System32` contient un `msvcp140_clr0400.dll` et un `vcruntime140_clr0400.dll`, copies privées du .NET Framework sans usage ici — un `dir msvcp140*` trouve donc quelque chose et peut faire conclure l'inverse.

Le script copie **tout le dossier `Microsoft.VC*.CRT`** du redistribuable, soit dix DLL pour 1,8 Mo, et non une liste de noms choisis. C'est une correction, pas un choix de départ : il embarquait `vcruntime140.dll`, `vcruntime140_1.dll` et `msvcp140.dll`, et il manquait **`msvcp140_1.dll`**, que `Qt5Core.dll` et `Qt5Widgets.dll` importent. Toute archive produite avant cette correction échoue donc au lancement sur une machine sans redistribuable, avec « The code execution cannot proceed because MSVCP140_1.dll was not found » — constaté sur un Windows 10 LTSC 2019 propre. La dépendance vient des binaires précompilés de Qt 5.15.2 et non de la compilation faite ici : elle ne dépend pas de la version des Build Tools, et elle a toujours été là.

Depuis, le script **vérifie avant de compresser** qu'aucun binaire de l'archive ne réclame une DLL du runtime C++ absente de l'archive, en relevant les imports au `dumpbin`. Ce contrôle ne remplace pas un essai sur une vraie machine sans redistribuable — il ne voit pas ce qui serait chargé par `LoadLibrary`, comme OpenSSL — mais il rend impossible la répétition exacte de cette panne.

S'y ajoute OpenSSL, traité dans sa propre section plus haut, pour une raison sans rapport avec la version de Windows : Qt le charge à l'exécution et ne le distribue plus.

Le reste de la pile de DLL qui accompagnait historiquement le programme n'existait que pour **Windows 7, qui n'est plus une cible** :

- l'**Universal CRT** (`ucrtbase.dll` et la quarantaine de `api-ms-win-crt-*` et `api-ms-win-core-*`) est un composant du système depuis Windows 10, alors que Windows 7 ne l'obtenait que par la mise à jour facultative KB2999226. Les DLL de redirection n'ont même pas d'existence en tant que fichiers sous Windows 10 : ces noms sont résolus par le schéma d'*API sets* du noyau, ce qui explique qu'on ne les trouve pas dans `System32` sans que rien ne manque pour autant ;
- **`D3Dcompiler_47.dll`**, dont Qt a besoin pour le rendu via ANGLE, fait partie de Windows 10 et n'était embarqué que parce que Windows 7 ne l'a généralement pas. `windeployqt` le copie encore avec le lot ANGLE, `dist-windows.ps1` le retire ensuite.

Ces deux suppressions retirent une quarantaine de fichiers et quelques mégaoctets de l'archive — 47 fichiers et 6,4 Mo relevés avec le SDK 10.0.26100, mais le nombre de DLL de l'Universal CRT varie avec la version du SDK et ce chiffre n'est pas à figer. Le gain de poids est modeste ; le vrai gain est ailleurs. Le redistribuable de l'Universal CRT n'était pas au même endroit selon la version du SDK (`Redist\<version>\ucrt\DLLs\x64` pour les récents, `Redist\ucrt\DLLs\x64` pour les anciens), et cette recherche à deux dispositions, avec l'échec franc qui la protégeait, était la partie la plus fragile du script : elle avait déjà coûté une archive silencieusement incomplète. Elle n'a plus de raison d'être.

#### `opengl32sw.dll`, et pourquoi il ne fait plus partie de l'archive

Ce n'est pas une pièce Windows 7 : il est parti pour une raison à lui, et c'est le plus gros retrait de l'archive, 20 Mo sur 178.

Le raisonnement qui l'a longtemps gardé était : sans pilote OpenGL utilisable, Windows ne fournit que le « GDI Generic » en version 1.1, très en deçà de ce que Qt demande, donc `opengl32sw.dll` est le seul recours. La première moitié est exacte, la conclusion ne suit pas. Le comportement par défaut de Qt bascule sur **ANGLE**, qui ne fait pas d'OpenGL du tout : il traduit en Direct3D 11, et en l'absence de GPU utilisable Direct3D se rabat sur **WARP**, le rasteriseur logiciel livré avec Windows. Le repli logiciel existe donc déjà dans le système, une couche plus bas.

Mesuré sur une machine virtuelle sans la moindre accélération graphique (« Microsoft Basic Display Adapter », aucun ICD OpenGL enregistré) :

| `QT_OPENGL` | `GL_RENDERER` obtenu |
| --- | --- |
| *(défaut)* | `ANGLE (Microsoft Basic Render Driver Direct3D11 vs_5_0 ps_5_0)` |
| `desktop` | `GDI Generic`, OpenGL 1.1 — inutilisable |
| `angle` | `ANGLE (Microsoft Basic Render Driver Direct3D11 vs_5_0 ps_5_0)` |
| `software` | `Gallium 0.4 on llvmpipe`, Mesa 12 — c'est `opengl32sw.dll` |

Sur cette même machine, l'archive privée d'`opengl32sw.dll` affiche correctement une page dans QtWebEngine, en mode par défaut comme en mode `angle`. Le fichier n'est donc **pas** le recours des machines sans pilote : ANGLE et WARP le sont.

L'historique des versions publiées dit la même chose. Les releases **v3.1.6 à v3.1.10**, de 2018 à mars 2019, ont été distribuées avec QtWebEngine et ANGLE mais **sans aucun rendu OpenGL logiciel**, pendant environ un an et demi, sans problème signalé. `opengl32sw.dll` est apparu en v3.1.11 en même temps que `vc_redist.x86.exe` et qu'un changement de version de Qt : deux fichiers que `windeployqt` ajoute de lui-même, jamais une réponse à une panne.

Il ne reste utile que si ANGLE lui-même échoue, ou si quelqu'un force `QT_OPENGL=software` — auquel cas, sans lui, Qt affiche « Failed to create OpenGL context » et le programme s'arrête. Ce dernier cas ne peut venir que d'une variable d'environnement posée à la main, le programme ne la définit jamais.

**Il est donc retiré de l'archive**, comme `D3Dcompiler_47.dll`, après le passage de `windeployqt` qui continue de le copier. La réserve honnête : WARP est un composant de Windows, mais rien ne prouve qu'aucune machine cible n'a un Direct3D 11 cassé ou désactivé, et la panne serait alors totale. Le remettre est une ligne à supprimer dans `dist-windows.ps1`.

### Linux

Pour Linux, installez les paquets `qtbase5-dev qtmultimedia5-dev libhunspell-dev qtwebengine5-dev zlib1g-dev`. Les noms des paquets sont ceux pour Debian, si vous utilisez une autre distribution ils peuvent changer.

Rendez-vous ensuite dans le dossier `respawnIrc` :

    cd respawnIrc

Et exécutez ces commandes :

    qmake
    make

L'exécutable `RespawnIRC` est produit à la racine du dépôt, là où sont `resources/` et `themes/` qu'il va chercher à côté de lui. Il n'y a rien à déplacer, lancez-le de là :

    cd ..
    ./RespawnIRC

Tout ceci en une ligne :

    cd respawnIrc; qmake; make; cd ..; ./RespawnIRC

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

Contrairement à Linux c'est un bundle `RespawnIRC.app` qui est produit, et non un exécutable simple : le système de fichiers de macOS ne fait pas la différence entre majuscules et minuscules, un exécutable nommé `RespawnIRC` ne pourrait donc pas cohabiter à la racine du dépôt avec le dossier de sources `respawnIrc`. Le bundle est posé à la racine, lancez-le de là ou double-cliquez dessus depuis le Finder :

    cd ..
    ./RespawnIRC.app/Contents/MacOS/RespawnIRC

Le programme cherche `resources/`, `themes/` et `userdata/` **à côté du bundle** et non dedans (voir `pathTool::dataDirPath`), le bundle doit donc rester à la racine du dépôt.

#### Fabriquer une version distribuable

Le bundle produit ci-dessus embarque les chemins du Qt de la machine qui l'a compilé : il n'est utilisable ailleurs qu'après un passage par `macdeployqt`, ce dont s'occupe `dist-macos.sh` :

    ./dist-macos.sh ~/Qt/5.15.2/clang_64

Le script compile, copie Qt et QtWebEngine dans le bundle, le signe, et fabrique `dist/RespawnIRC-<version>-macos.dmg` (environ 100 Mo pour un bundle de 200 Mo). Sans argument, il utilise le Qt dont le `qmake` est dans le `PATH`.

L'image disque contient un dossier `RespawnIRC` avec l'application **et** les dossiers `resources` et `themes` : c'est ce dossier entier qu'il faut glisser dans les Applications, ou n'importe où ailleurs. Ces dossiers doivent rester à côté du bundle, que `pathTool::dataDirPath()` va y chercher. Ils ne sont plus jamais écrits depuis que le programme range ce qu'il produit dans `~/Library/Application Support` et `~/Library/Caches` : le bundle pourrait donc les embarquer et l'image se réduire à un simple `RespawnIRC.app`, ce qui reste à faire.

Trois limites de cette distribution :

- l'application est en x86_64 seulement, parce que le Qt 5.15.2 officiel n'existe qu'en x86_64. Sur un Mac Apple Silicon elle tourne donc via Rosetta 2. Une version arm64 demanderait de compiler Qt et QtWebEngine depuis les sources, ou de passer à Qt 6 ;
- la signature est ad hoc et l'application n'est pas notarisée : au premier lancement macOS la refusera. Il faut passer par un clic droit sur l'application puis « Ouvrir », ou retirer la mise en quarantaine avec `xattr -dr com.apple.quarantine /chemin/vers/RespawnIRC`. Cette signature ad hoc est bien celle qu'exigerait Apple Silicon pour du code arm64 ; elle ne dispense pas de la notarisation, seul un certificat Developer ID en dispenserait ;
- l'icône vient du `.ico` de Windows, qui plafonne à 128 pixels, et paraît donc un peu molle dans les grands affichages du Finder.

---

## Où le programme range ses données

Les dossiers `resources` et `themes` sont livrés avec le programme et ne sont jamais modifiés. Ce que RespawnIRC écrit se range en trois catégories, qui vont là où chaque système les attend.

**Sous Windows**, tout tient dans un dossier `userdata` à côté de l'exécutable, pour que le programme reste portable : on décompresse l'archive où l'on veut, et déplacer le dossier emporte la configuration avec lui.

| Chemin | Contenu |
| --- | --- |
| `userdata\config.ini` | les réglages : comptes, thème, listes de pseudos |
| `userdata\user_fr.dic` | mots ajoutés au correcteur orthographique |
| `userdata\resources\shortcut.txt` | raccourcis d'écriture |
| `userdata\resources\stickers\` | stickers téléchargés en lisant les topics |
| `userdata\logs\` | journal et pages sauvegardées quand `RESPAWNIRC_DEBUG` est actif |

**Sous Linux et macOS**, ce sont les dossiers standards :

| Contenu | Linux | macOS |
| --- | --- | --- |
| `config.ini` | `~/.config/RespawnIRC/` | `~/Library/Application Support/RespawnIRC/` |
| dictionnaire, raccourcis | `~/.local/share/RespawnIRC/` | `~/Library/Application Support/RespawnIRC/` |
| stickers téléchargés, logs | `~/.cache/RespawnIRC/` | `~/Library/Caches/RespawnIRC/` |

Les stickers téléchargés et les logs sont dans le cache parce qu'ils sont refabriquables : les premiers se retéléchargent tout seuls, les seconds ne servent qu'au diagnostic. Les effacer ne fait rien perdre.

Ces dossiers reproduisent la disposition des données livrées, et la lecture les consulte en premier : un fichier qui s'y trouve masque celui d'origine. C'est ce qui permet de poser son propre dictionnaire à côté sans toucher à celui livré.

Deux raisons à cette séparation. Une mise à jour se fait en remplaçant `resources` et `themes`, sans risquer d'effacer des réglages ; et les scripts de distribution n'embarquent plus les données du mainteneur, ce qu'ils faisaient jusqu'ici puisque les stickers téléchargés atterrissaient dans `resources/stickers/`, indiscernables de ceux livrés.

Les versions antérieures posaient tous ces fichiers directement à côté de l'exécutable ; ils sont déplacés au premier démarrage, il n'y a rien à faire à la main.
