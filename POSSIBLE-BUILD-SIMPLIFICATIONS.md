# Pistes de simplification de la compilation et de la distribution

Relevé de juillet 2026 sur `respawnIrc.pro`, `tests/tests.pro`, `zlib.pri`, les scripts Windows, `dist-macos.sh` et la section « Compilation » du README. Sur onze pistes, **dix sont faites** ; il n'en reste qu'une d'ouverte, le point 7.

Ce fichier a donc été ramené à ce qui n'existe qu'ici : la piste ouverte, les décisions prises, les réserves qui restent et les leçons que ces pistes ont coûtées. Le détail de ce qui a été fait vit maintenant là où il sert — `CLAUDE.md`, le README, les commentaires des scripts — plutôt qu'en double. La version longue, avec les états des lieux d'origine, leurs chiffres et leurs numéros de ligne, est dans l'historique git ; c'est là qu'il faut aller pour savoir à quoi ressemblait la chaîne avant, et le commit qui a allégé ce fichier est le point de départ.

## La piste encore ouverte

### 7. macOS compile dans les sources, Windows non

`dist-macos.sh` a besoin d'un `rm -rf RespawnIRC.app` uniquement parce que la règle qmake qui fabrique `Info.plist` n'a aucune dépendance et que `make` la saute dès qu'un bundle est déjà là. Compiler dans `build/` comme sous Windows rend ce contournement inutile : un dossier neuf ne peut pas traîner un `Info.plist` périmé.

C'est du confort, rien ne casse si ça attend.

## Ce qui a été fait, et où c'est documenté maintenant

| # | Piste | Le détail est passé dans |
| --- | --- | --- |
| 1 | Le numéro de version analysé trois fois, dans trois langages | `version.pri`, et `CLAUDE.md` pour la contrepartie : plus de compilation sans qmake |
| 2 | Trois options `qmake` sous Windows dont aucune n'avait besoin d'exister | `CLAUDE.md` et le README : il ne reste que `DEFINES+=HUNSPELL_STATIC`, les deux `..._LIB_NAME` ne servant plus qu'aux noms inhabituels |
| 3 | Pas de script de compilation, seulement amorçage, distribution et lancement | `build-windows.ps1`, son en-tête, `CLAUDE.md` et le README |
| 4 | Une trentaine de lignes dupliquées entre les scripts PowerShell | `windows-common.ps1` et son en-tête |
| 5 | Chaque distribution recompilait tout | `CLAUDE.md` et le README : objets repris, `-Clean`, et l'exécutable effacé avant `nmake` |
| 6 | Les bibliothèques d'exécution de MSVC figées en dur | `CLAUDE.md` (« Ce qui manque et ne se voit pas ») et le README |
| 8 | L'installation des outils Visual Studio et son redistribuable | `CLAUDE.md` (« Ce que l'abandon de Windows 7 a retiré ») |
| 9 | Deux listes de « trois choses » qui ne se recouvraient pas | résolu par l'abandon de Windows 7, commentaires de `dist-windows.ps1` |
| 10 | La compression | `dist-windows.ps1` : `ZipFile::CreateFromDirectory`, 7,6 s contre 13,5 |
| 11 | `opengl32sw.dll`, 20 Mo pour un repli que Windows fournit déjà | `CLAUDE.md`, le README et le commentaire de `dist-windows.ps1` |

Le point 6 est de loin le plus instructif des dix, et il vaut d'être lu dans l'historique : rangé ici en « deux détails » pour son gain de quelques lignes de script, il cachait l'absence de `msvcp140_1.dll`, c'est-à-dire une archive qui ne démarrait sur aucune machine sans redistribuable Visual C++.

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

- **`build-windows.ps1` n'a encore rien compilé pour de bon** (point 3). Seul son enchaînement a été essayé, avec un `windows-common.ps1` simulé et un faux `qmake.exe` affichant ses arguments : les deux cibles dans leurs dossiers, les options réellement transmises, le code de retour des tests, `-Clean`, l'erreur quand `qmake` manque, l'appel depuis `run-windows.ps1` et celui depuis `dist-windows.ps1` jusqu'à `windeployqt`. C'est l'enchaînement qui est vérifié, pas la compilation ;
- **`ZipFile::CreateFromDirectory` écrit des antislashs** dans les noms d'entrées de l'archive (point 10), mesuré sur ce .NET Framework 4.8 alors qu'on attendait des `/`. Ce qui sauve le changement, c'est que le `Compress-Archive` de PowerShell 5.1 en écrivait exactement autant : la forme des noms ne change pas et les archives publiées en ont toujours contenu. Mais la spécification ZIP demande des `/`, et un décompresseur non-Windows peut en sortir des fichiers dont le nom contient des antislashs plutôt qu'une arborescence. Si ça devenait un sujet, ce serait un défaut préexistant et non une régression.

## Les leçons, qui portent au-delà de ces pistes

- **une affirmation entre parenthèses est une affirmation.** « Ce dossier ne contient que les trois DLL voulues plus `concrt140.dll` » avait la forme d'une observation et n'était qu'une supposition ; un `dir` l'aurait démentie en une seconde, et elle a coûté une archive qui ne démarrait pas ;
- **classer une piste par son gain la classe mal quand le risque est ailleurs.** Le même point était rangé en « deux détails » à cause du gain, quelques lignes de script ; sa non-réalisation coûtait bien plus ;
- **ne pas figer de chiffres venant d'une installation** — nombre de DLL, numéro de version des outils, noms de fichiers : ils suivent la version du SDK ou des Build Tools. Chercher par glob, copier le dossier entier ;
- **une machine cesse d'être un témoin valable dès qu'on y installe ce dont on veut prouver l'absence.** Installer les Build Tools pose `msvcp140.dll` et sa famille dans `System32` : la machine qui fabrique l'archive ne peut donc jamais la valider. C'est le mécanisme qui a laissé sortir deux archives silencieusement incomplètes.

## Ce qu'il ne faut pas toucher

- `Invoke-BuildTool` ressemble à de la cérémonie, mais c'est la plus petite correction juste au fait que PowerShell 5.1 transforme la sortie d'erreur des outils natifs en erreurs fatales. S'en passer voudrait dire abandonner `$ErrorActionPreference = 'Stop'`, ce qui serait pire.
- `--no-compiler-runtime`, les DLL du runtime C++ de MSVC, le BOM des `.ps1` : tous documentés, tous justifiés. Les DLL du runtime ne sont pas parties avec Windows 7 et ne doivent pas être confondues avec ce qui l'a fait — elles ne sont dans aucun Windows. Ne pas non plus chercher à les réduire à celles qui sont importées aujourd'hui : c'est cette économie-là, 1,1 Mo, qui a produit une archive incapable de démarrer.
- `opengl32sw.dll` a longtemps figuré ici, au titre du rendu de secours des machines sans pilote OpenGL. Il en est sorti, et de l'archive avec : ce rôle est tenu par ANGLE et WARP, pas par lui. C'est le rappel utile de cette section — une entrée y était depuis des années sur une justification que personne n'avait vérifiée.
