# RespawnIRC (version PC, Qt)

Client alternatif pour les forums de jeuxvideo.com, en C++/Qt5 Widgets. Le pendant
Android, **beaucoup plus à jour**, est dans `../../repo-android` : c'est la
meilleure référence quand JVC change quelque chose. Regarder son `JVCParser.java` et
son historique git avant de deviner quoi que ce soit.

## Compiler et tester

Chaque bloc part de la racine du dépôt. **Rien ne se compile plus dans les sources** : les deux
cibles, sur les trois plateformes, ont leur dossier sous `build/`.

```bash
# sous macOS et sous Linux, un seul script pour les deux ; -t compile et lance les tests
./build-unix.sh -t            # il trouve Qt seul, y compris le ~/Qt d'aqtinstall sous macOS

# à la main ; sous macOS, ajouter CONFIG+=sdk_no_version_check au qmake
mkdir -p build/respawnIrc && cd build/respawnIrc
qmake ../../respawnIrc/respawnIrc.pro && make -j4

# les tests ; sous Windows, build-windows.ps1 -Tests fait la même chose avec nmake
mkdir -p build/tests && cd build/tests
qmake ../../tests/tests.pro && make -j4 && ../respawnIrcTests
```

`build-unix.sh` est le pendant de `build-windows.ps1`, et le seul endroit où la compilation Unix est
écrite : `dist-macos.sh` l'appelle au lieu de garder sa copie des mêmes étapes, et lance donc les
tests avant d'assembler le DMG. `unix-common.sh`, chargé par point-sourcing dans les deux, est le
pendant de `windows-common.ps1` et porte la seule chose qu'ils partagent : la résolution de Qt. Elle
y est descendue parce que **les deux doivent tomber sur le même Qt** — `build-unix.sh` compile contre
celui qu'il résout, `dist-macos.sh` prend `macdeployqt` dans celui qu'il résout, et une divergence
remplirait le bundle des frameworks d'un autre Qt que celui de la compilation. Ce serait une panne
silencieuse chez l'utilisateur, pas une erreur de compilation. `dist-macos.sh` passe donc le Qt résolu
explicitement en `-q` plutôt que de compter sur une coïncidence. Un script pour les deux systèmes et non un par plateforme — ils ne
diffèrent que par l'option qmake du SDK, la façon de compter les processeurs et le nom de ce qui
sort. Il n'y a délibérément **ni `bootstrap-` ni `run-` côté Unix** : le premier serait un emballage
autour d'un `apt install` ou de trois commandes Homebrew et `aqt`, le second autour de
`./RespawnIRC`. Les quatre scripts Windows n'existent que parce que Windows n'a ni gestionnaire de
paquets, ni Qt trouvable, ni Hunspell et zlib tout faits. Et **pas de `dist-linux.sh`** non plus : le
projet n'a aucun format de distribution Linux — ni `.deb`, ni AppImage, ni Flatpak — donc la question
n'est pas d'écrire un script mais de décider qu'on publie des binaires Linux, ce qui appartient au
mainteneur.

Les `.pro` ont un `DESTDIR` : quelle que soit la plateforme et l'endroit d'où `qmake` est lancé, le
programme **et les tests** atterrissent dans `build/`, et **rien à la racine du dépôt**, qui ne porte
donc que des sources. Il n'y a plus rien à déplacer à la main après compilation, seuls les objets
intermédiaires suivent la façon de compiler — ils restent dans `build/respawnIrc` et `build/tests`,
sous ce qui en sort. C'est ce qui rend la compilation hors des sources gratuite : elle ne déplace
**rien** de ce qui sort, seulement ce qui traîne pendant. Les `.pro` acceptent les deux façons — tous
leurs chemins passent par `$$PWD` — et rien ne les distingue à l'arrivée ; compiler dans les sources
marche donc encore, mais n'est plus documenté et le `.gitignore` ne couvre plus ce que ça y laisse.
Le nettoyage complet tient désormais en un `rm -rf build/`.

Le programme cherchant `resources/` et `themes/` à côté de son exécutable, **ils voyagent avec lui** :
le `QMAKE_BUNDLE_DATA` les met dans le bundle sous macOS, et un `QMAKE_POST_LINK` du bloc `!macx` les
recopie dans `build/` ailleurs. Deux conséquences à connaître. La première est que ce bloc porte la
seule chose du `.pro` qui distingue les plateformes en dehors de `macx` — la destination s'écrit
différemment selon l'outil, `cp -f -R` voulant le dossier parent et fusionnant dans une destination
déjà là, `xcopy /s /q /y /i` voulant le dossier cible. Ce n'est **pas** le cas du
`DEFINES+=HUNSPELL_STATIC` qu'on a refusé de descendre dans le `.pro` : celui-là avait une ligne de
commande `qmake` où vivre, celui-ci n'en a pas. La seconde est que la copie se fait à l'édition de
liens : un thème modifié sans qu'un `.cpp` bouge ne donnait donc rien à relier, et ne parvenait
jamais au programme. Le même bloc met maintenant les fichiers des deux dossiers en `PRE_TARGETDEPS`,
ce qui suffit à faire relier pour eux et donc à rendre la main au `QMAKE_POST_LINK` ; le prix est une
édition de liens pour un thème modifié. Sous macOS le même besoin est couvert autrement, les règles
du bundle ne passant pas par l'exécutable — voir plus bas.

Cette disposition a fait disparaître quatre règles du `.gitignore` et l'exception qui allait avec :
`/RespawnIRC`, `/RespawnIRC.app/`, `/RespawnIRC.exe`, `/RespawnIRC.pdb` et le `!/respawnIrc/` que la
première rendait nécessaire. C'était le piège de casse le plus désagréable du dépôt — `/RespawnIRC`
ignorait aussi le dossier de sources `respawnIrc`, macOS ne distinguant pas les majuscules, et tout
fichier qu'on y ajoutait devenait invisible pour `git status`. Le `/build/` déjà présent couvre
maintenant tout. **Ne pas réintroduire de règle de racine** sans avoir d'abord ramené quelque chose
à la racine.

Dépendances Debian : `qtbase5-dev qtmultimedia5-dev libhunspell-dev qtwebengine5-dev
zlib1g-dev`. zlib sert à décompresser le payload des pages (voir plus bas).

L'exécutable doit tourner depuis `build/` (il y cherche `themes/` et `resources/`, que la compilation
y dépose à côté de lui), ce que le `DESTDIR` lui donne gratuitement.

Le numéro de version est dans `version.pri`, à la racine, et **nulle part ailleurs** : le `.pro`
l'inclut, le pousse dans un `DEFINES` que `respawnIrc.cpp` reprend pour `currentVersionName`, et le
donne à la `VERSION` de qmake dont `Info.plist` tire la version du bundle ; les deux scripts de
distribution lisent ce même fichier pour nommer leurs archives. Une publication ne touche donc
qu'une ligne. Il était auparavant dans le littéral C++, que trois analyseurs syntaxiques allaient
lire — une regex PowerShell, un `sed`, et un enchaînement `$$cat`/`$$find`/`$$replace` dans le
`.pro`. Conséquence assumée : le programme ne se compile plus sans qmake, un `#error` le dit.

Ce numéro arrivant aux sources par un `-D` de la ligne de commande du compilateur, **`make` ne le voit
pas** : rien ne rattachait les objets déjà compilés à `version.pri`, et un numéro changé ne leur
parvenait donc pas. Constaté, et pas seulement possible : `version.pri` passé à 3.1.42, un
`./build-unix.sh` a rendu un bundle dont `Info.plist` annonçait 3.1.42 et dont le programme annonçait
toujours v3.1.17 — le `rm -rf` du script ne s'en gardait pas, il force l'édition de liens et pas la
compilation, et le `respawnIrc.o` resté en place portait l'ancien numéro. Une publication pouvait donc
sortir en annonçant deux versions différentes selon l'endroit où on la regardait. Le `.pro` fait
maintenant de `version.pri` un prérequis de `$(OBJECTS)`, la liste que qmake a lui-même écrite dans le
`Makefile` : un changement de version recompile tout, ce qui coûte moins cher que de tenir à jour la
liste des sources qui lisent le `DEFINES` — `respawnIrc.cpp` seul aujourd'hui, et rien dans le `.pro`
ne le dit. C'est le même procédé que pour les deux règles du bundle macOS plus bas, avec le même
avantage : le nom des cibles vient de qmake et n'est pas deviné.

**Il n'avait pas été essayé sous `nmake`, et il y cassait tout.** Sous MSVC, `CONFIG` porte
`debug_and_release` et qmake écrit **trois** fichiers : un `Makefile` qui ne fait que rappeler `nmake`
sur `Makefile.Release` et `Makefile.Debug`, et ces deux-là. Seuls les deux derniers définissent
`OBJECTS`. Dans le premier, `$(OBJECTS)` ne vaut rien, et `nmake` ne l'ignore pas comme le ferait
`make` : il s'arrête sur un `fatal error U1083: target macro '$(OBJECTS)' expands to nothing`.
Autrement dit **plus rien ne se compilait sous Windows**, ni `build-windows.ps1` ni
`dist-windows.ps1`, et l'échec ne désignait pas le `.pro` d'où il venait. Le `.pro` n'écrit donc la
règle que dans les passes qui compilent vraiment, par un `build_pass|!debug_and_release` : là où il
n'y a qu'un seul `Makefile`, ce qui est le cas de macOS et de Linux, `debug_and_release` est absent et
rien ne change. Vérifié des deux côtés après correction — `build-windows.ps1 -Tests` compile et rend
142 vérifications sans échec, `dist-windows.ps1` fabrique l'archive entière, et un `version.pri`
retouché fait bien tout recompiler sous `nmake`. La leçon vaut pour les règles ajoutées à la main dans
un `.pro` : **elles doivent être essayées sur chaque générateur**, `make` et `nmake` ne se comportant
pas pareil devant une cible vide.

### macOS

Quatre pièges, dont le README dit l'essentiel. **Les deux derniers ne mordent plus** — le `.pro` a
depuis donné leurs dépendances aux règles concernées, voir juste après la liste — mais ils sont
gardés tels quels : ce sont eux qui expliquent ce que le `.pro` fait et pourquoi.

- le `qt@5` de Homebrew n'a **plus** QtWebEngine (Chromium troué), il faut le Qt 5.15.2
  officiel via `aqtinstall` ; **Hunspell se compile à la main** dans le `hunspell/` de la racine, comme
  sous Windows et par la recette du README — statique, x86_64 et `-mmacosx-version-min=10.13`
  explicites —, et zlib vient du système. Homebrew n'est plus qu'un repli du `.pro`, avec sa
  bibliothèque au nom versionné (`-lhunspell-1.7`) et les deux réserves dites plus bas.
  Conséquence à ne pas sous-estimer : le `qmake` du
  `PATH` est celui de Homebrew dès qu'il est installé, donc **le comportement par défaut des scripts
  tombait droit sur le seul Qt de la machine qui ne peut pas convenir** — c'est ce qui faisait échouer
  un `./dist-macos.sh` sans argument, sur un `Unknown module(s) in QT: webenginewidgets` qui ne
  désignait pas le coupable. `unix-common.sh` écarte maintenant les Qt sans
  `mkspecs/modules/qt_lib_webenginewidgets.pri` — chemin obtenu par `qmake -query QT_INSTALL_ARCHDATA`
  et non deviné, les mkspecs n'étant pas sous le dossier de Qt sur un Debian — puis va chercher dans
  `~/Qt/*/clang_64`, là où le README fait installer le Qt officiel. Il n'y a donc plus de chemin à
  passer. Un Qt **désigné** à la main, lui, n'est jamais remplacé en douce : s'il ne convient pas le
  script le dit et s'arrête ;
- le système de fichiers ignore la casse, donc un exécutable `RespawnIRC` ne pourrait cohabiter ni
  avec le dossier de sources `respawnIrc`, ni avec le `build/respawnIrc` où vont ses objets : on
  compile un bundle `RespawnIRC.app`, qui ne se confond avec aucun des deux, posé dans `build/`. Il
  **embarque `resources/` et `themes/`** dans `Contents/Resources`, par le `QMAKE_BUNDLE_DATA` du
  `.pro`, et se déplace donc d'un bloc : une compilation ordinaire donne le même bundle que celui
  qu'on distribue, il n'y a **qu'une seule disposition** et `pathTool::dataDirPath()` n'a rien à
  reconnaître. C'est cette fonction qu'il faut utiliser partout plutôt que
  `QCoreApplication::applicationDirPath()` ;
- conséquence du point précédent, à connaître : la règle qui recopie ces deux dossiers n'a que le
  dossier **source** pour dépendance, pas les fichiers dedans. Un thème modifié sans qu'un `.cpp`
  bouge ne parvenait donc pas au bundle sur un `make` lancé à la main — constaté, `Nothing to be
  done`. Depuis que les deux autres plateformes recopient elles aussi ces dossiers à côté de
  l'exécutable, le piège était le leur également ;
- **la règle qmake qui fabrique `Info.plist` n'a aucune dépendance**, si bien que `make` la sautait
  dès qu'un bundle était déjà là : un numéro de version changé dans `version.pri` ne parvenait pas au
  bundle, qui gardait celui de la compilation précédente. **Compiler hors des sources n'y change
  rien** — on l'a cru, et c'était l'unique justification du point 7 de
  `POSSIBLE-BUILD-SIMPLIFICATIONS.md` : la cible de cette règle est le bundle du `DESTDIR`, et pas un
  fichier du dossier de compilation. Un dossier neuf ne peut donc rien y faire. **Déplacer le
  `DESTDIR` dans `build/` n'y a rien changé non plus**, pour exactement la même raison : la cible
  suit le `DESTDIR`, elle est simplement passée de
  `<racine>/RespawnIRC.app/Contents/Info.plist:` à `../../build/RespawnIRC.app/Contents/Info.plist:`,
  toujours terminée sur un deux-points nu — relu dans le `Makefile` engendré après le déplacement.
  L'expérience qui l'avait établie, elle, date d'avant : depuis un `build/respawnIrc`, le contenu
  d'`Info.plist` remplacé par un texte quelconque, `make` répond `Nothing to be done` et laisse le
  fichier tel quel.

**Ces deux règles ont maintenant les prérequis qui leur manquaient**, et c'est le `.pro` qui les leur
donne : l'`Info.plist` du dépôt et `version.pri` pour la première, la liste récursive des fichiers de
`resources/` et `themes/` pour les deux autres. Le procédé tient en une propriété de `make` : une même
cible peut apparaître dans plusieurs règles tant qu'une seule porte des commandes, les prérequis
s'additionnant. Ces `QMAKE_EXTRA_TARGETS` n'ont donc **que** des dépendances, et laissent les
commandes à qmake. Deux choses à savoir avant d'y toucher :

- le nom de la cible doit s'écrire **exactement** comme qmake l'écrit dans le `Makefile`. `make`
  compare des chaînes et ne sait pas que deux chemins désignent le même fichier : une écriture
  différente donnerait une seconde cible dont personne ne dépend, qui ne servirait à rien **sans que
  rien ne le signale**. D'où le `$(DESTDIR)` du `.pro` — la variable du `Makefile` d'où sortent les
  règles du bundle — plutôt qu'un chemin reconstruit. Ce n'est pas théorique : le premier essai
  passait par `$$relative_path()`, qui donne le chemin le plus court quand qmake, lui, écrit le sien
  en repassant par le dossier des sources ; les deux désignaient le même fichier et `make` n'en a
  refait aucun ;
- la liste de fichiers est figée au moment du `qmake`, mais elle n'a pas à connaître les fichiers
  ajoutés depuis : c'est le dossier, que qmake garde comme prérequis, qui rattrape l'ajout et la
  suppression — sa date bouge dans ces deux cas et pas quand un fichier change. Les deux prérequis
  sont complémentaires, et la liste récursive comprend les sous-dossiers, sans quoi un sticker ajouté
  ne serait vu de personne. Il reste un trou, étroit et sans conséquence en pratique : un fichier
  **créé puis modifié** sans `qmake` entre les deux échappe aux deux prérequis — la date de son
  dossier ne rebouge pas pour une modification, et lui n'était pas là au `qmake` pour être dans la
  liste. Constaté en l'essayant exprès : un `touch` sur un fichier ajouté depuis ne déclenche rien.
  Sa création, elle, a bien été vue, donc il est dans le bundle ; et `build-unix.sh` relance `qmake`
  à chaque fois, ce qui referme le trou dès qu'on passe par lui.

Vérifié règle par règle sur un bundle en place, sans jamais l'effacer : thème modifié, fichier ajouté,
fichier supprimé, sticker modifié dans son sous-dossier, `version.pri` changé et `Info.plist` modifié
parviennent tous au bundle sur un simple `make`, et un `make` à vide répond toujours `Nothing to be
done`. Le `rm -rf build/RespawnIRC.app` de `build-unix.sh` n'est donc plus ce qui s'en garde : il
reste, mais pour la seule raison qui n'a rien à voir avec macOS — le `DESTDIR` ne distinguant pas les
dossiers de compilation, ce qui traîne dans `build/` peut venir d'ailleurs. **La conclusion du point 7
tenait donc à un correctif que personne n'avait écrit, et non à une impossibilité** ; ce qu'elle
établissait sur le `Makefile` engendré, en revanche, était juste.

Un piège de plus, qui n'appartient pas au projet et qui a failli faire conclure à un correctif
défaillant : le `make` d'Apple est un **GNU Make 3.81**, qui ne compare les dates qu'à la seconde. Un
fichier modifié dans la même seconde que la compilation précédente n'est pas vu, et ne le sera jamais
— attendre ne change rien, les deux dates restant égales. Quatre des vérifications ci-dessus ont
d'abord échoué pour cette seule raison, enchaînées trop vite dans un script ; rejouées avec une
seconde d'écart, elles passent toutes. Cela vaut pour les `.cpp` comme pour le reste, et c'est
exactement le genre de résultat qu'il ne faut pas prendre pour la preuve qu'une règle est mauvaise.

La compilation se fait **hors des sources**, dans `build/respawnIrc`, comme sur les deux autres
plateformes : c'est ce que fait `build-unix.sh` et ce que décrit le README.

`./dist-macos.sh ~/Qt/5.15.2/clang_64` fabrique le DMG distribuable : `macdeployqt`, remplacement de
`resources/` et `themes/` par leur version commitée, allègement, signature ad hoc, puis une image
contenant le seul `RespawnIRC.app` et un lien vers `/Applications`, la disposition qu'attend un
utilisateur de macOS. Le bundle est **autonome**, ce qu'interdisait l'époque où `imageDownloadTool` écrivait les
stickers dans `resources/stickers/` : ces dossiers ne pouvaient pas être enfermés en lecture seule,
d'où l'ancien dossier `RespawnIRC` contenant l'application **et** ses données. Depuis que les
stickers vont dans le cache, plus rien n'y écrit.

Cette autonomie est **vérifiée sur le DMG** : ses 39 binaires Mach-O n'ont aucun chemin absolu vers
`/Users`, `/opt/homebrew` ou `/usr/local`, et le seul `LC_RPATH` de l'exécutable est
`@executable_path/../Frameworks`. **Hunspell n'y est plus un binaire du tout** : il est lié
statiquement dans l'exécutable, comme sous Windows, depuis que le `hunspell/` de la racine porte un
`libhunspell.a` compilé à la main — le `libhunspell-1.7.0.dylib` que `macdeployqt` copiait auparavant
à côté des frameworks Qt a disparu, ce qui avait fait passer ce compte de 44 à 43 ; l'allègement
décrit plus bas en a emporté quatre autres, `QtSerialPort` et les trois greffons `position`. Ne
restent en dehors que le système : `/usr/lib/libz.1.dylib`, `libc++`, `libSystem` et les frameworks d'Apple. À savoir aussi, et
conséquence du Qt officiel utilisé et non des scripts : le binaire est **thin x86_64**, donc sous
Rosetta sur une machine Apple Silicon.

#### D'où vient le macOS 10.13 minimum, et le seul fichier qui ne le respecte pas

Le `LSMinimumSystemVersion` du bundle vaut 10.13, et **ce n'est ni un choix du dépôt ni un hasard** :
c'est Qt qui le fixe, à la ligne d'à côté de celle qui fixe l'architecture. `mkspecs/common/macx.conf`
porte `QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13` en 8 et `QMAKE_APPLE_DEVICE_ARCHS = x86_64` en 9 : deux
valeurs littérales, aucune déduite de la machine, et le même raisonnement vaut donc pour les deux — la
cible ne dépend pas de la machine qui compile. De là sort le `-mmacosx-version-min=10.13` de chaque
compilation et de chaque édition de liens, que qmake reprend en `MACOSX_DEPLOYMENT_TARGET` et
qu'`Info.plist` reprend à son tour par `${MACOSX_DEPLOYMENT_TARGET}`. **Rien dans le dépôt n'écrit ce
nombre**, et le descendre demanderait de recompiler Qt : ses frameworks précompilés portent eux-mêmes
10.13 dans leur load command, `QtCore` comme `QtWebEngineCore`. À savoir pour la migration : Qt 6.11
demande **macOS 13**, cinq versions majeures plus haut — c'est la ligne « macOS minimum » du tableau
de `MIGRATION-QT6.md`.

**Une pièce du bundle distribué démentait ce plancher, et c'était `libhunspell-1.7.0.dylib`** : il
annonçait `minos 14.0` là où tout le reste annonce 10.13. Ce n'était pas Qt mais Homebrew, qui compile
pour la machine où il tourne — le `hunspell` de `/usr/local` était ici un 1.7.3 sur un macOS 15.7.8.
Le DMG publié annonçait donc 10.13 en transportant une bibliothèque qui en demandait 14, et
**l'éditeur de liens n'en disait rien** : le journal d'une compilation complète ne portait aucun
avertissement du genre « built for newer macOS version ». Ce qui se serait réellement passé entre
10.13 et 13.x n'a jamais été connu, faute d'une machine de cette génération — et c'est le point de
méthode à retenir, **la question a été supprimée plutôt que répondue**.

**C'est fait** : Hunspell se compile maintenant à la main dans le `hunspell/` de la racine, que le
`.pro` prend en premier sans qu'on désigne rien, avec `-arch x86_64` et
`-mmacosx-version-min=10.13` explicites — la recette est dans le README, et c'est celle de Windows,
qui attaque `src/hunspell/*.cxx` sans passer par le `configure`. Elle donne un `libhunspell.a`, donc
**une bibliothèque statique et plus aucun binaire à déployer**. Vérifié sur le DMG refabriqué : 43
Mach-O, aucun chemin de la machine de compilation, et **aucun plancher autre que 10.13**. Deux choses
à ne pas confondre restent réglées par le même fichier : ce plancher, et le besoin d'un Hunspell
x86_64 de la section Apple Silicon. Le repli sur Homebrew que garde le `.pro` traîne toujours les
deux défauts, et ne doit être utilisé qu'en connaissance de cause.

**Mais « autonome » ne vaut que pour ce bundle-là**, et pas pour celui que laisse une compilation
ordinaire. Les deux ont bien la même disposition — c'est tout ce que dit le point du
`QMAKE_BUNDLE_DATA` plus haut, et il ne faut pas lui faire dire que le bundle du build est
distribuable — mais celui du build garde pour second `LC_RPATH` le `lib` du Qt de compilation, et
c'est par là qu'il trouve Qt. C'est voulu, et **il ne faut pas déplacer les étapes de `dist-macos.sh` dans
`build-unix.sh`** :

- ça coûterait une trentaine de secondes **à chaque compilation**, mesuré ici : 19,5 s pour
  `macdeployqt`, qui fait passer le bundle de 5,5 Mo à 205 Mo, puis 10,4 s pour le
  `codesign --deep` de ces 205 Mo. Et **sans rien économiser d'une fois sur l'autre**, le
  `rm -rf build/RespawnIRC.app` de `build-unix.sh` effaçant le bundle avant chaque `make` : il n'y a jamais
  de déploiement incrémental à reprendre, ces trente secondes s'ajouteraient telles quelles au
  rebuild d'un seul `.cpp` ;
- ça n'achèterait rien, le bundle de développement trouvant déjà Qt par ce second `LC_RPATH` ;
  l'autonomie ne sert qu'à sortir de la machine ;
- et deux des trois étapes seraient **nuisibles** en développement : le `git archive` remplacerait
  `resources/` et `themes/` par leur version commitée, donc effacerait à chaque compilation le thème
  qu'on est en train de modifier, et la signature serait de toute façon à refaire après chaque
  édition de liens.

Le seul besoin que ça couvrirait — essayer la disposition réellement distribuée — l'est déjà par
`./dist-macos.sh --skip-tests`.

Trois choses à ne pas défaire :

- **le `git archive` reste nécessaire alors que la compilation a déjà rempli le bundle.** Elle y
  copie le dossier de travail, où traîne ce que le mainteneur a accumulé en se servant du programme —
  les stickers qu'une version antérieure téléchargeait dans `resources/stickers/`, que rien ne
  distingue de ceux livrés. Le script efface donc les deux dossiers du bundle et les réextrait de
  `HEAD` ; c'est la même raison que sous Windows, et elle survit au fait que le `.pro` s'en occupe
  maintenant en temps ordinaire ;
- **ce remplacement doit précéder la signature.** `codesign` scelle le contenu du bundle ; un fichier
  changé après elle la casse, et macOS refuse alors de lancer l'application ;
- **l'allègement aussi**, et pour la même raison. Il vient juste avant la signature, et retire ce que
  `dist-windows.ps1` retire depuis longtemps de son côté — une décision prise pour une plateforme ne
  s'était pas propagée toute seule à l'autre.

Ce que l'allègement retire, et ce qu'il ne faut surtout pas y ajouter :

| Pièce | Poids | Pourquoi |
| --- | --- | --- |
| `qtwebengine_locales` sauf `fr` et `en-US` | 17 Mo, 51 fichiers sur 53 | le programme est en français, `en-US` est le repli de Chromium |
| `qtwebengine_devtools_resources.pak` | 1,5 Mo | les outils de développement de Chromium ne s'ouvrent jamais depuis le programme |
| greffons `position` et `QtSerialPort` | 0,3 Mo | rien n'utilise la géolocalisation ; `macdeployqt` les copie parce que QtWebEngine déclare le module, et `QtSerialPort` n'entre que par le greffon NMEA |

Le bundle passe de 208 à 190 Mo et l'image disque de 87 à 80. Trois choses à savoir avant d'y
toucher. **`QtPositioning` doit rester** : l'exécutable et `QtWebEngineCore` s'y lient pour de bon —
`otool -L` le dit — et le retirer empêcherait le programme de démarrer ; seuls les greffons, chargés
à la demande, s'en vont. Les `qt_*.qm` que `dist-windows.ps1` réduit **n'ont pas de pendant ici**,
`macdeployqt` ne les mettant pas dans le bundle. Et le reste du poids n'est pas là : sur les 208 Mo,
197 sont des frameworks et **164 le seul `QtWebEngineCore`**, dont 132 de binaire. Ces 18 Mo sont donc
à peu près tout ce qui se prend sans toucher à Chromium lui-même.

Le repli de Chromium sur `en-US` a été **essayé et non supposé**, puisque c'est le seul vrai risque de
cet allègement, et **les deux usages que le programme fait de QtWebEngine l'ont été aussi**, sur le
bundle extrait du DMG : « Ouvrir RespawnIRC Navigator » affiche jeuxvideo.com, le `QtWebEngineProcess`
lancé étant bien celui des `Helpers` du bundle, et « Se connecter » puis « Afficher la page de
connexion » rend la page de `/sso/login`. Relancé avec `LANG=ja_JP.UTF-8`, dont le `.pak` a disparu, le
programme affiche la même page avec un Chromium qui tourne en `--lang=ja` : le repli vaut donc pour
l'application elle-même et pas seulement pour un programme d'essai pointé sur ses ressources par
`QTWEBENGINE_RESOURCES_PATH` et `QTWEBENGINE_LOCALES_PATH`. Deux choses à savoir pour rejouer ce
parcours : il se pilote par `osascript` et System Events, ce qui demande l'**accès assistif** accordé à
l'outil depuis lequel on travaille — sans lui `osascript` répond `-1719` et rien ne se clique — et la
langue de Chromium suit aussi bien `LANG` que `-AppleLanguages`, les deux ayant donné `--lang=ja` ici.

Une chose vue au passage, qui n'a rien à voir avec l'allègement et qu'il ne faut pas lui imputer : le
navigateur interne écrit des `Uncaught SyntaxError` dans le journal sur les pages de jeuxvideo.com. Le
Chromium de Qt 5.15 date de 2020 et le site utilise une syntaxe plus récente ; les pages s'affichent
quand même. C'est un argument pour `MIGRATION-QT6.md`, pas un défaut du DMG.

#### Compiler sur un Mac Apple Silicon

**La cible ne dépend pas de la machine qui compile**, et c'est ce qu'il faut savoir avant de
raisonner sur ce sujet. Les mkspecs du Qt 5.15.2 officiel fixent `QMAKE_APPLE_DEVICE_ARCHS = x86_64`
en dur dans `mkspecs/common/macx.conf`, valeur littérale et non déduite de l'hôte ; `default_post.prf`
la transforme en un `EXPORT_VALID_ARCHS = x86_64` du `Makefile`, d'où sort un `-arch x86_64` explicite
dans `CXXFLAGS` et `LFLAGS`. Un Mac Apple Silicon produit donc le même bundle x86_64 qu'un Mac Intel,
sans rien à passer à `qmake`, et la question de savoir si `clang` s'exécute traduit ou natif ne se
pose pas : l'architecture est écrite sur sa ligne de commande. **Ne pas ajouter d'option
d'architecture** en croyant corriger quelque chose — elle ferait au mieux double emploi.

Ce qui manque vraiment tient en deux pièces, les deux en dehors du dépôt :

- **Rosetta 2.** `qmake`, `macdeployqt` et `respawnIrcTests` sont en x86_64 comme tout ce Qt, donc
  refusés par le noyau sans lui. Le piège n'était pas l'absence de Rosetta mais le **diagnostic** :
  `qtHasWebEngine` avalait la sortie d'erreur de `qmake` et rendait faux, si bien qu'un « Bad CPU type
  in executable » était rapporté comme un Qt sans QtWebEngine, avec le conseil d'installer par
  `aqtinstall` celui qui était déjà là. `unix-common.sh` distingue maintenant les deux, par une
  `qtSuitability` qui rend 1 pour un `qmake` qui ne démarre pas et 2 pour un Qt sans le module ; sur
  un hôte `arm64`, le message nomme Rosetta et donne `softwareupdate --install-rosetta`. Les cinq
  chemins ont été rejoués sur un Mac Intel — Qt désigné mort, Qt désigné sans module, vrai Qt,
  candidats tous morts, et le message d'Apple Silicon en interposant un faux `uname` dans le `PATH`.
  Le faux `qmake` rend **86**, qui est le code exact de ce refus ;
- **un Hunspell x86_64**, qui était le seul vrai obstacle et qui **n'en est plus un** : la recette du
  README compile Hunspell à la main avec `-arch x86_64` explicite, donc ce qu'elle produit convient
  sur les deux familles de Mac et il n'y a rien de particulier à faire ici. Ce qui suit ne vaut plus
  que pour le repli sur Homebrew, à éviter sur une machine Apple Silicon. `brew --prefix hunspell`
  désigne
  `/opt/homebrew` sur une telle machine, dont la bibliothèque est en arm64 et ne se lie pas à
  un binaire x86_64. Le `.pro` accepte donc un `HUNSPELL_DIR` sur la ligne de commande —
  il l'écrasait auparavant, l'affectation étant inconditionnelle — et **un dossier désigné n'est
  jamais remplacé en douce** par celui de Homebrew, le même principe que pour le Qt désigné aux
  scripts. Les deux sorties sont donc le Hunspell compilé à soi et posé dans le `hunspell/` de la
  racine, que le `.pro` prend sans qu'on désigne rien, ou le Homebrew de `/usr/local`
  (`HUNSPELL_DIR=/usr/local/opt/hunspell HUNSPELL_LIB_NAME=hunspell-1.7`). Le repli sur `brew`
  s'arrête en plus sur un `error()` quand il ne rend rien : sans lui, `LIBS` gardait un `-L/lib/` et
  l'échec n'apparaissait qu'à l'édition de liens, en « library not found » qui ne dit pas d'où venait
  ce chemin vide. Ce cas n'est pas que celui d'un Hunspell absent — un Homebrew de `/opt/homebrew`
  appelé depuis un `qmake` traduit refuserait de répondre, `uname -m` lui rendant `x86_64`.

**Rien de tout ceci n'a été essayé sur un Mac Apple Silicon**, faute d'en avoir un, et il ne faut pas
le présenter autrement. Ce qui est constaté l'a été sur un Mac Intel : la provenance du `-arch
x86_64`, relue dans les mkspecs et dans le `Makefile` engendré ; les cinq messages de résolution de
Qt ; les trois chemins de résolution de Hunspell — défaut Homebrew, `HUNSPELL_DIR` désigné, `brew`
muet — et un `./build-unix.sh -t` complet, 142 vérifications sans échec, rendant toujours un exécutable
`Mach-O 64-bit executable x86_64`. Le reste est raisonné : Rosetta traduisant le Chromium de
QtWebEngine, et le refus de Homebrew sous traduction. C'est exactement le genre de sujet où une
machine témoin est irremplaçable, comme pour les machines vierges de la section Windows.

### Windows

Le point de départ, dont tout le reste découle : **QtWebEngine n'existe pas pour MinGW**, Chromium
ne se compilant qu'avec MSVC. Le README détaille la mise en place, que `bootstrap-windows.ps1`
exécute d'un bloc sur une machine vierge — Build Tools, Qt, Hunspell, zlib et OpenSSL. Ce script ne
remplace pas le README : il applique ce qu'il décrit, et c'est le README qu'il faut lire quand une
version change ou qu'une étape échoue. La suite est scriptée elle aussi : `build-windows.ps1`
compile, avec `-Tests` pour les vérifications, `run-windows.ps1` lance le programme et appelle le
précédent si l'exécutable manque, et `dist-windows.ps1` fabrique l'archive en appelant lui aussi
`build-windows.ps1` — la compilation n'existe donc qu'à un seul endroit. Les pièges à connaître :

- les `.pro` n'ont **pas** été modifiés pour Windows, et ne devraient pas avoir à l'être : tout passe
  par la ligne de commande de `qmake`, où il ne reste que `DEFINES+=HUNSPELL_STATIC`. Les variables
  `HUNSPELL_LIB_NAME` et `ZLIB_LIB_NAME` existent toujours mais ne servent plus qu'aux bibliothèques
  autrement nommées — le `hunspell-1.7` de vcpkg, les bibliothèques de débogage — la recette du
  README produisant maintenant `hunspell.lib` et `zlib.lib`, qui sont les défauts des `.pro`. Les
  passer quand même ne fait rien, mais c'est du bruit : on les répétait à trois endroits pour rien.
  Ce `DEFINES` pourrait lui aussi devenir une ligne `win32:` du `.pro` ; ça a été proposé et
  **refusé**, précisément pour garder les `.pro` sans rien de spécifique à Windows ;
- l'installation ciblée des Build Tools demande **deux** composants, `VC.Tools.x86.x64` et un
  Windows SDK. Le premier seul pose `cl.exe` mais aucun `Windows Kits`, et plus rien ne compile :
  depuis Visual Studio 2015 les en-têtes de la bibliothèque C appartiennent au SDK et pas au
  compilateur, un `#include <stdio.h>` suffit à s'en rendre compte ;
- Hunspell et zlib se compilent **à la main**, c'est la méthode documentée : deux petites
  bibliothèques sans dépendance, une quinzaine de secondes. vcpkg reste décrit en second choix.
  Les deux se compilent **deux fois**, en `/MD` et en `/MDd` : `/MD` et `/MDd` ne se mélangent pas
  dans un même binaire, et les seules bibliothèques release faisaient échouer `nmake debug`. Le
  débogage se choisit au `qmake`, par `HUNSPELL_LIB_NAME=hunspelld ZLIB_LIB_NAME=zlibd`, et donc dans
  un dossier de compilation à part — les `.pro` restent sans rien de spécifique à Windows ;
- **les deux ne préviennent pas de la même façon, et zlib est le piège.** Hunspell est en C++ : ses
  en-têtes posent les enregistrements `RuntimeLibrary` et `_ITERATOR_DEBUG_LEVEL`, le mélange est un
  `LNK2038` et la compilation s'arrête. zlib est en C et n'en pose aucun : il ne reste que le
  `/DEFAULTLIB:MSVCRT` de ses objets release — visible au `dumpbin /directives zlib.lib`, quinze fois —
  qui ne donne qu'un `LNK4098`, un avertissement, et laisse **deux CRT dans le même binaire**. On a
  d'autant plus envie de l'ignorer qu'il n'empêche rien ; c'est pourtant le mélange exact que la
  section « Corruption de tas sous Windows » plus bas apprend à traquer. Constaté ici : la première
  version du correctif ne traitait que Hunspell, et l'avertissement était la seule trace du zlib
  release qui restait lié au binaire de débogage. **Ne pas conclure d'un `nmake debug` qui aboutit
  que les CRT sont cohérentes, lire les avertissements de l'édition de liens** ;
- `HUNSPELL_STATIC` est nécessaire au Hunspell compilé à la main, dont le `hunvisapi.h` teste
  vraiment la macro, mais sans effet sur celui de vcpkg, dont le port engendre un en-tête au test
  figé à `#if 1`. Le passer systématiquement marche donc dans les deux cas, et c'est ce que fait
  `build-windows.ps1`. Il ne va **pas** aux tests : `tests.pro` n'inclut que `zlib.pri`, ni Hunspell
  ni sa macro ne les concernent, et seul `ZLIB_LIB_NAME` leur est transmis quand il est donné ;
- la compilation se fait **hors des sources**, dans `build/`, comme sur les deux autres plateformes ; seuls
  les objets intermédiaires restent dans les sous-dossiers, le `DESTDIR` envoyant l'exécutable et ses
  `resources\`/`themes\` dans `build\` comme ailleurs.
  `build-windows.ps1` met `build\respawnIrc` pour le programme et `build\tests` pour les tests ;
- ce `DESTDIR` ne distingue pas release et debug : les deux produisent `RespawnIRC.exe` dans `build\` et
  **s'écrasent l'un l'autre**, même depuis deux dossiers de compilation séparés. Conséquence contre-intuitive,
  constatée : après un `nmake debug`, un `nmake release` dans son propre dossier **ne fait rien** — sa cible
  est l'exécutable de débogage laissé dans `build\`, plus récent que ses objets, donc jugé à jour. Il n'affiche
  rien et on continue d'exécuter le binaire de débogage en croyant l'avoir remplacé. Effacer `RespawnIRC.exe`
  avant de recompiler ; la taille tranche, 1,5 Mo en release contre 3,5 en debug. `build-windows.ps1` y échappe
  en effaçant ce seul `RespawnIRC.exe` avant son `nmake` : le lien est alors toujours refait, donc ce qui sort
  de là sort forcément des objets de son propre dossier. La protection valait pour la seule distribution
  quand elle vivait dans `dist-windows.ps1` ; elle couvre maintenant toute compilation scriptée, et le piège
  ne mord plus que sur ce qu'on tape à la main, la compilation de débogage en tête. Le script de distribution
  rasait auparavant tout `build\respawnIrc`, ce qui achetait la même garantie au prix des 45 sources à chaque archive.
  Le cas qui juge ce mécanisme a été exercé : un `nmake debug` avait laissé ses 3,5 Mo en place, et le
  `dist-windows.ps1` lancé ensuite a bien rendu un exécutable de 1,5 Mo, sorti des objets de son dossier.
  La **suite** du script, de `windeployqt` à la compression, a tourné depuis elle aussi : une archive complète
  a été fabriquée d'un bout à l'autre, 426 fichiers, 159 Mo décompressés et 71 compressés, avec les deux
  `resources/` bien fusionnés, la vérification au `dumpbin` à 98 imports du runtime sans manquant, et un
  `git status` vide à l'arrivée. Le même `DESTDIR` dépose aussi `RespawnIRC.pdb` à côté de l'exécutable,
  14 Mo — couverts par la seule règle `/build/` du `.gitignore` depuis que plus rien ne sort à la racine,
  là où il fallait auparavant une règle par artefact pour que `git status` reste vide après un `nmake debug` ;
- `windeployqt` crée un dossier `resources/` pour QtWebEngine, exactement le nom du dossier de
  données du programme, et au même endroit puisque `pathTool::dataDirPath()` renvoie le dossier de
  l'exécutable. Les deux contenus doivent **fusionner**, pas se remplacer ;
- `build-windows.ps1`, `dist-windows.ps1` et `run-windows.ps1` chargent `windows-common.ps1` par
  point-sourcing, où vivent la résolution du dossier de Qt, la vérification d'OpenSSL,
  `Import-MsvcEnvironment` et `Invoke-BuildTool`. Les deux dernières y sont descendues quand le script de
  compilation en est devenu le troisième utilisateur ; `Import-MsvcEnvironment` rend la main aussitôt si
  `nmake` répond déjà, ce qui rend sans coût son appel par un script et par celui qu'il appelle.
  `bootstrap-windows.ps1` **garde ses propres copies** des deux, et c'est délibéré : il tourne quand rien
  n'est encore installé, et il est le seul des quatre à avoir une raison de ne dépendre de rien ;
- **la résolution de Qt regarde dans `C:\Qt` quand personne n'a désigné de dossier**, comme
  `unix-common.sh` regarde dans `~/Qt/*/clang_64`, et pour exactement la même raison : le défaut des
  scripts doit tomber sur le Qt que le README fait installer. Elle ne le faisait pas, et l'écart se
  voyait mal parce qu'il ne mordait que sur la machine fraîchement amorcée : `bootstrap-windows.ps1`
  installe Qt dans `C:\Qt` et ne touche pas au `PATH`, si bien qu'un `.\dist-windows.ps1` sans argument
  échouait sur « Qt introuvable » au sortir de l'amorçage qui venait de l'installer. Les commandes que
  le bootstrap affiche en terminant portaient bien un `-QtDir`, donc elles marchaient — mais rien ne
  disait que c'était l'argument qui les sauvait, et le README annonçait par ailleurs que sans argument
  le `qmake` du `PATH` suffisait. Le bootstrap n'affiche plus ce `-QtDir` que lorsqu'il est vraiment
  nécessaire, c'est-à-dire pour un `-QtRootDir` hors du défaut ;
- **les Qt sans QtWebEngine sont écartés en le disant**, là encore comme sous Unix, et c'est le
  pendant Windows du `qt@5` de Homebrew : le Qt pour MinGW n'a pas le module, Chromium ne se compilant
  qu'avec MSVC, et c'est celui que trouve l'installateur en ligne de Qt quand on ne prend pas garde à
  l'architecture. Le module se cherche en demandant `QT_INSTALL_ARCHDATA` à `qmake` puis en regardant
  s'il y a un `mkspecs\modules\qt_lib_webenginewidgets.pri`, jamais en devinant un chemin. Sans ce
  test, un tel Qt était accepté et l'échec n'arrivait qu'au `Unknown module(s) in QT:
  webenginewidgets` de `qmake`, qui ne dit ni quel Qt a été pris, ni pourquoi celui-là n'ira jamais.
  Un Qt **désigné** par `-QtDir` n'est en revanche jamais remplacé en douce : s'il ne convient pas, on
  le dit et on s'arrête. Les six chemins ont été exercés — sans argument, Qt désigné valable, Qt
  désigné absent, Qt désigné sans le module, `qmake` qui ne démarre pas, et les deux cas où le `PATH`
  porte un mauvais Qt sur lequel la recherche enchaîne vers `C:\Qt`. Le Qt sans QtWebEngine a été
  simulé sans rien casser, par une copie de `qmake.exe` accompagnée d'un `qt.conf` qui déplace son
  `ArchData` vers un dossier vide ;
- les **cinq** `.ps1` du dépôt sont en UTF-8 **avec BOM** — `build-windows.ps1`, `dist-windows.ps1`,
  `run-windows.ps1`, `windows-common.ps1` et `bootstrap-windows.ps1` : PowerShell 5.1 lit un `.ps1` comme de l'ANSI sans
  lui, et tous les accents des messages sont abîmés. N'en réenregistrer aucun sans le BOM, et le
  vérifier sur les octets du fichier, pas à travers un pipeline PowerShell qui décode le texte et
  masquerait la perte ;
- toujours dans PowerShell 5.1, `qmake`, `nmake` et `windeployqt` écrivent leur progression sur la
  sortie d'erreur : avec `$ErrorActionPreference = 'Stop'` chaque ligne devient une erreur fatale
  alors que la commande a réussi. D'où `Invoke-BuildTool`, qui juge sur le code de retour.

#### Amorcer une machine vierge

`bootstrap-windows.ps1` enchaîne les cinq installations. Ce qui ne se devine pas depuis une machine
déjà équipée, et qui a coûté des essais :

- **la stratégie d'exécution par défaut d'un Windows 10 est `Restricted`**, donc un
  `.\bootstrap-windows.ps1` échoue avant d'afficher la moindre ligne. La commande à donner est
  `powershell -ExecutionPolicy Bypass -File .\bootstrap-windows.ps1`. Vérifié sur machine vierge :
  le script est bien refusé par un `PSSecurityException` / `UnauthorizedAccess`. Attention à ne pas
  conclure de `Get-ExecutionPolicy` qu'il n'y a pas de problème : un `Bypass` de portée `Process`,
  posé par l'outil depuis lequel on travaille, masque complètement le refus. Deux précisions, les
  deux constatées sur la machine vierge et les deux capables d'égarer le diagnostic :
  `Get-ExecutionPolicy -List` n'affiche **pas** le mot `Restricted` — les cinq portées y sont
  `Undefined`, `Restricted` n'étant que le défaut implicite des éditions client quand rien n'est
  posé ; et le `Bypass` de portée `Process` **se transmet aux processus enfants** par la variable
  d'environnement `PSExecutionPolicyPreference`, si bien que relancer un `powershell.exe` neuf ne
  révèle rien non plus. Pour voir le vrai comportement il faut vider cette variable, et c'est
  seulement alors que `Get-ExecutionPolicy` répond `Restricted`. **Cela vaut aussi pour les trois
  autres scripts**, ce qui s'est vu en suivant les lignes que `bootstrap-windows.ps1` affiche en
  terminant : elles donnaient un `.\build-windows.ps1` nu, refusé par un `PSSecurityException` sur la
  machine que le script venait pourtant d'amorcer. Elles reprennent maintenant le
  `powershell -ExecutionPolicy Bypass -File`. Ne pas essayer de ne l'afficher que si c'est nécessaire :
  le test se ferait dans un processus lancé en `Bypass`, donc il conclurait « inutile » exactement sur
  les machines qui en ont besoin ;
- **le script n'utilise pas git et ne récupère pas le dépôt** — il en fait partie, on l'a forcément
  déjà. Ce qui compte est la **façon** dont on l'a obtenu : `dist-windows.ps1` extrait `resources/` et
  `themes/` avec `git archive HEAD`, donc un zip décompressé, qui n'est pas un dépôt, fait échouer la
  distribution — tard, après une compilation complète. La compilation et `run-windows.ps1`, eux,
  marcheraient. Sur un zip, la marque de provenance de Windows bloque en plus le script même avec
  `Bypass` (`Unblock-File`). Ne pas réécrire ceci en « git est un prérequis du script » : ça a été
  écrit, et ça fait chercher un `git clone` qui n'existe nulle part ;
- **l'installation des Build Tools demande confirmation, et c'est la seule étape qui le fasse.** La
  raison n'est pas seulement son poids — 3,3 Go, un quart d'heure, le processeur occupé : c'est que
  l'invite UAC arrive **dans la seconde** qui suit le `Start-Process`, si bien que la ligne
  « élévation nécessaire » s'affichait sans qu'on ait le temps de la lire. Constaté, et c'est ce qui a
  fait ajouter le `Read-Host`. Le message dit aussi ce que l'invite annonce — « Visual Studio
  Installer », éditeur vérifié « Microsoft Corporation » : accorder une élévation à un installateur
  nommé n'est pas la même chose que l'accorder à un script dont on ne sait pas ce qu'il élève. Ces
  deux libellés ont été vus à l'écran et retrouvés dans le binaire, `FileDescription` pour le premier
  et sujet de la signature Authenticode pour le second, sur le `setup.exe` que pose l'installation —
  c'est le même bootstrapper que `vs_BuildTools.exe`, ce qui permet de rejouer l'invite sur une
  machine déjà équipée sans rien retélécharger. Le bloc est **dans la branche
  qui installe**, donc rien n'est demandé si les Build Tools sont déjà là ou avec `-SkipBuildTools` —
  la réentrance garde sa valeur, une reprise ne redemande rien. `-Yes` s'en passe. Ne pas déplacer ce
  bloc en tête de script : il redemanderait à chaque reprise, ce que la conception évite depuis le
  début ;
- **seule l'installation des Build Tools demande l'élévation**, et le script n'élève que cet
  installateur-là, par un `-Verb RunAs` qui déclenche une invite UAC : un PowerShell ordinaire
  suffit donc pour tout le script. N'élever que l'installateur n'est pas un détail — le reste
  continue dans le processus d'origine, et ce qui s'écrit dans le dépôt et dans `C:\Qt` appartient à
  l'utilisateur courant, alors qu'un script entièrement relancé élevé laisserait des fichiers
  d'administrateur dans le dépôt. L'invite **n'apparaît que si les Build Tools manquent** : le test
  de `vswhere.exe` rend la main avant, donc une machine déjà équipée ne demande rien. Une reprise,
  ou un `-SkipBuildTools`, n'élève rien non plus ;
- `aqt` écrit un `aqtinstall.log` dans le **dossier courant**. Le script l'appelle donc depuis
  `build\bootstrap` : sans ce `Push-Location`, le fichier atterrit à la racine du dépôt et apparaît
  dans `git status`. Ne pas « simplifier » ce détour ;
- le script est **réentrant**, chaque étape se sautant si son résultat est déjà là. C'est ce qui rend
  une reprise après échec sans douleur — utile si les Build Tools rendent 3010, code qui signale un
  redémarrage conseillé et que le script traite comme un succès, mais qui pourrait demander un vrai
  redémarrage avant que `cl.exe` fonctionne.

Compter une trentaine de minutes et environ 4,5 Go, dont 3,3 pour les seuls Build Tools et 0,9 pour
Qt avec QtWebEngine. Le reste est négligeable. La durée dépend surtout de la machine et du réseau, et
l'estimation est large : sur une machine rapide, l'installation des Build Tools a pris deux minutes et
demie et le script entier moins de dix. Ne pas s'inquiéter d'un écart dans un sens comme dans l'autre.

**Les cinq étapes ont maintenant tourné**, l'installation des Build Tools comprise : exécutée par le
script sur une machine virtuelle vierge — ni MSVC ni Qt, `vswhere.exe` absent — puis suivie de la
compilation du programme et des tests pour vérifier que la chaîne obtenue sert vraiment à quelque
chose. Elle avait longtemps été la seule branche jamais empruntée, faute d'une machine où
désinstaller les Build Tools pour réessayer. Les quatre autres ont été rejouées, dossiers effacés, y
compris celle de Qt — qui, la première fois qu'on l'a vraiment lancée, a révélé les deux défauts
corrigés ci-dessus.

**La branche `-Verb RunAs` a maintenant tourné elle aussi**, et avec elle la combinaison qui restait
neuve — sur une machine vierge **suivante**, distincte de celle du paragraphe ci-dessus, et sous
Windows 10 IoT Enterprise LTSC 2021. Le script lancé depuis un PowerShell **ordinaire** y a affiché sa
ligne « élévation nécessaire », l'invite UAC a été acceptée, l'installateur élevé a rendu 0 et les
quatre étapes suivantes ont continué dans le processus d'origine. Ce qui se déduisait de la conception est
désormais constaté : `C:\Qt`, `qmake.exe`, `hunspell.lib`, la bibliothèque de zlib et les DLL d'OpenSSL appartiennent
à l'utilisateur courant, quand `C:\Program Files (x86)\Microsoft Visual Studio` appartient à
`BUILTIN\Administrators`. C'est exactement ce que n'élever que l'installateur cherche à obtenir, et
c'est la raison de ne pas « simplifier » en relançant tout le script élevé.

Du même coup, **les étapes 3 à 5 ont travaillé sans élévation** pour la première fois — elles avaient
jusqu'ici tourné pour de bon dans un processus élevé et s'étaient sautées dans un processus ordinaire,
jamais l'inverse. Rien n'a bronché : compilation de Hunspell et de zlib, extraction d'OpenSSL,
vérification finale, `git status` vide à l'arrivée, aucun `aqtinstall.log` à la racine et
`build\bootstrap` bien effacé.

**Le refus de l'invite a maintenant été essayé lui aussi**, sur une machine vierge et depuis un
PowerShell ordinaire : le `catch` se déclenche, le script s'arrête sur son message et rend 1, et la
machine reste un témoin valable — ni `vswhere.exe` ni `msvcp140.dll` dans `System32`, `git status`
vide. Seul `build\bootstrap\vs_BuildTools.exe` subsiste, que le `.gitignore` couvre et que la
réentrance réutilise à la reprise : le `Remove-Item` final du dossier de téléchargement ne s'exécute
évidemment pas, le script s'étant arrêté avant.

Ce qu'il a fallu corriger, et qu'aucun raisonnement n'aurait donné : **le refus n'est pas
reconnaissable.** Il arrive en `InvalidOperationException` « This command cannot be run due to the
error: The operation was canceled by the user. », **sans exception interne** — le `Win32Exception`
1223 y est aplati en texte. Il n'y a donc aucun code à tester, et le message suit la langue de
Windows. Or ce `catch` attrape tout autant un installateur introuvable ou corrompu : il affirmait le
refus, il rapporte maintenant la cause telle quelle et laisse la conduite à tenir, refus compris. **Ne
pas y remettre un diagnostic que rien ne permet de poser.**

Et ce n'est pas qu'une question de type d'exception : **trois causes différentes rendent la même
exception, au caractère près.** Le bouton « Non », la croix de l'invite, et l'invite laissée intacte —
qui expire d'elle-même au bout de **deux minutes** — donnent tous le `The operation was canceled by
the user.` ci-dessus. Mesuré : 4,4 s pour la croix, 122,4 s pour l'expiration. Windows ne distingue
donc pas « l'utilisateur a refusé » de « il n'y avait personne devant la machine », et le script ne le
peut pas davantage. Deux conséquences pratiques : un `bootstrap-windows.ps1` lancé sans surveillance
sur une machine qui a besoin de l'élévation **échoue au bout de deux minutes** au lieu d'attendre, et
la valeur d'un essai de cette branche tient entièrement à savoir qui était devant l'écran. C'est le
piège qui a été rencontré ici : deux des exécutions ont d'abord été présentées comme des refus
volontaires alors qu'elles n'étaient que des expirations, le message étant identique.

Ce qui **reste supposé** dans cette étape, et à ne pas présenter autrement : le traitement du code de
retour 3010 comme un succès. Les deux installations observées ont rendu 0 — les journaux de
l'installateur disent `Exit code: 0x0, restarting: No` — et rien n'a encore exigé le redémarrage que
3010 conseille.

Et une conséquence à connaître avant de vouloir tout enchaîner sur la même machine : **installer les
Build Tools détruit la seule propriété qui rendait cette machine utile.** L'installation pose
`msvcp140.dll`, `msvcp140_1.dll` et toute leur famille dans `System32` — vérifié, ils y sont après et
n'y étaient pas avant. La machine qui vient d'être amorcée ne peut donc plus servir à essayer
l'archive : le chargeur y trouvera dans `System32` ce que l'archive aurait oublié d'embarquer. Relever
l'inventaire de `System32` **avant** de lancer le script, ou garder une seconde machine pour l'essai.
C'est ce piège, et pas un autre, qui a laissé sortir l'archive incomplète décrite plus bas.

La leçon de méthode vaut au-delà de ce script : **sur une machine déjà équipée, toute étape
d'installation se saute et se déclare bonne sans avoir rien fait.** Pour l'essayer, effacer sa cible
ou la détourner vers un dossier jetable. C'est aussi ce qui rend une machine vierge irremplaçable —
et la même propriété sert maintenant à l'invite UAC, qui ne peut pas apparaître sur une machine dont
le `vswhere.exe` est déjà là.

#### Ce qui manque et ne se voit pas

La cible est **Windows 10 64 bits ou plus récent**. `windeployqt` ne copie **ni OpenSSL ni les
bibliothèques d'exécution de MSVC**, ce sont les deux seules pièces à ajouter à la main :

- **sans OpenSSL, aucune page n'est joignable.** Qt 5.15.2 charge `libssl-1_1`/`libcrypto-1_1` à
  l'exécution ; en leur absence `QSslSocket::supportsSsl()` est faux et tout échoue silencieusement.
  Qt ne distribue plus le 1.1.1, seulement un OpenSSL 3 incompatible. Pour vérifier, un programme de
  trois lignes affichant `supportsSsl()` et `sslLibraryVersionString()` suffit et évite de deviner ;
- sans `vcruntime140.dll` et compagnie, le programme ne démarre pas là où le redistribuable n'a
  jamais été installé. **Ces DLL-là ne partent pas avec Windows 7** : elles ne font partie d'aucun
  Windows. Le test qui tranche, sur une machine vierge : `ucrtbase.dll` est dans `System32`,
  `msvcp140.dll` et `vcruntime140.dll` n'y sont pas. Attention en le faisant : `System32` contient
  bien un `msvcp140_clr0400.dll` et un `vcruntime140_clr0400.dll`, copies privées du .NET Framework
  qui ne servent à rien ici — un `dir msvcp140*` trouve donc quelque chose et peut faire conclure
  l'inverse de la vérité.

**Et « compagnie » compte pour de bon.** Le script figeait une liste de trois noms —
`vcruntime140.dll`, `vcruntime140_1.dll`, `msvcp140.dll` — alors que `Qt5Core.dll` et `Qt5Widgets.dll`
importent aussi **`msvcp140_1.dll`**. Résultat : toute archive sortie d'ici échouait sur une machine
sans redistribuable avec « The code execution cannot proceed because MSVCP140_1.dll was not found »,
constaté sur un Windows 10 LTSC 2019 propre. Le point important pour ne pas se tromper sur la cause :
cette dépendance vient des **binaires précompilés de Qt 5.15.2**, pas de la compilation faite ici.
Elle ne dépend donc ni de la version des Build Tools ni d'un changement récent — elle est là depuis
que ce Qt existe, et toutes les archives précédentes en manquaient. `dist-windows.ps1` copie
maintenant tout le dossier `Microsoft.VC*.CRT`, dix DLL pour 1,8 Mo, et vérifie avant de compresser
qu'aucun import de la famille du runtime ne manque à l'archive. Relevé au `dumpbin` : sur les dix,
**quatre sont réellement importées** — `msvcp140.dll`, `msvcp140_1.dll`, `vcruntime140.dll` et
`vcruntime140_1.dll` — et `msvcp140_1.dll` était la seule des quatre à manquer. Les six autres ne sont
importées par rien ; elles sont copiées quand même, 1,1 Mo sur 158 valant mieux qu'une liste de noms à
tenir à jour. **L'archive corrigée a été essayée sur une machine virtuelle vierge sous Windows 10 LTSC
2019, celle où la précédente échouait, et le programme fonctionne.**

#### Ce que l'abandon de Windows 7 a retiré

Deux pièces ont disparu de l'archive avec la cible Windows 7 — 47 fichiers et 6,4 Mo relevés avec le
SDK 10.0.26100, chiffre à ne pas figer puisque le nombre de DLL de l'UCRT suit la version du SDK :

- l'**Universal CRT** (`ucrtbase.dll`, les `api-ms-win-crt-*` et `api-ms-win-core-*`), composant du
  système depuis Windows 10 et que Windows 7 n'obtenait que par la mise à jour facultative
  KB2999226. Ne pas s'étonner de ne pas trouver les DLL de redirection dans `System32` : sous
  Windows 10 ce ne sont pas des fichiers, ces noms sont résolus par le schéma d'*API sets* du noyau ;
- **`D3Dcompiler_47.dll`**, nécessaire au rendu ANGLE de Qt, fourni par Windows 10. `windeployqt`
  continue de le copier avec le lot ANGLE, `dist-windows.ps1` le retire après coup. Attention en
  relisant : `opengl32sw.dll` n'est plus là pour rattraper un ANGLE en panne, il a été retiré lui
  aussi, pour ses raisons propres (plus bas).

Ce que cela a supprimé de plus précieux, c'est la partie la plus fragile du script. Le redistribuable
de l'Universal CRT n'était pas au même endroit selon la version du SDK — `Redist\<version>\ucrt\DLLs\x64`
pour les récents, `Redist\ucrt\DLLs\x64` pour les anciens — et cette recherche à deux dispositions
avait déjà coûté une archive silencieusement incomplète, du temps où elle ne se protégeait que par un
`Write-Warning` noyé dans la sortie de `windeployqt`. Tout ce bloc est parti. **Ne pas le
réintroduire sans réintroduire d'abord Windows 7 comme cible** : c'était la seule pièce dont
l'absence ne se voyait sur aucune machine où l'on pouvait essayer l'archive, et c'est exactement ce
qui la rendait dangereuse.

#### Ce que les releases d'amont ont vraiment livré

Ce dépôt a longtemps expliqué la pile de DLL de l'Universal CRT comme celle « historiquement
distribuée avec le programme », justifiée et non du gras. **C'est faux, et il ne faut pas réécrire
cette histoire.** Les archives publiées sur `franckrj/respawnirc` ont été inspectées : aucune ne
contient `ucrtbase.dll` ni le moindre `api-ms-win-*`. Elles diffèrent d'ailleurs sur presque tout :

- elles sont en **32 bits** (`vc_redist.x86.exe`, `libssl-1_1.dll` sans suffixe `-x64`), quand ce
  dépôt vise le 64 bits ;
- Hunspell y est **dynamique** (`libhunspell.dll`), ici il est statique ;
- pour les bibliothèques d'exécution de MSVC, elles embarquent **l'installateur**
  `vc_redist.x86.exe` (13,7 Mo) plutôt que les DLL elles-mêmes. C'est le fichier que `windeployqt`
  ajoute dès que `VCINSTALLDIR` est définie, et exactement celui que `--no-compiler-runtime` écarte
  ici.

Deux enseignements qui portent au-delà de l'anecdote. D'abord, `vc_redist.x86.exe` et
`opengl32sw.dll` apparaissent **dans la même release**, la v3.1.11 de juillet 2019, en même temps
qu'un changement de version de Qt : ce sont deux ajouts automatiques de `windeployqt`, pas des
décisions. Ensuite, les v3.1.6 à v3.1.10 ont été distribuées **avec QtWebEngine et ANGLE mais sans
`opengl32sw.dll`**, pendant environ un an et demi, sans problème signalé — ce qui corrobore la mesure
faite ici sur une machine sans accélération. Voir `POSSIBLE-BUILD-SIMPLIFICATIONS.md`.

Le gras est ailleurs, et `dist-windows.ps1` s'en occupe : traductions de QtWebEngine et de Qt
réduites au français, outils de développement de Chromium retirés, une vingtaine de mégaoctets.
`opengl32sw.dll` (20 Mo) a été **retiré**, et ce n'est pas une pièce Windows 7 : il est parti pour une
raison à lui. On lisait ici que sans pilote OpenGL utilisable il était le seul recours ; c'était faux.
Le défaut de Qt bascule sur ANGLE, qui traduit en Direct3D 11 et, faute de GPU, tombe sur WARP, le
rasteriseur logiciel de Windows — le repli est donc déjà dans le système, une couche plus bas.
Vérifié sur une machine virtuelle sans aucune accélération : `GL_RENDERER` vaut `ANGLE (Microsoft
Basic Render Driver Direct3D11 vs_5_0 ps_5_0)` et QtWebEngine affiche correctement une page sans ce
fichier. Les releases v3.1.6 à v3.1.10 d'amont ont d'ailleurs été distribuées ainsi pendant un an et
demi (voir plus bas). Il ne servait plus que si ANGLE échouait, ou si `QT_OPENGL=software` était forcé
à la main — auquel cas Qt affiche « Failed to create OpenGL context » et s'arrête. **Ne pas le
remettre sans une machine réelle qui le réclame** : c'est le plus gros fichier de l'archive après
Chromium.

Le plus gros morceau était plus sournois : `windeployqt` embarque `vc_redist.x64.exe`, 24 Mo que
rien ne lance jamais et qui font doublon avec les DLL du runtime copiées à côté de l'exécutable. Il
ne le fait que si `VCINSTALLDIR` est définie, donc seulement quand le script tourne après
`vcvars64.bat` — d'où une archive dont le poids dépendait de la façon de l'appeler, et un fichier
qui n'apparaît pas si on essaie `windeployqt` à la main dans un shell neuf. D'où le
`--no-compiler-runtime`, à ne pas retirer.

Répartition de ce qui reste, pour situer les ordres de grandeur : sur 159 Mo décompressés (71 Mo
compressés, 426 fichiers), **environ 124 tiennent à QtWebEngine**, soit 78 %. Chromium lui-même en
fait 112 (`Qt5WebEngineCore.dll` seul en pèse 97, le reste étant `icudtl.dat` et ses fichiers
`.pak`), QtQuick, QML et WebChannel 8, et ANGLE 3 (`libGLESv2.dll` et `libEGL.dll`). Attention au
raisonnement : RespawnIRC est une application Widgets, qui dessine en raster et n'utilise ni QML ni
OpenGL — tout cela n'est là que parce que WebEngine s'en sert. Le client lui-même, avec Qt Core, Gui,
Widgets, Network, OpenSSL et les runtimes, pèse une trentaine de mégaoctets.

Ces chiffres sont ceux de l'archive Windows 10. Celle qui visait Windows 7 en faisait 184 avec les
mêmes composants : 6 Mo de différence tiennent à l'Universal CRT et à `D3Dcompiler_47.dll`, et 20 au
seul `opengl32sw.dll`. Le poids n'était la raison d'aucun des trois retraits, mais il explique que le
dernier soit le plus visible.

Windows 7 n'est plus une cible, et n'avait de toute façon **jamais été essayé** : sa compatibilité
était raisonnée d'après la documentation de Microsoft, sans machine pour la vérifier.

Il a longtemps été écrit ici que le démarrage du programme depuis l'archive, sous Windows 10 et sur
une machine virtuelle vierge, était « la seule chose à présenter comme vérifiée ». **C'était faux, et
c'est instructif.** L'archive manquait `msvcp140_1.dll` depuis toujours, et sur un Windows 10 LTSC
2019 réellement propre elle ne démarre pas du tout. La machine qui avait servi à l'essai n'était donc
pas vierge de ce qui compte : elle avait le redistribuable Visual C++, presque certainement parce que
les Build Tools y avaient été installés — ce qui le pose dans `System32`, comme constaté depuis. Le
constat « ça démarre » était juste, sa portée non : il ne disait rien de l'archive, seulement de cette
machine-là.

La leçon est la même que celle du bas de la section précédente, appliquée cette fois non pas à une
étape d'installation mais à l'essai final : **une machine cesse d'être un témoin valable dès qu'on y
installe ce dont on veut prouver l'absence.**

**L'archive corrigée a maintenant été essayée pour de bon**, sur une machine virtuelle vierge sous
Windows 10 LTSC 2019 — la configuration exacte où la précédente échouait sur `MSVCP140_1.dll` — et le
programme fonctionne. C'est ce qui manquait : le relevé au `dumpbin`, fait contre l'inventaire de
`System32` pris avant l'installation des Build Tools, disait seulement qu'aucun import statique du
runtime C++ ne manquait. Il ne pouvait rien dire de ce qui se charge par `LoadLibrary`, OpenSSL
compris. Les deux se complètent et aucun ne remplace l'autre : le `dumpbin` tourne à chaque archive et
attrape la régression tout de suite, l'essai sur machine vierge est le seul à juger le résultat entier
mais demande qu'on y pense.

#### Pourquoi Hunspell et zlib ne passent pas par vcpkg

Les deux méthodes ont été mesurées sur la même machine, avec le même compilateur, et donnent un
programme identique — les 142 vérifications passent dans les deux cas. À la main : 2,4 Mo
téléchargés, une quinzaine de secondes, 44 Mo sur le disque. Avec vcpkg : une dizaine de minutes et
912 Mo, CMake, 7zip, PowerShell Core et un MSYS2 complet compris.

L'écart vient surtout de **libiconv, qui mange les deux tiers des dix minutes sans servir à rien
ici** : c'est une dépendance du paquet vcpkg de Hunspell, pas de Hunspell tel que RespawnIRC
l'utilise. La bibliothèque compilée à la main s'en passe et tout fonctionne. Et c'est le pire genre
de dépendance à compiler sous Windows, un `configure` autotools passé par un bash MSYS2 : des
milliers de compilations d'une ligne lancées les unes après les autres, d'où un processeur au
ralenti et une attente qui ne ressemble pas à du travail.

L'argument des mises à jour ne tient pas non plus : Hunspell sort une version tous les deux à quatre
ans (1.7.0 en 2018, 1.7.2 fin 2022, 1.7.3 en mai 2026), et la surface d'attaque que corrigent ces
versions, ce sont les dictionnaires malformés — or RespawnIRC embarque les siens.

vcpkg garderait l'avantage avec son cache binaire, en intégration continue, ou le jour où le projet
prendrait des dépendances plus lourdes.

#### Ce qui reste à faire pour Windows

Chacun de ces points est détaillé à sa place, ici ou dans les autres fichiers ; cette liste ne sert
qu'à les rassembler et à dire lesquels comptent. Le portage lui-même est fini : l'amorçage, la
compilation, l'archive et son démarrage sur machine vierge sont tous constatés. Ce qui reste tient en
un seul vrai point, l'autre étant réglé.

- ~~**`nmake debug` ne compile pas.**~~ **Fait.** `bootstrap-windows.ps1` compile Hunspell **et zlib**
  une seconde fois en `/MDd /Z7`, vers un `hunspelld.lib` et un `zlibd.lib` à part. La section
  « Corruption de tas sous Windows » plus bas n'a donc plus à se rabattre sur
  `CONFIG+=force_debug_info`. Voir la section Windows plus haut pour les deux `..._LIB_NAME`, le
  dossier de compilation séparé qu'ils imposent, et surtout le `LNK4098` de zlib, qui est ce que
  cette tâche avait de moins évident : le correctif que CLAUDE.md décrivait — Hunspell seul — laissait
  une CRT release dans le binaire de débogage sans que rien n'échoue ;
- **OpenSSL 1.1.1 n'est plus maintenu depuis septembre 2023 et il est distribué tel quel.** Aucun
  rangement de la chaîne de compilation n'y touche : il faut Qt 6, ou recompiler Qt 5.15.2 avec
  `-schannel` pour le TLS de Windows. C'est le sujet de `MIGRATION-QT6.md`, et le seul point restant
  qui soit une exposition et non du confort. À l'échelle de « ce qui reste pour Windows », il pèse
  plus lourd que tout le reste réuni : ce qui subsiste, au fond, c'est d'être arrimé à un Qt et à une
  bibliothèque TLS tous deux en fin de vie. **La migration est néanmoins repoussée**, décision du
  mainteneur : le projet reste sur Qt 5.15.2 et OpenSSL 1.1.1 pour l'instant, l'exposition étant connue
  et assumée. Les conclusions de `MIGRATION-QT6.md` tiennent et son ordre de travail est inchangé, mais ne pas
  entreprendre le portage sans le mainteneur. Ce document a été relu ligne à ligne le 31 juillet 2026, code en
  main : des numéros de ligne avaient dérivé, sa table des API supprimées était incomplète — il lui manquait le
  `QTextStream::setCodec` de `tests/main.cpp`, qui est justement dans la première étape de son plan — et deux de
  ses justifications étaient fausses, celle du retrait de FFmpeg et celle de la durée de vie libre d'une branche
  LTS. Aucun arbitrage n'a changé de sens pour autant. La leçon est celle du dépôt tout entier : **une analyse
  dont les conclusions sont bonnes peut avoir des détails faux**, et ce sont les détails qu'on recopie sans les
  revérifier le jour où on l'exécute.

Les pistes de `POSSIBLE-BUILD-SIMPLIFICATIONS.md` étaient du confort et rien n'y cassait si elles
attendaient ; **les onze sont maintenant faites**, la dernière étant la compilation hors des sources
sous macOS du point 7 — dont l'argument s'est révélé faux, le `rm -rf RespawnIRC.app` qu'elle
promettait de supprimer restant nécessaire pour la raison expliquée dans la section macOS plus haut.
Ce qu'il faut en retenir tient en une ligne : le numéro de version vit dans `version.pri`,
les noms de bibliothèques ne se passent plus à `qmake` que s'ils sortent de l'ordinaire,
`windows-common.ps1` porte ce que les trois scripts partagent, `build-windows.ps1` est le seul endroit
où la compilation est écrite, la distribution reprend les objets au lieu de les raser et lance les
tests avant d'assembler, et l'archive se compresse par `ZipFile`. Le fichier a été allégé d'autant : il
ne garde que la piste ouverte, les décisions à ne pas rouvrir, les réserves et les leçons — le récit de
ce qui a été fait est ici ou dans le README, et la version longue dans l'historique git. **La réserve
qui pesait sur le point 3 est levée** : `build-windows.ps1 -Tests` a compilé pour de bon, sur une
machine amorcée par `bootstrap-windows.ps1` — 142 vérifications sans échec, `RespawnIRC.exe` à 1,52 Mo,
soit la taille attendue d'un release. Deux avertissements sortent au passage, les deux dans
`utilityTool.cpp:32` et antérieurs à tout ceci : un `C4100` sur un paramètre inutilisé et un `C5051`
disant que `[[maybe_unused]]` demanderait `/std:c++17`. Restent enfin deux réserves qui ne sont pas des tâches, seulement des choses
à ne pas oublier : le code de retour 3010, toujours non constaté et de faible valeur — il ne diffère
de 0 que par un redémarrage conseillé — et le retrait d'`opengl32sw.dll`, vérifié sur une machine sans
accélération mais pas sur une machine dont Direct3D 11 serait cassé ou désactivé, cas qu'une machine
virtuelle ordinaire n'exerce pas. Ce dernier restera probablement un risque documenté. Le refus de
l'invite UAC, qui figurait ici, est maintenant constaté — et il n'était pas gratuit : il a montré que
ce refus ne se reconnaît pas, et fait corriger le message qui l'affirmait.

## Où le programme range ses données

`resources/` et `themes/` sont **livrés avec le programme et jamais écrits**, à côté de
l'exécutable — sous macOS dans le `Contents/Resources` du bundle. Ce que le programme écrit se répartit en **trois
rôles**, qui atterrissent là où chaque système les attend :

| Rôle | Contenu | Windows | Linux | macOS |
| --- | --- | --- | --- | --- |
| configuration | `config.ini` | `userdata/` | `~/.config/RespawnIRC/` | `~/Library/Application Support/RespawnIRC/` |
| données | `user_fr.dic`, `resources/shortcut.txt` | `userdata/` | `~/.local/share/RespawnIRC/` | idem ci-dessus |
| cache | `resources/stickers/`, `logs/` | `userdata/` | `~/.cache/RespawnIRC/` | `~/Library/Caches/RespawnIRC/` |

Sous Windows les trois rôles retombent sur un unique `userdata/` à côté de l'exécutable : le
programme y est distribué en archive à décompresser où l'on veut, il doit donc rester portable. Cet
`#ifdef Q_OS_WIN` est **le seul endroit** du programme qui distingue les plateformes pour les
chemins ; tout le reste passe par les fonctions ci-dessous.

Les logs sont dans le cache et non dans les données : ils ne sont écrits qu'avec
`RESPAWNIRC_DEBUG`, et les perdre ne coûte rien. La spécification XDG les rangerait plutôt dans
`XDG_STATE_HOME`, mais Qt 5.15 n'a pas de `StateLocation` et il faudrait lire la variable
d'environnement à la main, pour un gain nul ici.

### Pourquoi un `.ini` et pas un plist sous macOS

La configuration est un `config.ini` écrit par un `QSettings` en `IniFormat`, le même sur les trois
plateformes, à un chemin décidé par `pathTool`. La question du plist a été étudiée et **le `.ini`
est gardé** ; ce qui suit a été mesuré avec un petit programme Qt 5.15.2, pas raisonné :

- `QSettings::NativeFormat` n'est pas « le format de macOS » mais celui de chaque plateforme. Sous
  **Windows c'est le registre**, ce qui détruirait le `userdata/` portable de l'archive ; sous
  **Linux c'est le même moteur INI qu'`IniFormat`**, au seul suffixe `.conf` près, donc gain nul —
  et surtout ce n'est **pas** dconf/GSettings, Qt n'ayant pas de greffon pour eux. Le seul endroit
  où `NativeFormat` change vraiment quelque chose est donc macOS, et le seul qu'il casserait,
  Windows ;
- par domaine, le plist est **binaire**, son chemin échappe à `pathTool` (`QSettings::fileName()`
  est en lecture seule), et **cfprefsd devient la source de vérité à la place du fichier** :
  constaté, juste après un `sync()` le plist sur disque n'avait qu'une clé sur quatre quand
  `defaults read` les affichait toutes. Copier, éditer ou effacer le fichier ne suffit alors plus ;
- l'organisation étant laissée vide exprès (voir `main.cpp`), le nom obtenu est
  `com.trolltech.unknown-organization.RespawnIRC.plist`. Un `setOrganizationDomain("franckrj.com")`
  donne `com.franckrj.RespawnIRC.plist`, soit exactement le `CFBundleIdentifier` de l'`Info.plist`
  depuis qu'il est capitalisé, et **sans perturber `QStandardPaths`** — mesuré, `AppDataLocation` et
  `CacheLocation` ne bougent pas. Attention à ne pas prendre `setOrganizationName` pour l'équivalent :
  celui-là **insère un niveau de dossier** dans tous les chemins de `QStandardPaths`
  (`~/Library/Application Support/<Org>/RespawnIRC`, `~/.config/<Org>/RespawnIRC`), donc déplacerait
  configuration, données et cache sur macOS comme sous Linux et demanderait une migration de plus.
  C'est ce que l'organisation laissée vide évite ;
- ce qu'un plist apporterait est mince : de vrais tableaux au lieu des listes échappées et l'UTF-8
  stocké tel quel au lieu des `h\xe9h\xe9` du `.ini`. Les valeurs sans équivalent plist restent des
  blobs `@Variant(...)` dans les deux formats, ce qui couvre les `QByteArray` de géométrie. Et la
  moitié de ce gain tomberait toute seule le jour d'un passage à Qt 6, dont l'`IniFormat` écrit en
  UTF-8 par défaut — voir `MIGRATION-QT6.md`.

Ne pas rouvrir sans une raison neuve — préférences gérées par MDM, scriptage `defaults`, un
composant natif à qui parler. **La capitalisation du `CFBundleIdentifier` n'en est pas une** : elle
n'a levé que l'objection cosmétique, le domaine du plist coïncidant maintenant avec le bundle, et
laisse les quatre autres intactes. Il faudrait de plus écrire l'import des `config.ini` existants, et
ce fichier porte les cookies de connexion, les listes de pseudos et les couleurs : une migration
ratée ne perd pas du confort, elle déconnecte les comptes. Si un plist était malgré tout voulu, la
seule variante saine est `QSettings(chemin, NativeFormat)`, qui écrit un plist XML à l'endroit choisi
sans passer par cfprefsd.

Enfin, sur `~/Library/Preferences`, que le commentaire de `pathTool::configDirPath()` écarte : y
poser le `.ini` ne **casserait** rien, cfprefsd ne touchant qu'aux fichiers qu'il reconnaît comme
domaines. C'est une question de rangement — Apple y réserve les plists de son système de
préférences et destine `Application Support` à ce que l'application gère elle-même — et
`AppConfigLocation` y créerait de toute façon un **sous-dossier** `RespawnIRC` dans un dossier censé
être plat.

| Fonction de `pathTool` | Usage |
| --- | --- |
| `dataDirPath()` | les données livrées, en lecture seule |
| `configDirPath()` / `configFilePath()` | la configuration |
| `userDataDirPath()` | ce que l'utilisateur a écrit |
| `cacheDirPath()` | les stickers téléchargés |
| `logDirPath()` | `cacheDirPath() + "/logs"` |
| `dirPathsForReading()` | tous les dossiers de lecture, dans l'ordre, sans doublon |
| `pathForReading(rel)` | les données de l'utilisateur si le fichier y est, sinon celles livrées |
| `pathForWriting(rel)` | toujours les données de l'utilisateur, dossiers parents créés au passage |

**Utiliser `pathForReading` pour tout ce qui peut avoir été écrit** (`shortcut.txt`, dictionnaires)
et `pathForWriting` pour toute écriture. `dataDirPath()` ne reste direct que pour ce qui n'est
jamais écrit : sons, images de tags, thèmes.

Les dossiers d'écriture **reproduisent la disposition des données livrées** :
`<données>/resources/shortcut.txt`, `<cache>/resources/stickers/`. Ce niveau `resources/` au milieu
d'un dossier de cache a l'air arbitraire, mais il est obligatoire : voir plus bas.

Pourquoi cette séparation : les scripts de distribution embarquaient les données du mainteneur,
`resources/shortcut.txt` et les stickers téléchargés en se servant du programme. Aucune règle de nom
ne pouvait les exclure, un sticker téléchargé étant dans le même dossier et de la même forme
(`<id>.png`) qu'un sticker livré. Les scripts extraient donc maintenant `resources/` et `themes/`
avec `git archive HEAD` — uniquement ce qui est commité — et ne copient jamais ce qui est écrit.

`pathTool::migrateOldUserDataIfNeeded()`, appelé au tout début de `main`, déplace vers ces dossiers
ce que les versions antérieures laissaient à côté de l'exécutable. `QCoreApplication` doit avoir un
`applicationName` **avant** tout appel à `pathTool` : c'est de lui que `QStandardPaths` déduit tous
les chemins, et il est posé dans `main` juste après la construction de `QApplication`.

### Stickers : lecture dans plusieurs dossiers

237 stickers sont livrés dans `resources/stickers/`, les nouveaux vont dans le cache, et ceux qu'une
version antérieure avait téléchargés sont restés parmi les premiers — ils ne sont pas déplaçables,
rien ne les en distingue. Trois endroits consultent donc tous les dossiers de lecture :

- `imageDownloadTool::basePathsForReading()`, pour ne pas retélécharger un sticker déjà là
  (`basePathForWriting()` désigne, lui, le seul dossier où écrire, le cache) ;
- `selectStickerWindow`, qui les liste tous, dédoublonne, puis retrie — la concaténation de listes
  triées ne l'est plus ;
- les `setSearchPaths()` de `showTopic` et `selectStickerWindow`, qui reçoivent `dirPathsForReading()`.

**Ne pas toucher à la forme des chemins dans le HTML des messages.** `parsingTool.cpp` écrit
`<img src="resources/stickers/<id>.png">` et `respawnIrc.cpp` relit exactement ce préfixe par regex
pour refabriquer `[[sticker:p/<id>]]` au moment de citer. C'est la raison pour laquelle les dossiers
d'écriture reproduisent la disposition de `resources/` au lieu d'être à plat, y compris le cache :
les mêmes chemins relatifs résolvent partout, sans rien changer au HTML ni aux regex qui le
relisent. C'est aussi pourquoi `dirPathsForReading()` rend des **racines** et non des sous-dossiers
de stickers.

## Logs et diagnostic

Rien n'est journalisé par défaut. Avec `RESPAWNIRC_DEBUG=1` :

```bash
RESPAWNIRC_DEBUG=1 ./RespawnIRC
```

- toutes les catégories `respawnirc.*` passent en debug,
- les logs sont écrits dans `<cache>/logs/respawnirc.log`, soit `userdata/logs/` sous Windows et
  `~/.cache/RespawnIRC/logs/` sous Linux (voir `pathTool::logDirPath()`),
- les pages dont l'analyse échoue sont sauvegardées à côté, en `page-*.html` (20 max).

Le chemin exact est affiché au démarrage, inutile de le deviner.

Pour n'activer qu'une catégorie sans écrire de fichier :
`QT_LOGGING_RULES="respawnirc.parsing.debug=true" ./RespawnIRC`.
Catégories : `respawnirc.network`, `.parsing`, `.topic`, `.forum` (voir `logTool.hpp`).

### Corruption de tas sous Windows : deux pièges de diagnostic

Un plantage en `0xC0000374` au démarrage a coûté cher à comprendre, pour deux raisons qui se
reproduiront :

- **le programme cesse de planter sous débogueur.** Après quelques plantages, Windows applique de
  lui-même le shim *Fault Tolerant Heap* à l'exécutable et compense la corruption. `cdb` l'annonce
  par une ligne `FTH: (...) Fault tolerant heap shim applied to current process`, facile à manquer
  au milieu des `ModLoad`. L'entrée est dans `HKLM\SOFTWARE\Microsoft\FTH\State`, en écriture
  protégée ; le plus simple est de tester depuis un autre chemin, la liste étant indexée par chemin
  d'exécutable ;
- **le plantage est signalé loin du bug**, à la libération d'un bloc et non à l'écriture fautive.
  Activer le page heap complet (`gflags /p /enable RespawnIRC.exe /full`, à désactiver ensuite) fait
  fauter à l'endroit exact. Comme `release` n'a pas de symboles, recompiler pour obtenir une pile
  lisible plutôt que des `image00007ff6+0x...` : un vrai `nmake debug` est maintenant possible
  (`HUNSPELL_LIB_NAME=hunspelld ZLIB_LIB_NAME=zlibd`, dans son propre dossier de compilation), et `CONFIG+=force_debug_info`
  reste utile quand on veut les symboles sans quitter les optimisations de release.

Les outils viennent du Windows SDK, feature `OptionId.WindowsDesktopDebuggers` de `winsdksetup.exe`,
qui ne fait pas partie de l'installation des Build Tools.

## Comment jeuxvideo.com sert ses pages depuis la refonte de 2026

C'est le point le plus important du dépôt, et ce qui avait cassé l'affichage des
messages : **le HTML d'une page de forum ne contient plus ni les messages ni la liste
des sujets**. Tout est dans un objet JSON encodé dans la page :

```html
<script>window.jvc=window.jvc||{};window.jvc.forumsAppPayload="<base64(gzip(json))>";</script>
```

(Côté JVC : `base64_encode(gzencode(json_encode(...), 6))`. Le gzip date du 11 juin 2026.)

`payloadTool::getPayload(source)` s'occupe de l'extraction, du décodage et du cache.
Toutes les fonctions de `parsingTool` qui ont besoin du payload le demandent avec la
source de la page ; le cache évite de décompresser six fois la même page.

Clés utiles du payload :

| Clé | Contenu |
| --- | --- |
| `listMessage` | les messages d'une page de topic |
| `listTopics` | les sujets d'une page de forum |
| `topicTitleField` | nom du topic |
| `pagerView` | `currentPage`, `pageCount`, et une liste **partielle** de pages |
| `forumInfo.header.btnVal` | nombre de connectés |
| `forum.isForumReadOnly` / `forum.lockReason` | topic fermé |
| `formSession` | les champs cachés du formulaire d'envoi |
| `ajaxToken`, `ajaxModerationToken` | jetons ajax |
| `messageEditor` | options de l'éditeur |

Un message de `listMessage` : `id`, `text` (source brute), `renderedText` (HTML),
`publishedAuthorName`, `publishedAuthorRole` (vide pour un utilisateur normal),
`publishedAuthorAvatar` (URL complète), `publishedDate` (« 02 juin 2026 à 22:25:06 »),
`updatedDate` / `updatedText`, `publishedAuthorSignatureRendered`, `stateMessage`.

Attention : `pagerView.pages` ne liste que les pages proches de la page courante puis
de dix en dix. Sur un topic de 1300 pages, la dernière page n'y est pas — il faut la
reconstruire à partir de n'importe quel lien (`parsingTool::buildLinkForThisPage`).

### HTML des messages (`renderedText`)

Les anciennes classes ont toutes changé. Correspondances :

| Avant | Depuis 2026 |
| --- | --- |
| `bloc-spoil-jv` / `contenu-spoil` | `message__spoil` / `message__spoilContent` |
| `pre-jv` / `code-jv` | `message__pre` / `message__code` |
| `blockquote-jv` | `message__blockquote` |
| `img-shack` | `message__urlImg` |
| `<p>` | `<p class="message__p">`, `<p class="message__noBlankline">` |
| `<i></i><span>` (césure de lien) | `<i class="message__cesure"></i><span class="message__middleCesure">` |

Les regex de `parsingTool.cpp` acceptent les deux formes, au cas où des pages
anciennes traînent. Deux pièges rencontrés :

- une image noelshack postée seule est rendue **sans balise `<img>`** : un `<span>`
  vide avec l'URL dans `data-src-background`, suivie d'une accolade parasite
  (`...jpg"}"`). `normalizeLargeNoelshackImages()` la ramène à la forme normale.
- JVC écrit `target="_blank""` avec un guillemet en trop dans les liens d'image.

### Actions sur un message : ne rien reconstruire

Chaque message du payload porte un objet `actions` (nul si on n'est pas connecté) qui
dit **ce que le site autorise** et donne l'URL à appeler :

```json
"actions": {
  "quote":  {"label": "Citer le message"},
  "edit":   {"url": "/forums/message/edit?...&ajax_hash=..."},
  "delete": {"url": "/forums/message/delete?ids=...&type=delete&ajax_hash=..."},
  "blacklist": {"url": "..."}, "report": {"url": "..."}, "privateMessage": {"url": "..."}
}
```

Deux pièges qui ont chacun causé un bug :

- **Être l'auteur d'un message ne veut pas dire pouvoir l'éditer.** JVC ne fournit pas
  toujours l'action `edit` sur nos propres messages (fenêtre d'édition fermée). Les
  boutons éditer/supprimer se décident donc sur la présence de `editUrl`/`deleteUrl`,
  pas sur une comparaison de pseudo.
- **Le `ajax_hash` des URL d'action n'est pas `ajaxToken`.** Ce sont deux jetons
  différents dans la même page. Il faut utiliser l'URL fournie telle quelle plutôt que
  d'en fabriquer une avec un hash récupéré ailleurs.

Ces URL sont relatives : `parsingTool::makeAbsoluteUrl` leur ajoute le domaine déduit
du payload. `showTopic` garde texte source et URL d'action des messages affichés dans
`listOfInfosForActions`.

### Modifier un message : deux requêtes, deux sessions de formulaire

`actions.edit.url` n'est **pas** une cible d'envoi (y poster répond 404), c'est une URL
à lire en GET :

```
GET /forums/message/edit/form-values?id_message=<id>&ajax_hash=<ajaxToken>
→ {"formSession":{...,"fs_version":"forum_edit_message"},"needsCaptcha":false,
   "text":"<texte actuel>","ajaxToken":"..."}
```

Puis seulement :

```
POST /forums/message/edit   (multipart, comme message/add)
    text, topicId, forumId, group, messageId
    + les champs de la formSession renvoyée ci-dessus
    + ajax_hash = ajaxToken renvoyé ci-dessus
→ {"html":"<p>...</p>","formSession":{...}}   (pas de champ d'erreur = accepté)
```

Le point important : la `formSession` de l'édition vaut `fs_version:
"forum_edit_message"`, alors que celle de la page vaut `topic_nouveau_message`.
Réutiliser celle de la page ne marche pas. C'est `parsingTool::getEditFormValues` qui
lit la première réponse, et `sendMessages` qui envoie la seconde.

### Supprimer un message : POST, pas GET

`actions.delete.url` s'appelle en **POST avec un corps vide** (tout est déjà dans
l'URL : `ids`, `type`, `ajax_hash`). En GET, le site répond 404 — comme pour l'édition,
l'apparence d'un lien ne veut pas dire qu'on peut l'ouvrir en GET.

```
POST /forums/message/delete?ids=<id>&type=delete&ajax_hash=<hash>
→ {"errors":[],"success":["Le message #<id> a été supprimé."]}
```

`errors` vide vaut succès ; attention à ne pas confondre `success` avec un message
d'erreur. La suppression étant irréversible et le lien minuscule, `showTopic` demande
confirmation avant d'envoyer.

### Réponses aux actions

`message/add`, `message/edit` et `message/delete` répondent en JSON. Le succès n'a pas
une forme unique, alors que l'échec porte toujours un champ `erreur`/`errors` non vide :
`parsingTool::getErrorOfMessageSending` considère donc que tout ce qui n'annonce pas
d'erreur est passé. C'est l'inverse qui produisait le bug « le message n'a pas été
envoyé » sur un message pourtant posté.

### Ce qui a disparu

Ces URL renvoient maintenant 404 :

- `forums/ajax_citation.php` → la citation se fait **localement**, à partir du champ
  `text` du payload (gardé par `showTopic` dans `listOfRawMessages`).
- `forums/ajax_edit_message.php` → même chose pour récupérer le texte à éditer.
- `forums/modal_del_message.php` → remplacé par `forums/message/delete?ids=...`.

L'envoi d'un message ne se fait plus en POST urlencodé sur l'URL du topic, mais en
**multipart** sur `forums/message/add` (ou `/edit`), avec les champs `text`, `topicId`,
`forumId`, `group`, `messageId`, plus tout `formSession` et `ajax_hash`.

La page de connexion est passée de `/login` à `/sso/login`.

## Cloudflare

jeuxvideo.com est derrière Cloudflare et **refuse curl** (403 avec l'en-tête
`cf-mitigated: challenge`), même avec des en-têtes de navigateur complets. En revanche
la pile réseau de Qt passe. Pour récupérer une page de test, écrire un petit programme
Qt avec `QNetworkAccessManager` plutôt que de se battre avec curl — voir
`parsingTool::buildRequestWithThisUrl` pour la requête exacte utilisée par le
programme (HTTP/2 activé, ce qui apaise Cloudflare).

`getTopicMessages` détecte l'en-tête `cf-mitigated` et affiche « Entravé par
Cloudflare » plutôt que de faire semblant que le topic n'existe pas.

## Tests

`tests/` contient un petit harnais maison (pas QtTest) qui rejoue le parsing sur de
vraies pages sauvegardées dans `tests/fixtures` (stockées en `.html.gz` pour ne pas
alourdir le dépôt, décompressées à la volée avec `payloadTool::gzipUncompress`).

Le harnais écrit en UTF-8, ce que la console de Windows n'est pas par défaut : `main` y
passe donc la page de codes en UTF-8 le temps des tests, puis **remet celle d'origine**.
C'est un réglage de la console et non du processus, il survivrait sinon au programme et
laisserait la fenêtre en UTF-8 pour tout ce qu'on y taperait ensuite. Constaté de bout en
bout : les accents sortent droits dans une vraie console, la page de codes d'origine est
bien rendue — essayé en partant d'une console en 850 — et `dumpbin /imports` montre les
deux appels dans le binaire. Ce dernier contrôle n'est pas de trop : la restauration seule
ne prouve rien, un bloc qui n'aurait pas été compilé donnant exactement le même résultat.

Pour ajouter une fixture : récupérer la page avec un programme Qt (cf. Cloudflare),
`gzip -9` le résultat dans `tests/fixtures`, puis écrire les vérifications dans
`testParsing.cpp`. Les fixtures actuelles sont figées dans le temps (juillet 2026),
donc les valeurs attendues (nombre de messages, pseudos, ids) sont stables.

## État des fonctions qui demandent d'être connecté

- **Envoi d'un message** : confirmé, le message part. Le faux « message non envoyé » qui
  suivait est corrigé (lecture JSON de la réponse).
- **Affichage du nombre de MP** : confirmé.
- **Édition** : les deux requêtes ont été confirmées à la main sur un vrai message (le
  site a accepté la modification). Le parcours complet dans l'interface, lui, n'a pas
  été rejoué faute de message encore éditable au moment des tests.
- **Suppression** : la requête a été confirmée sur un vrai message (supprimé pour de
  bon). Le parcours dans l'interface, boîte de confirmation comprise, n'a pas été
  cliqué en vrai.

`showDeleteButton` reste à `false` par défaut dans `settingTool.cpp`, choix du
mainteneur d'origine depuis 2016. Une confirmation existe désormais avant l'envoi, donc
le défaut pourrait être rediscuté, mais il n'a pas été changé.

Pour vérifier : poster un message, puis tenter l'action tout de suite avec
`RESPAWNIRC_DEBUG=1`. Le log contient la requête (noms des champs seulement, jamais les
valeurs des jetons) et la réponse du site.

## Attention aux fixtures

Ne **jamais** figer telle quelle une page récupérée en étant connecté : au-delà du
payload, le HTML contient un `ajax_hash` de session. `tests/fixtures/topic-connecte.html.gz`
est une page fabriquée à la main, avec des jetons factices, pour tester les actions.

De la même façon, aucune fixture ni aucun commentaire ne doit contenir le pseudo JVC
du mainteneur, ni un chemin absolu de sa machine : les pseudos des fixtures sont des
noms inventés (`MembreDeTest`, `AutreMembre`), et les ids de topic et de message qui
les accompagnent sont eux aussi factices.

## Conventions

- Commentaires et messages de commit en français, sans retour à la ligne manuel.
- Style du dépôt : `if(x == true)`, accolades sur leur propre ligne, noms en
  `camelCase` suffixés (`...Class`, `...Struct`, `expFor...`).
- Le `.gitignore` couvre les objets de compilation ; ne pas committer `RespawnIRC`
  ni `userdata/`.
- Fins de ligne en **LF partout**, imposées par `.gitattributes` (`* text=auto eol=lf`) et non
  par le `core.autocrlf` de la machine, qui n'a donc plus d'effet. Ne pas réintroduire de règle
  `eol=crlf` : rien ici n'en a besoin. Le seul piège d'encodage qui reste est le BOM des `.ps1`.
