# Pistes de simplification de la compilation et de la distribution

Relevé de juillet 2026 sur `respawnIrc.pro`, `tests/tests.pro`, `zlib.pri`, les scripts Windows, `dist-macos.sh` et la section « Compilation » du README. Les douze pistes de la première passe sont faites — les onze du relevé d'origine, plus l'absence de script de compilation côté Unix, venue après coup et décrite plus bas.

**Une seconde passe en a ouvert six autres**, décrites juste en dessous. Elles ne se recouvrent pas avec les premières : celles-là portaient sur ce qui était en double ou en trop, celles-ci sur l'endroit où le temps passe et sur ce que les trois plateformes ne font pas pareil. **Deux sont faites**, le format de l'image disque du point 16 et l'allègement du bundle du point 15 ; les quatre autres sont ouvertes et rien n'en a été appliqué.

**Une troisième passe, le 31 juillet 2026, en a ouvert une septième** — le `qmake` que `build-unix.sh` relance à chaque compilation, point 19 — et a surtout corrigé deux des précédentes : les secondes du point 13 avaient dérivé et sont remesurées, et le point 15 gagne le relevé de l'endroit où le poids du bundle se trouve vraiment. Elle est partie du code et des scripts, ce fichier n'étant rouvert qu'ensuite ; c'est ce qui lui a fait trouver la seule des trois qui n'était écrite nulle part.

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

**Deux des chiffres de la colonne de gauche ont été remesurés le 31 juillet 2026, sur la même machine, et ils ont doublé** : un `make` à vide répond en 1,5 s là où ce fichier annonçait 0,43 s, et une source modifiée coûte 9,7 s là où il annonçait 4,50 s — trois exécutions chacune, la machine au repos. Une précision sur cette seconde mesure, qui n'est pas tout à fait celle de la ligne du tableau : `make` n'y a fait que compiler et relier, la recopie des données du bundle n'ayant pas eu lieu puisque ses prérequis étaient satisfaits. **La cause de l'écart n'est pas établie**, et les deux candidats plausibles ne se départagent pas d'ici : les prérequis ajoutés depuis aux règles du bundle, qui font relever des centaines de dates à chaque `make`, ou l'état de la machine le jour des mesures d'origine. Ce qu'il faut en retenir avant de juger la piste : **la colonne « sans en-tête précompilé » est à refaire**, et celle de droite avec elle, puisqu'il faudrait remonter l'essai pour l'obtenir. Les rapports, eux, n'ont aucune raison connue d'avoir bougé.

Deux réserves. La première est que **ceci n'a été mesuré que sous macOS et clang** : le mécanisme de MSVC n'est pas le même — un `.cpp` engendré, `/Yc` une fois et `/Yu` sur chaque source — et n'a pas été essayé, pas plus que gcc. La seconde est que le contenu de l'en-tête est un réglage à lui seul : le prendre parapluie comme ici est le choix maximal, qui économise le plus par source et coûte le plus à fabriquer, et il pèse 37 Mo dans chaque dossier de compilation.

### 14. Sous Windows, la compilation est en série, et c'est la seule des trois plateformes dans ce cas

`build-windows.ps1` appelle `nmake`, qui n'a pas d'équivalent du `-j` de `make` : les 42 sources se compilent l'une après l'autre pendant que macOS et Linux en font quatre à la fois. `cl /MP` n'y change rien, `qmake` engendrant une invocation de `cl` par source. L'outil fait pour ça est **jom**, le clone parallèle de `nmake` écrit par Qt, un zip de 1,7 Mo sur `download.qt.io/official_releases/jom/` — exactement la forme de dépendance que `bootstrap-windows.ps1` sait déjà installer avec son `Get-FileIfNeeded`, `build-windows.ps1` le prenant quand il est là et retombant sur `nmake` sinon.

**Raisonné et non constaté, et il ne faut pas le présenter autrement.** Il n'y a pas de machine Windows ici, et la vérification de repli n'a pas marché non plus : `qmake -spec win32-msvc` refuse d'engendrer le `Makefile` sans `cl` pour l'interroger, donc même la forme des règles est déduite de celle du `Makefile` Unix. C'est aussi, si ça se confirme, le plus gros gain absolu de toutes les pistes ouvertes, puisque c'est la seule plateforme où rien ne se parallélise — et il s'ajoute à celui du point 13 au lieu de s'y substituer.

### 15. ~~`dist-macos.sh` n'allège rien, alors que `dist-windows.ps1` le fait depuis longtemps~~ **Fait**

Le script Windows retirait les traductions de QtWebEngine autres que `fr` et `en-US`, les `qt_*.qm` autres que le français, et les ressources des outils de développement de Chromium ; le script macOS ne retirait rien, et le DMG publié portait donc tout. Il fait maintenant les mêmes retraits, plus deux petites pièces qui n'ont pas d'équivalent Windows, et **le DMG a été refabriqué de bout en bout pour le vérifier** — `./dist-macos.sh` complet, 142 vérifications sans échec :

| Ce qui est retiré | Avant | Après |
| --- | --- | --- |
| `qtwebengine_locales` | 53 fichiers, 17 Mo | 2 fichiers, `fr` et `en-US` |
| `qtwebengine_devtools_resources.pak` | 1,5 Mo | retiré |
| greffons `position` et `QtSerialPort` | 0,3 Mo | retirés |
| **bundle** | 208 Mo | **190 Mo** |
| **image disque** | 87 Mo | **80 Mo** (79,7 Mio) |

Il n'y avait rien à inventer : ce sont les mêmes décisions que sous Windows, déjà prises et déjà documentées. C'est la troisième fois que la leçon du bas de ce fichier se vérifie — une décision prise pour une plateforme ne se propage pas toute seule aux autres. Trois détails, tous relevés en le faisant :

- les `.qm` de Qt ne sont pas dans le bundle, `macdeployqt` ne les copiant pas : seules les pièces de QtWebEngine sont concernées, et la ligne que `dist-windows.ps1` consacre aux `qt_*.qm` n'a pas de pendant ici ;
- l'allègement doit **précéder la signature**, pour la même raison que le `git archive` ;
- **`QtPositioning` reste**, et c'est ce qu'il ne faut pas confondre : l'exécutable et `QtWebEngineCore` s'y lient pour de bon, `otool -L` le dit, donc le retirer empêcherait le programme de démarrer. Ne partent que les trois greffons `position`, chargés à la demande et jamais demandés ici, et le `QtSerialPort` que le seul greffon NMEA faisait entrer.

Le résultat est vérifié sur le DMG refabriqué : 39 binaires Mach-O au lieu de 43, aucun chemin absolu vers `/Users`, `/opt/homebrew` ou `/usr/local`, tous en `10.13`, et une signature que `codesign --verify --deep --strict` accepte. Le bundle démarre. **Et Chromium a été exercé pour de bon**, ce qui était le vrai risque de l'allègement : un petit programme `QWebEngineView`, pointé sur les ressources du bundle allégé par `QTWEBENGINE_RESOURCES_PATH` et `QTWEBENGINE_LOCALES_PATH`, rend une page **y compris sous une locale dont le `.pak` a été retiré** — essayé en `ja_JP`, où le repli sur `en-US` fonctionne comme prévu.

**Le parcours dans l'application elle-même a été rejoué ensuite**, sur le bundle extrait du DMG, et c'est ce qui manquait à la vérification ci-dessus : les deux seuls usages que le programme fait de QtWebEngine sont le navigateur interne et la fenêtre de connexion, et les deux ont été ouverts pour de vrai. « Ouvrir RespawnIRC Navigator » affiche jeuxvideo.com — le `QtWebEngineProcess` lancé est bien celui des `Helpers` du bundle —, et « Se connecter » puis « Afficher la page de connexion » rend la page de `/sso/login`. Relancé avec `LANG=ja_JP.UTF-8`, le programme affiche la même page avec un Chromium en `--lang=ja` alors que ce `.pak` a disparu : **le repli vaut donc pour l'application et pas seulement pour un programme d'essai**. Ce parcours se pilote par `osascript` et System Events, ce qui demande l'accès assistif accordé à l'outil depuis lequel on travaille — sans lui `osascript` répond `-1719`, et c'est cette réponse-là, et non une impossibilité, qui avait fait écrire ici que le parcours n'était pas rejouable. **Leçon, et c'est la même que celle du point 7** : une vérification déclarée impossible mérite d'être réessayée avant d'être recopiée.

**Le même relevé dit où le poids se trouve vraiment, ce qui borne ce que cette piste peut rapporter** : sur les 208 Mo du bundle, 197 sont des frameworks et **164 le seul `QtWebEngineCore`** — 132 de binaire et 32 de ressources, `icudtl.dat` et les `.pak` compris. Tout le reste réuni, Qt Core, Gui, Widgets, Network, QtQuick, QML, les greffons, l'exécutable et les données du programme, fait 44 Mo. Les 18 Mo de cette piste sont donc à peu près tout ce qui se prend sans toucher à Chromium, et c'est la répartition à avoir en tête avant d'aller chercher des mégaoctets ailleurs.

**Ce qui mène à une correction qui n'appartient pas à ce fichier, mais qui change la façon de lire ces 164 Mo** : `unix-common.sh` écrivait que QtWebEngine est ce « dont dépend tout l'affichage des messages ». C'était faux, et c'est corrigé sur place ; ce commentaire était le seul endroit du dépôt à le dire, `CLAUDE.md` et le README ne l'écrivant nulle part. Les messages s'affichent dans un `QTextBrowser` — `messagesBox`, dans `showTopic.hpp` — et QtWebEngine ne sert qu'à deux choses : la fenêtre de connexion (`connectWindow`, qui charge la page de `/sso/login` dans un `QWebEngineView` pour en récupérer les cookies) et le navigateur interne (`webNavigator`, ouvert sur un lien ou sur la boîte de réception des MP). Ces 164 Mo sont donc le prix d'une page de connexion et d'un navigateur d'appoint, pas celui de la fonction principale du programme. **Ce n'est pas pour autant une piste de compilation** : s'en passer serait retirer une fonctionnalité, et la connexion derrière Cloudflare a toutes les chances d'avoir besoin d'un vrai navigateur — l'`addCookieWindow` du programme montre qu'un contournement manuel existe, il ne montre pas qu'il suffirait. C'est une question pour le mainteneur, pas un rangement de la chaîne.

### 16. ~~L'image disque est en UDZO, qui est le format que personne n'a choisi~~ **Fait**

`dist-macos.sh` passait `-format UDZO`, la compression zlib. Or c'est aussi ce que `hdiutil` prend par défaut quand on lui donne un dossier source : ce n'était pas un choix, c'était celui qui n'avait jamais été fait. Il passe maintenant `-format ULFO`, et **le DMG a été refabriqué de bout en bout pour le vérifier** — `./dist-macos.sh` complet, 142 vérifications sans échec, `hdiutil imageinfo` annonçant bien `UDIF read-only compressed (lzfse)`.

Les deux mesures qui décident, faites sur le bundle **réellement distribué**, celui de 208 Mo que l'allègement du point 15 n'avait pas encore touché :

| Format | Temps | Taille |
| --- | --- | --- |
| UDZO (ce qu'on publiait) | 14,2 s | 96,2 Mio |
| **ULFO** (lzfse) | **12,2 s** | **87,0 Mio** |

ULFO est **plus petit et plus rapide**, ce qui est assez rare pour rendre la décision facile : -9,6 % sur l'image, sans contrepartie de temps. La compatibilité est réglée par le `man hdiutil` : ULFO demande macOS 10.11, quand l'application annonce elle-même 10.13 dans son `LSMinimumSystemVersion`, donc l'image ne peut pas être le maillon le plus exigeant. Ce 10.13 n'est d'ailleurs pas un choix du dépôt mais celui de Qt, `CLAUDE.md` dit d'où il sort ; ce qui compte ici est qu'il ne peut pas descendre, donc que la marge d'ULFO est acquise.

**Attention en relisant les chiffres de cette piste, un premier jet les a conflés** : les mesures d'origine comparaient un ULFO sur bundle allégé, 79,8 Mio, à un UDZO sur bundle complet, 96,2 — soit -17 %, mais dont la moitié revenait à l'allègement du point 15 et non au format. Le tableau ci-dessus ne compare plus que ce qui doit l'être, à bundle identique. Pour mémoire, les autres formats mesurés sur le bundle allégé : UDZO 88,6 Mio, `zlib-level=9` 81,2 en 39,4 s, ULFO 79,8, ULMO 64,5 en 117,0 s. **Les deux gains se cumulent**, ce que le point 15 a depuis confirmé en se faisant : le DMG publié par le script, ULFO sur bundle allégé, fait 79,7 Mio — soit la ligne annoncée ici, à 0,1 près.

ULMO est la seule ligne à écarter, et pas pour ses deux minutes : il demande macOS 10.15 et **relèverait le plancher au-dessus de celui de l'application**. C'est la seule objection que le passage à Qt 6 lèverait, son plancher étant macOS 13 — noté dans la section « ce que ça rapporte » de `MIGRATION-QT6.md`, avec la réserve que le temps de compression, lui, ne bougera pas.

### 17. Rien ne vérifie que le bundle macOS est autonome, alors que l'archive Windows l'est à chaque fois

`dist-windows.ps1` relève au `dumpbin` que chaque DLL du runtime réclamée par un binaire de l'archive est bien dans l'archive, et cette vérification existe pour une raison écrite noir sur blanc dans `CLAUDE.md` : **la machine qui fabrique n'est pas un témoin valable**. macOS est dans le même cas, et même un peu pire, puisque la machine de développement porte à la fois le Qt de compilation, que le bundle trouve par un second `LC_RPATH`, et le Hunspell de Homebrew. Un bundle mal déployé y fonctionne parfaitement et ne démarre nulle part ailleurs. `CLAUDE.md` rapporte bien que l'autonomie a été vérifiée — mais **une fois, à la main**, et le script ne la vérifie pas.

Le pendant existe et a été essayé ici : `otool -l` sur chaque Mach-O du bundle, en refusant tout `name` ou `path` qui mentionne `/Users`, `/opt/homebrew` ou `/usr/local`. Sur le DMG publié il trouvait 44 binaires et aucun fautif, ce qui reproduisait exactement le relevé de `CLAUDE.md` ; rejoué le 31 juillet 2026 sur le DMG allégé du point 15, il en trouve 39 — Hunspell est devenu statique et l'allègement a emporté quatre binaires — et toujours aucun fautif. Et **il ne dit pas seulement oui** : lancé sur le bundle d'une compilation ordinaire, il désigne le seul binaire présent et ses deux lignes, `/usr/local/opt/hunspell/lib/libhunspell-1.7.0.dylib` et le `path /Users/.../Qt/5.15.2/clang_64/lib`. Il distingue donc bien un bundle déployé d'un bundle qui ne l'est pas, ce qui est la panne à attraper. Il lui faudrait le même garde-fou qu'à son cousin Windows — zéro binaire relevé veut dire que la vérification est cassée, jamais que le bundle est propre — et il a la même limite : il ne voit que ce qui est lié, pas ce qu'un `dlopen` irait chercher.

**La même passe doit relever le plancher macOS de chaque binaire**, et ce second contrôle a rapporté quelque chose que le premier ne voyait pas : `libhunspell-1.7.0.dylib` annonçait `minos 14.0` quand tout le reste annonce 10.13, `LSMinimumSystemVersion` compris. Homebrew compile pour la machine où il tourne, et l'éditeur de liens n'a rien dit — aucun avertissement du genre « built for newer macOS version » dans le journal d'une compilation complète. C'était exactement la forme de panne que ce contrôle existe pour attraper : invisible sur la machine qui fabrique, contredisant ce que le bundle promet, et repérable en une commande.

**Ce cas-là est réglé**, non pas en répondant à la question mais en la supprimant : Hunspell se compile maintenant à la main dans le `hunspell/` de la racine, statique et avec le bon plancher, et le DMG refabriqué n'a plus aucun binaire hors de 10.13 — voir `CLAUDE.md` et le README. **Ça ne retire rien à la piste**, au contraire : c'est la démonstration qu'elle attrape de vraies pannes, et le contrôle reste à écrire pour que celle-ci ne revienne pas par le repli sur Homebrew que le `.pro` garde. Ce qui appartient en propre à cette piste est que **le seuil à comparer ne doit pas être écrit en dur** mais lu dans l'`Info.plist` du bundle, sans quoi le contrôle mentirait le jour où le plancher bougera — et il bougera, Qt 6.11 demandant macOS 13.

Un piège d'écriture, relevé en refaisant ce contrôle sur le DMG allégé : **le plancher ne s'écrit pas dans la même charge utile selon qui a produit le binaire.** Les 39 Mach-O de ce bundle le portent en `LC_VERSION_MIN_MACOSX`, dont le champ s'appelle `version`, quand le `libhunspell` de Homebrew le portait en `LC_BUILD_VERSION`, dont le champ s'appelle `minos` — c'est le producteur qui décide, un outillage récent émettant la seconde forme. Un contrôle qui ne chercherait que `minos`, ce que le paragraphe ci-dessus disait, **ne relèverait donc rien du tout sur les binaires de Qt** et se déclarerait content : exactement la panne muette contre laquelle le garde-fou « zéro binaire relevé veut dire que la vérification est cassée » existe.

### 18. Quatre petites choses

- **`QMAKE_CXXFLAGS_RELEASE += -O2` dans `respawnIrc.pro` ne sert à rien.** `-O2` est déjà le défaut de la compilation release sur les trois plateformes — `QMAKE_CFLAGS_OPTIMIZE` dans `gcc-base.conf` pour gcc et clang, dans `msvc-desktop.conf` pour MSVC —, et le `Makefile` engendré porte bien `-O2 -O2`. `tests/tests.pro` n'a pas cette ligne : les deux `.pro` se contredisent déjà, sans conséquence ;
- **les deux scripts de distribution effacent tout `dist/` en commençant**, et pas seulement le dossier de travail `dist/image` : fabriquer la version suivante supprime silencieusement l'archive de la précédente, qui peut être celle qu'on vient de publier ;
- **`dist-macos.sh` prend son Qt en argument positionnel**, seul de tous les scripts, et son analyse d'arguments prend pour un chemin de Qt tout ce qu'elle ne reconnaît pas. Constaté : `./dist-macos.sh --skip-test`, une lettre de moins, répond `--skip-test/bin/qmake est introuvable ou non exécutable`, là où `build-unix.sh` répond `Option inconnue : --skip-test` ;
- **l'aide de `build-unix.sh` est un `sed -n '2,17p' "$0"`**, un intervalle de lignes tenu à la main : une ligne ajoutée à l'en-tête tronque l'aide sans que rien ne le signale.

### 19. `build-unix.sh` relance `qmake` à chaque compilation, alors que le `Makefile` sait le refaire lui-même

Le script appelle `qmake` puis `make` à chaque fois, pour le programme comme pour les tests. Or le `Makefile` que `qmake` engendre **porte sa propre règle de régénération**, avec `respawnIrc.pro`, `version.pri`, `zlib.pri` et tous les mkspecs en prérequis, et elle rejoue la ligne de commande exacte — `CONFIG+=sdk_no_version_check` compris, `qmake` l'ayant écrite dans le `Makefile`. Un `make` suffit donc à rattraper un `.pro` ou un `version.pri` modifié, et le `qmake` du script ne fait que refaire d'avance ce que `make` aurait fait au besoin. Mesuré ici le 31 juillet 2026, machine au repos, trois exécutions par ligne :

| Ce qu'on lance | Temps |
| --- | --- |
| `qmake` sur `respawnIrc.pro` | 5,1 s |
| `qmake` sur `tests.pro` | 2,25 s |
| `make -j4` à vide | 1,5 s |
| `make -j4`, une source modifiée | 9,7 s |
| **`./build-unix.sh`, une source modifiée** | **19,6 s** |

Soit à peu près la moitié d'une compilation incrémentale scriptée passée hors de la compilation : 5,1 s de `qmake` que rien ne réclamait, plus la seconde d'effacement du bundle déjà mesurée plus bas. Avec `-t`, les 2,25 s des tests s'y ajoutent.

**Ce n'est pas une ligne à supprimer sans la contrepartie**, et c'est ce qui rend la piste moins simple qu'elle n'en a l'air : c'est ce `qmake` systématique qui referme le trou décrit dans `CLAUDE.md` — la liste `$$files()` des données du bundle est figée au moment du `qmake`, donc un fichier **créé puis modifié** sans `qmake` entre les deux échappe à ses deux prérequis. Le supprimer rouvre ce trou pour qui ne passe pas par `-c`. La forme raisonnable est donc de ne lancer `qmake` que si le `Makefile` manque, et de dire dans l'aide que `-c` est ce qu'il faut après avoir ajouté des fichiers ; pas de faire disparaître l'appel.

Deux choses vérifiées au passage, pour éviter les fausses pistes. La lenteur de `qmake` ne vient **pas** de la récursion `$$files` sur `resources/` et `themes/` : isolée dans un `.pro` jetable qui ne fait qu'elle, elle coûte 0,08 s. Les 5,1 s sont la résolution des modules Qt et l'écriture des 4 125 lignes du `Makefile`, et l'écart avec les 2,25 s de `tests.pro` tient à `webenginewidgets` et à ce qu'il traîne. Il n'y a donc rien à gagner en allégeant le `.pro`, seulement en ne le relisant pas pour rien. **Le pendant Windows n'a pas été regardé** : `nmake` reçoit un `Makefile` de la même famille, qui a toutes les chances de porter la même règle, mais il n'y a pas de machine ici pour le lire.

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
