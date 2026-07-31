# Pistes de simplification de la compilation et de la distribution

Relevé de juillet 2026 sur `respawnIrc.pro`, `tests/tests.pro`, `zlib.pri`, les scripts Windows, `dist-macos.sh` et la section « Compilation » du README. Les douze pistes de la première passe sont faites — les onze du relevé d'origine, plus l'absence de script de compilation côté Unix, venue après coup et décrite plus bas.

**Une seconde passe en a ouvert six autres**, décrites juste en dessous et dont **aucune n'est appliquée**. Elles ne se recouvrent pas avec les premières : celles-là portaient sur ce qui était en double ou en trop, celles-ci sur l'endroit où le temps passe et sur ce que les trois plateformes ne font pas pareil.

Ce fichier garde donc ce qui n'existe qu'ici : les pistes ouvertes, les décisions prises, les réserves qui restent et les leçons que tout cela a coûtées. Le détail de ce qui a été fait vit là où il sert — `CLAUDE.md`, le README, les commentaires des scripts — plutôt qu'en double. La version longue, avec les états des lieux d'origine, leurs chiffres et leurs numéros de ligne, est dans l'historique git ; c'est là qu'il faut aller pour savoir à quoi ressemblait la chaîne avant, et le commit qui a allégé ce fichier est le point de départ.

## Les pistes ouvertes

Les mesures qui suivent ont été faites ici, sur un Mac Intel — Core i3-1000NG4, 2 cœurs et 4 fils —, avec le Qt 5.15.2 officiel et `make -j4`. **Ce sont des chiffres de machine lente, et ce sont les rapports qui comptent, pas les secondes.** Ce qui n'a pas pu être mesuré est signalé comme tel à chaque fois : il n'y a ici ni machine Windows ni machine Linux.

### 13. Aucun en-tête précompilé, alors que 42 sources relisent les mêmes en-têtes Qt

Chacune des 42 sources du programme ouvre `QtWidgets`, `QtCore` ou `QtNetwork`, et le compilateur les réanalyse à chaque fois. `qmake` sait faire un en-tête précompilé — `CONFIG += precompile_header` et `PRECOMPILED_HEADER` — et le fait avec MSVC comme avec clang et gcc : **ce serait deux lignes dans les `.pro` et un fichier d'en-tête, sans rien de spécifique à une plateforme**, ce qui compte vu le principe rappelé plus bas. Mesuré avec les quatre en-têtes parapluie `QtCore`, `QtGui`, `QtWidgets` et `QtNetwork` :

| Ce qu'on compile | Sans en-tête précompilé | Avec | Écart |
| --- | --- | --- | --- |
| tout, dossier de compilation neuf | 99,2 s (274,2 s de processeur) | 90,9 s (148,9 s) | **-8 %** |
| les 42 sources, en-tête précompilé déjà là | 77,7 s (174,1 s) | 46,6 s (78,8 s) | **-40 %** |
| une source modifiée, édition de liens et bundle compris | 4,50 s | 2,96 s | **-34 %** |

Le temps de processeur tombe de moitié partout ; le temps d'attente, lui, ne suit qu'à partir de la deuxième compilation. **C'est le point à comprendre avant de juger la piste** : fabriquer l'en-tête précompilé coûte une quarantaine de secondes qui ne se parallélisent pas, si bien qu'une compilation partant de rien ne gagne presque rien et que `-c` / `-Clean` deviennent relativement plus chers. Le gain est sur la compilation ordinaire, celle qu'on fait vingt fois par jour. Les tests gagneraient de la même façon, et d'autant plus qu'ils recompilent six sources du programme pour leur compte.

Deux réserves. La première est que **ceci n'a été mesuré que sous macOS et clang** : le mécanisme de MSVC n'est pas le même — un `.cpp` engendré, `/Yc` une fois et `/Yu` sur chaque source — et n'a pas été essayé, pas plus que gcc. La seconde est que le contenu de l'en-tête est un réglage à lui seul : le prendre parapluie comme ici est le choix maximal, qui économise le plus par source et coûte le plus à fabriquer, et il pèse 37 Mo dans chaque dossier de compilation.

### 14. Sous Windows, la compilation est en série, et c'est la seule des trois plateformes dans ce cas

`build-windows.ps1` appelle `nmake`, qui n'a pas d'équivalent du `-j` de `make` : les 42 sources se compilent l'une après l'autre pendant que macOS et Linux en font quatre à la fois. `cl /MP` n'y change rien, `qmake` engendrant une invocation de `cl` par source. L'outil fait pour ça est **jom**, le clone parallèle de `nmake` écrit par Qt, un zip de 1,7 Mo sur `download.qt.io/official_releases/jom/` — exactement la forme de dépendance que `bootstrap-windows.ps1` sait déjà installer avec son `Get-FileIfNeeded`, `build-windows.ps1` le prenant quand il est là et retombant sur `nmake` sinon.

**Raisonné et non constaté, et il ne faut pas le présenter autrement.** Il n'y a pas de machine Windows ici, et la vérification de repli n'a pas marché non plus : `qmake -spec win32-msvc` refuse d'engendrer le `Makefile` sans `cl` pour l'interroger, donc même la forme des règles est déduite de celle du `Makefile` Unix. C'est aussi, si ça se confirme, le plus gros gain absolu des six pistes, puisque c'est la seule plateforme où rien ne se parallélise — et il s'ajoute à celui du point 13 au lieu de s'y substituer.

### 15. `dist-macos.sh` n'allège rien, alors que `dist-windows.ps1` le fait depuis longtemps

Le script Windows retire les traductions de QtWebEngine autres que `fr` et `en-US`, les `qt_*.qm` autres que le français, et les ressources des outils de développement de Chromium. Le script macOS ne retire rien, et le DMG publié porte donc tout. Relevé sur le DMG de la version 3.1.17 : `qtwebengine_locales` contient **53 fichiers pour 17 Mo** quand deux suffisent, et `qtwebengine_devtools_resources.pak` en fait 1,5 de plus. Le bundle passe de 208 à 190 Mo, et l'image disque de 96,2 à 88,6 Mio.

Il n'y a rien à inventer : ce sont les deux mêmes décisions que sous Windows, déjà prises et déjà documentées. Deux détails valent d'être notés au passage : les `.qm` de Qt ne sont pas dans le bundle, `macdeployqt` ne les copiant pas, donc seules les deux pièces de QtWebEngine sont concernées ; et l'allègement doit **précéder la signature**, pour la même raison que le `git archive`. C'est la troisième fois que la leçon du bas de ce fichier se vérifie — une décision prise pour une plateforme ne se propage pas toute seule aux autres.

### 16. L'image disque est en UDZO, qui est le format que personne n'a choisi

`dist-macos.sh` passe `-format UDZO`, la compression zlib. Or c'est aussi ce que `hdiutil` prend par défaut quand on lui donne un dossier source : ce n'est pas un choix, c'est celui qui n'a jamais été fait. Mesuré sur le bundle allégé du point 15 :

| Format | Temps | Taille |
| --- | --- | --- |
| UDZO, bundle complet (ce qu'on publie aujourd'hui) | 16,2 s | 96,2 Mio |
| UDZO | 14,2 s | 88,6 Mio |
| UDZO, `zlib-level=9` | 39,4 s | 81,2 Mio |
| **ULFO** (lzfse) | **12,2 s** | **79,8 Mio** |
| ULMO (lzma) | 117,0 s | 64,5 Mio |

ULFO est **plus petit et plus rapide** que ce qu'on fait aujourd'hui, ce qui est rare et ce qui rend la piste facile à juger : -17 % sur l'image, sans contrepartie de temps. La seule question est la compatibilité, et elle est réglée par le `man hdiutil` : ULFO demande macOS 10.11, quand l'application annonce elle-même 10.13 dans son `LSMinimumSystemVersion` — l'image ne peut donc pas être le maillon le plus exigeant. ULMO, lui, demande 10.15 et **relèverait ce plancher au-dessus de celui de l'application** pour 15 Mio et deux minutes de plus : c'est la seule des cinq lignes à écarter. L'image ULFO a été montée pour vérifier qu'elle porte bien le bundle allégé et le lien vers `/Applications`.

### 17. Rien ne vérifie que le bundle macOS est autonome, alors que l'archive Windows l'est à chaque fois

`dist-windows.ps1` relève au `dumpbin` que chaque DLL du runtime réclamée par un binaire de l'archive est bien dans l'archive, et cette vérification existe pour une raison écrite noir sur blanc dans `CLAUDE.md` : **la machine qui fabrique n'est pas un témoin valable**. macOS est dans le même cas, et même un peu pire, puisque la machine de développement porte à la fois le Qt de compilation, que le bundle trouve par un second `LC_RPATH`, et le Hunspell de Homebrew. Un bundle mal déployé y fonctionne parfaitement et ne démarre nulle part ailleurs. `CLAUDE.md` rapporte bien que l'autonomie a été vérifiée — mais **une fois, à la main**, et le script ne la vérifie pas.

Le pendant existe et a été essayé ici : `otool -l` sur chaque Mach-O du bundle, en refusant tout `name` ou `path` qui mentionne `/Users`, `/opt/homebrew` ou `/usr/local`. Sur le DMG publié il trouve 44 binaires et aucun fautif, ce qui reproduit exactement le relevé de `CLAUDE.md`. Et **il ne dit pas seulement oui** : lancé sur le bundle d'une compilation ordinaire, il désigne le seul binaire présent et ses deux lignes, `/usr/local/opt/hunspell/lib/libhunspell-1.7.0.dylib` et le `path /Users/.../Qt/5.15.2/clang_64/lib`. Il distingue donc bien un bundle déployé d'un bundle qui ne l'est pas, ce qui est la panne à attraper. Il lui faudrait le même garde-fou qu'à son cousin Windows — zéro binaire relevé veut dire que la vérification est cassée, jamais que le bundle est propre — et il a la même limite : il ne voit que ce qui est lié, pas ce qu'un `dlopen` irait chercher.

### 18. Quatre petites choses

- **`QMAKE_CXXFLAGS_RELEASE += -O2` dans `respawnIrc.pro` ne sert à rien.** `-O2` est déjà le défaut de la compilation release sur les trois plateformes — `QMAKE_CFLAGS_OPTIMIZE` dans `gcc-base.conf` pour gcc et clang, dans `msvc-desktop.conf` pour MSVC —, et le `Makefile` engendré porte bien `-O2 -O2`. `tests/tests.pro` n'a pas cette ligne : les deux `.pro` se contredisent déjà, sans conséquence ;
- **les deux scripts de distribution effacent tout `dist/` en commençant**, et pas seulement le dossier de travail `dist/image` : fabriquer la version suivante supprime silencieusement l'archive de la précédente, qui peut être celle qu'on vient de publier ;
- **`dist-macos.sh` prend son Qt en argument positionnel**, seul de tous les scripts, et son analyse d'arguments prend pour un chemin de Qt tout ce qu'elle ne reconnaît pas. Constaté : `./dist-macos.sh --skip-test`, une lettre de moins, répond `--skip-test/bin/qmake est introuvable ou non exécutable`, là où `build-unix.sh` répond `Option inconnue : --skip-test` ;
- **l'aide de `build-unix.sh` est un `sed -n '2,17p' "$0"`**, un intervalle de lignes tenu à la main : une ligne ajoutée à l'en-tête tronque l'aide sans que rien ne le signale.

### Regardé, mesuré, et écarté

Ces cinq-là ne sont pas des oublis, et deux ont coûté une mesure chacune :

- **fondre la signature dans `macdeployqt`** avec son `-codesign=-`, ce qui supprimerait une étape et le `--deep` qu'Apple a déprécié : mesuré **plus lent**, 33,8 s contre 17,8 + 3,2 = 21,0 s pour les deux étapes actuelles, les deux bundles étant acceptés par `codesign --verify --deep --strict`. À ne pas refaire ;
- **le `rm -rf` du bundle que `build-unix.sh` fait à chaque compilation**, qu'on soupçonnerait de forcer une édition de liens et une recopie des 366 fichiers de données pour rien : mesuré à 1,1 s, 1,54 s contre 0,43 s pour un `make` à vide. Négligeable, et c'est le prix du `DESTDIR` que les dossiers de compilation partagent. Ne pas y toucher ;
- **ccache** : le cas courant est ici une source qu'on vient de modifier, c'est-à-dire exactement celui que ccache ne peut pas servir. Il ne gagnerait que sur les compilations parties de rien et les changements de branche, pour une dépendance de plus sur deux plateformes et rien du tout sur la troisième. Le point 13 attaque le même coût par le bon bout ;
- **partager avec les tests les six sources du programme qu'ils recompilent** : il faudrait une bibliothèque statique et un `.pro` de plus, pour six fichiers sur 42 ;
- **`aqt --archives`** pour alléger le téléchargement de Qt à l'amorçage : ça figerait une liste de noms d'archives qui suit la version de Qt, ce que la leçon « ne pas figer de chiffres venant d'une installation » interdit précisément.

## Ce qui a été fait, et où c'est documenté maintenant

| # | Piste | Le détail est passé dans |
| --- | --- | --- |
| 1 | Le numéro de version analysé trois fois, dans trois langages | `version.pri`, et `CLAUDE.md` pour la contrepartie : plus de compilation sans qmake |
| 2 | Trois options `qmake` sous Windows dont aucune n'avait besoin d'exister | `CLAUDE.md` et le README : il ne reste que `DEFINES+=HUNSPELL_STATIC`, les deux `..._LIB_NAME` ne servant plus qu'aux noms inhabituels |
| 3 | Pas de script de compilation, seulement amorçage, distribution et lancement | `build-windows.ps1`, son en-tête, `CLAUDE.md` et le README |
| 4 | Une trentaine de lignes dupliquées entre les scripts PowerShell | `windows-common.ps1` et son en-tête |
| 5 | Chaque distribution recompilait tout | `CLAUDE.md` et le README : objets repris, `-Clean`, et l'exécutable effacé avant `nmake` |
| 6 | Les bibliothèques d'exécution de MSVC figées en dur | `CLAUDE.md` (« Ce qui manque et ne se voit pas ») et le README |
| 7 | macOS compilait dans les sources, Windows non | `dist-macos.sh`, `CLAUDE.md` et le README : `build/respawnIrc` sur les trois plateformes, Linux compris, et le piège d'`Info.plist` qui n'était pas ce qu'on croyait — voir ci-dessous |
| 8 | L'installation des outils Visual Studio et son redistribuable | `CLAUDE.md` (« Ce que l'abandon de Windows 7 a retiré ») |
| 9 | Deux listes de « trois choses » qui ne se recouvraient pas | résolu par l'abandon de Windows 7, commentaires de `dist-windows.ps1` |
| 10 | La compression | `dist-windows.ps1` : `ZipFile::CreateFromDirectory`, 7,6 s contre 13,5 |
| 11 | `opengl32sw.dll`, 20 Mo pour un repli que Windows fournit déjà | `CLAUDE.md`, le README et le commentaire de `dist-windows.ps1` |

Le point 3 a d'abord été livré sans avoir jamais compilé — seul son enchaînement avait été essayé avec des outils simulés, faute d'une machine équipée. Ce n'est plus le cas : `build-windows.ps1 -Tests` a compilé pour de bon sur une machine amorcée par `bootstrap-windows.ps1`, 142 vérifications sans échec et un `RespawnIRC.exe` de 1,52 Mo. Et la commande lancée était celle que le script d'amorçage affiche en terminant, ce qui a du même coup montré qu'elle ne marchait pas : elle donnait un `.\build-windows.ps1` nu, refusé par la stratégie d'exécution sur la machine qu'elle venait pourtant d'amorcer. Elle reprend maintenant le `powershell -ExecutionPolicy Bypass -File`.

Les points 3 et 5 n'avaient été appliqués qu'à Windows, et c'est **une douzième piste** qui s'est révélée en cherchant l'équivalent macOS des scripts `.ps1` : `dist-macos.sh` gardait sa propre copie de la compilation et n'a jamais lancé les tests, si bien qu'un DMG pouvait sortir sans qu'une seule des 142 vérifications ait tourné. `build-unix.sh` corrige les deux — un seul script pour macOS et Linux, les deux ne différant que par trois lignes. Ce qu'il faut en retenir dépasse le cas : **une décision prise pour une plateforme ne se propage pas toute seule aux autres**, et c'est d'autant plus vrai que ce fichier-ci s'était déclaré clos. Ce qui a été **refusé** au passage, et n'est pas un oubli : ni `bootstrap-` ni `run-` côté Unix, où ils emballeraient une ligne, et pas de `dist-linux.sh` tant que le projet n'a aucun format de distribution Linux — écrire ce script serait décider de publier des binaires, ce qui n'est pas une simplification de la chaîne.

Le point 6 est de loin le plus instructif des onze, et il vaut d'être lu dans l'historique : rangé ici en « deux détails » pour son gain de quelques lignes de script, il cachait l'absence de `msvcp140_1.dll`, c'est-à-dire une archive qui ne démarrait sur aucune machine sans redistribuable Visual C++.

Le point 7, lui, a été fait mais **son argument était faux**, et c'est tout ce qu'il en reste d'intéressant. Il promettait que compiler dans `build/` rendrait inutile le `rm -rf RespawnIRC.app` de `dist-macos.sh`, « un dossier neuf ne pouvant pas traîner un `Info.plist` périmé ». Le `Makefile` engendré dit le contraire : la cible de la règle est `<racine>/RespawnIRC.app/Contents/Info.plist`, là où le `DESTDIR` envoie le bundle, et pas un fichier du dossier de compilation. Elle est donc sautée dès qu'un bundle est là, quel que soit l'endroit d'où l'on compile, et l'effacement reste nécessaire — il est resté, avec un commentaire qui dit maintenant pourquoi. Vérifié et pas seulement lu : depuis un `build/respawnIrc`, un `Info.plist` dont on remplace le contenu par n'importe quoi survit intact à un `make`, qui répond `Nothing to be done`. La compilation hors des sources a été faite quand même, pour ce qu'elle donne vraiment : `respawnIrc/` ne garde plus ni `Makefile` ni `.o`, et le `.gitignore` a perdu les cinq règles qui les couvraient. Linux a suivi dans la foulée, la piste ne le mentionnant pas alors qu'il était dans le même cas que macOS, puis les tests, dont l'asymétrie était du même ordre : `build\tests` était arrivé avec `build-windows.ps1` — écrire un script fait choisir des dossiers explicites, taper `cd tests && qmake && make` à la main compile là où on est. La ligne de partage n'était donc pas « Windows contre Unix » mais « scripté contre tapé à la main ». **Les deux cibles se compilent maintenant sous `build/` sur les trois plateformes**, ce qui était le seul vrai gain à attendre de cette piste : plus rien ne s'écrit dans les sources, le `.gitignore` a perdu ses dix règles d'objets de compilation, et le nettoyage complet tient en un `rm -rf build/`.

**Suite, et elle confirme l'analyse ci-dessus** : le `DESTDIR` du programme est depuis passé de la racine à `build/`, de sorte que plus rien du tout n'atterrit à la racine, sur aucune plateforme. La cible de la règle `Info.plist` a suivi le `DESTDIR` comme prévu — `../../build/RespawnIRC.app/Contents/Info.plist:`, toujours sur un deux-points nu — donc l'effacement restait nécessaire, et pour la même raison qu'avant. Ce que ce déplacement a réellement rapporté est ailleurs : `resources/` et `themes/` voyagent maintenant avec l'exécutable sur les trois plateformes et non plus sous le seul macOS, et le `.gitignore` a perdu ses quatre règles de racine et l'exception `!/respawnIrc/` qui allait avec.

**Fin de l'histoire, et elle nuance la leçon** : ce qui manquait à ces règles, ce sont des dépendances, et le `.pro` peut les leur donner sans toucher à leurs commandes — une cible peut apparaître dans plusieurs règles d'un `Makefile` tant qu'une seule porte des commandes. C'est fait, pour les trois cas de la même famille : `Info.plist`, la copie de `resources/` et `themes/`, et la compilation des sources que rien ne rattachait à `version.pri`. Le `rm -rf` n'est donc plus ce qui garde de ces pièges ; il reste dans `build-unix.sh` pour la raison qui n'a jamais concerné macOS, le `DESTDIR` que les dossiers de compilation partagent. Ce que le point 7 avait établi sur le `Makefile` engendré était juste, sa conclusion pratique non : **elle tenait à un correctif que personne n'avait écrit, pas à une impossibilité.** La leçon du bas de ce fichier — lire ce que produisent les outils — vaut donc toujours, avec une seconde moitié : l'avoir lu ne dit pas encore qu'on ne peut rien y faire. Détail utile : le nom de la cible qu'on ajoute doit s'écrire exactement comme qmake l'a écrite, `make` comparant des chaînes ; le premier essai reconstruisait le chemin autrement, et n'a rien fait sans que rien ne le signale.

Ce n'était pas un écart : les `.pro` étaient déjà écrits pour ça, tous leurs chemins passant par `$$PWD`, et aucun n'a eu à changer. C'est aussi ce que fait Qt Creator par défaut — la première ligne du `.gitignore`, `build-respawnIrc*`, ignore ses dossiers de compilation depuis des années. Et si le passage à CMake évoqué par `MIGRATION-QT6.md` a lieu un jour, la compilation hors des sources y est la norme : cette piste rapproche de ce chemin-là plutôt qu'elle n'en éloigne.

## Les décisions à ne pas rouvrir sans le mainteneur

Ce ne sont pas des oublis :

- **`DEFINES+=HUNSPELL_STATIC` reste sur la ligne de commande** (point 2), plutôt que de devenir une ligne `win32:` du `.pro`. Le principe des `.pro` sans rien de spécifique à Windows a été jugé plus précieux que la disparition de la dernière option ;
- **la compilation de débogage reste hors de `build-windows.ps1`** (point 3) : ses propres noms de bibliothèques, son propre dossier, et un usage de diagnostic et non de production. Sa recette est dans le README ;
- **ne pas réintroduire la copie de l'Universal CRT sans réintroduire d'abord Windows 7 comme cible** (point 8). Cette recherche à deux dispositions selon la version du SDK était la partie la plus fragile du script, et la seule pièce dont l'absence ne se voyait sur aucune machine où l'on pouvait essayer l'archive ;
- **`opengl32sw.dll` reste retiré** (point 11), et la contrepartie est assumée : WARP est un composant de Windows, mais rien ne prouve qu'aucune machine cible n'a un Direct3D 11 cassé ou désactivé, et la panne serait alors totale et sans message utile. Le remettre est une ligne à supprimer dans `dist-windows.ps1`.

L'argument le plus solide du dernier point porte sur de vrais utilisateurs, et c'est le seul endroit où il est écrit : les releases publiées en amont sur `franckrj/respawnirc` ont été distribuées **avec QtWebEngine et ANGLE mais sans aucun rendu OpenGL logiciel** pendant environ un an et demi, sans problème signalé.

| Release | Date | QtWebEngine | `opengl32sw.dll` |
| --- | --- | --- | --- |
| v3.1.6 à v3.1.10 | 2018 – mars 2019 | oui (49 Mo) | **non** |
| v3.1.11 et suivantes | juillet 2019 → | oui (57 Mo puis 72) | oui |

Son apparition en v3.1.11 n'a rien d'une correction : elle est simultanée à un changement de version de Qt et à l'arrivée de `vc_redist.x86.exe` dans la même archive. Ce sont les deux fichiers que `windeployqt` ajoute tout seul — le second étant précisément celui que `--no-compiler-runtime` écarte ici. Il est donc arrivé comme effet de bord de l'outil de déploiement, jamais comme une réponse à une panne.

## Les réserves qui restent

- **`ZipFile::CreateFromDirectory` écrit des antislashs** dans les noms d'entrées de l'archive (point 10), mesuré sur ce .NET Framework 4.8 alors qu'on attendait des `/`. Ce qui sauve le changement, c'est que le `Compress-Archive` de PowerShell 5.1 en écrivait exactement autant : la forme des noms ne change pas et les archives publiées en ont toujours contenu. Mais la spécification ZIP demande des `/`, et un décompresseur non-Windows peut en sortir des fichiers dont le nom contient des antislashs plutôt qu'une arborescence. Si ça devenait un sujet, ce serait un défaut préexistant et non une régression.

## Les leçons, qui portent au-delà de ces pistes

- **une affirmation entre parenthèses est une affirmation.** « Ce dossier ne contient que les trois DLL voulues plus `concrt140.dll` » avait la forme d'une observation et n'était qu'une supposition ; un `dir` l'aurait démentie en une seconde, et elle a coûté une archive qui ne démarrait pas ;
- **classer une piste par son gain la classe mal quand le risque est ailleurs.** Le même point était rangé en « deux détails » à cause du gain, quelques lignes de script ; sa non-réalisation coûtait bien plus ;
- **ne pas figer de chiffres venant d'une installation** — nombre de DLL, numéro de version des outils, noms de fichiers : ils suivent la version du SDK ou des Build Tools. Chercher par glob, copier le dossier entier ;
- **une piste peut être bonne et son argument faux.** Le point 7 promettait la disparition d'un `rm -rf` que le changement de dossier ne faisait pas disparaître : le `Makefile` engendré le disait, personne ne l'avait ouvert. Lire ce que produisent les outils plutôt que raisonner sur ce qu'ils devraient produire — c'est valable pour `qmake`, et ça l'a déjà été pour `dumpbin` et `windeployqt`. **Et la suite du même point ajoute l'autre moitié** : ce `rm -rf` a fini par perdre sa raison d'être, une fois les dépendances manquantes ajoutées aux règles — il n'est plus dans les instructions du README, et ne reste dans `build-unix.sh` que pour un motif étranger à macOS. Constater ce qu'un outil produit dit ce qui est, pas ce qui est possible ;
- **une machine cesse d'être un témoin valable dès qu'on y installe ce dont on veut prouver l'absence.** Installer les Build Tools pose `msvcp140.dll` et sa famille dans `System32` : la machine qui fabrique l'archive ne peut donc jamais la valider. C'est le mécanisme qui a laissé sortir deux archives silencieusement incomplètes ;
- **« qu'est-ce qu'une plateforme fait que les autres ne font pas ? » est la question la plus productive de ce fichier.** Elle avait donné la douzième piste ; à la seconde passe elle en a donné trois de plus sur six — l'allègement que seul Windows fait, la vérification que seul Windows fait, et la compilation en série que seul Windows subit. Deux des trois sont des manques de macOS et la troisième un manque de Windows : ce n'est donc pas une plateforme qui est en retard, c'est le fait que chaque correctif a été écrit là où le problème s'est présenté. **La lire comme une liste de choses à faire sur une seule plateforme serait passer à côté.**

## Ce qu'il ne faut pas toucher

- `Invoke-BuildTool` ressemble à de la cérémonie, mais c'est la plus petite correction juste au fait que PowerShell 5.1 transforme la sortie d'erreur des outils natifs en erreurs fatales. S'en passer voudrait dire abandonner `$ErrorActionPreference = 'Stop'`, ce qui serait pire.
- `--no-compiler-runtime`, les DLL du runtime C++ de MSVC, le BOM des `.ps1` : tous documentés, tous justifiés. Les DLL du runtime ne sont pas parties avec Windows 7 et ne doivent pas être confondues avec ce qui l'a fait — elles ne sont dans aucun Windows. Ne pas non plus chercher à les réduire à celles qui sont importées aujourd'hui : c'est cette économie-là, 1,1 Mo, qui a produit une archive incapable de démarrer.
- `opengl32sw.dll` a longtemps figuré ici, au titre du rendu de secours des machines sans pilote OpenGL. Il en est sorti, et de l'archive avec : ce rôle est tenu par ANGLE et WARP, pas par lui. C'est le rappel utile de cette section — une entrée y était depuis des années sur une justification que personne n'avait vérifiée.
