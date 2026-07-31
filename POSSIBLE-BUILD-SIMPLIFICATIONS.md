# Pistes de simplification de la compilation et de la distribution

Relevé de juillet 2026 sur `respawnIrc.pro`, `tests/tests.pro`, `zlib.pri`, les scripts Windows, `dist-macos.sh` et la section « Compilation » du README. Les onze pistes sont faites, et une douzième est venue après coup — l'absence de script de compilation côté Unix, décrite plus bas. Il n'en reste aucune d'ouverte.

Ce fichier a donc été ramené à ce qui n'existe qu'ici : les décisions prises, les réserves qui restent et les leçons que ces pistes ont coûtées. Le détail de ce qui a été fait vit maintenant là où il sert — `CLAUDE.md`, le README, les commentaires des scripts — plutôt qu'en double. La version longue, avec les états des lieux d'origine, leurs chiffres et leurs numéros de ligne, est dans l'historique git ; c'est là qu'il faut aller pour savoir à quoi ressemblait la chaîne avant, et le commit qui a allégé ce fichier est le point de départ.

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

**Suite, et elle confirme l'analyse ci-dessus** : le `DESTDIR` du programme est depuis passé de la racine à `build/`, de sorte que plus rien du tout n'atterrit à la racine, sur aucune plateforme. La cible de la règle `Info.plist` a suivi le `DESTDIR` comme prévu — `../../build/RespawnIRC.app/Contents/Info.plist:`, toujours sur un deux-points nu — donc l'effacement reste nécessaire, et pour la même raison qu'avant. Ce que ce déplacement a réellement rapporté est ailleurs : `resources/` et `themes/` voyagent maintenant avec l'exécutable sur les trois plateformes et non plus sous le seul macOS, et le `.gitignore` a perdu ses quatre règles de racine et l'exception `!/respawnIrc/` qui allait avec.

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
- **une piste peut être bonne et son argument faux.** Le point 7 promettait la disparition d'un `rm -rf` que rien ne faisait disparaître : le `Makefile` engendré le disait, personne ne l'avait ouvert. Lire ce que produisent les outils plutôt que raisonner sur ce qu'ils devraient produire — c'est valable pour `qmake`, et ça l'a déjà été pour `dumpbin` et `windeployqt` ;
- **une machine cesse d'être un témoin valable dès qu'on y installe ce dont on veut prouver l'absence.** Installer les Build Tools pose `msvcp140.dll` et sa famille dans `System32` : la machine qui fabrique l'archive ne peut donc jamais la valider. C'est le mécanisme qui a laissé sortir deux archives silencieusement incomplètes.

## Ce qu'il ne faut pas toucher

- `Invoke-BuildTool` ressemble à de la cérémonie, mais c'est la plus petite correction juste au fait que PowerShell 5.1 transforme la sortie d'erreur des outils natifs en erreurs fatales. S'en passer voudrait dire abandonner `$ErrorActionPreference = 'Stop'`, ce qui serait pire.
- `--no-compiler-runtime`, les DLL du runtime C++ de MSVC, le BOM des `.ps1` : tous documentés, tous justifiés. Les DLL du runtime ne sont pas parties avec Windows 7 et ne doivent pas être confondues avec ce qui l'a fait — elles ne sont dans aucun Windows. Ne pas non plus chercher à les réduire à celles qui sont importées aujourd'hui : c'est cette économie-là, 1,1 Mo, qui a produit une archive incapable de démarrer.
- `opengl32sw.dll` a longtemps figuré ici, au titre du rendu de secours des machines sans pilote OpenGL. Il en est sorti, et de l'archive avec : ce rôle est tenu par ANGLE et WARP, pas par lui. C'est le rappel utile de cette section — une entrée y était depuis des années sur une justification que personne n'avait vérifiée.
