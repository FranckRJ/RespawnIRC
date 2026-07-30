# Pistes de simplification de la compilation et de la distribution

État des lieux des redondances et des frottements de la chaîne de compilation, relevés en juillet 2026 sur `respawnIrc.pro`, `tests/tests.pro`, `zlib.pri`, `dist-windows.ps1`, `run-windows.ps1`, `dist-macos.sh` et la section « Compilation » du README. Ce fichier est une liste de pistes, classées par rapport entre l'effort et le gain, pas un plan arrêté : les points 1 à 5, 7 et 10 n'ont pas été faits. Ceux qui l'ont été portent la mention **fait** et sont conservés pour ce qu'ils ont appris — les points 8, 9 et 11, et le point 6 qui est le plus instructif des quatre.

Les numéros de ligne cités sont ceux du relevé de juillet 2026 et ont dérivé depuis ; les chercher par leur contenu plutôt que par leur numéro.

Les deux premières sont celles qui rapportent le plus pour le moins de risque : elles ne changent aucune sémantique de compilation et suppriment à elles deux quatre analyseurs dupliqués et neuf occurrences d'options.

## 1. Le numéro de version est analysé trois fois, dans trois langages

- `dist-windows.ps1:116` : une expression rationnelle PowerShell sur `respawnIrc.cpp` ;
- `dist-macos.sh:39` : un `sed` sur le même fichier ;
- `respawnIrc/respawnIrc.pro:29-36` : un enchaînement `$$cat` / `$$find` / `$$replace`, accompagné d'un commentaire expliquant que les quotes du shell ne survivent pas à l'analyse de qmake, plus un `error()` de garde.

Trois expressions rationnelles et une dizaine de lignes de contorsions qmake pour lire `"v3.1.17"` dans un littéral C++.

Piste : un `version.pri` contenant `VERSION = 3.1.17`, inclus par le `.pro` qui le pousse ensuite dans un `DEFINES` que `respawnIrc.cpp` consomme. Le bloc qmake se réduit alors à un `include()`, et les deux scripts lisent un fichier d'une ligne au lieu d'analyser du C++. Les tests ne compilent pas `respawnIrc.cpp`, rien d'autre ne bouge.

Contrepartie : le numéro de version disparaît des sources C++, et une compilation sans qmake ne marcherait plus — sans objet ici, qmake est le seul système de compilation du dépôt.

## 2. Trois options qmake sous Windows, dont aucune n'a besoin d'exister

Aujourd'hui : `qmake ...pro HUNSPELL_LIB_NAME=hunspell ZLIB_LIB_NAME=zs DEFINES+=HUNSPELL_STATIC`, répété dans `README.md:87`, encore dans la recette des tests en `README.md:104`, et encore dans `dist-windows.ps1:135`.

- `HUNSPELL_LIB_NAME=hunspell` est **déjà** la valeur par défaut, en `respawnIrc/respawnIrc.pro:54`. La passer ne fait rien. Le paramètre ne sert que pour le cas vcpkg (`hunspell-1.7`) : le mettre vide par défaut et ne le transmettre que s'il est renseigné.
- `ZLIB_LIB_NAME=zs` n'existe que parce que le README fait fabriquer `zs.lib` (`README.md:53`) alors que `zlib.pri:21` prend `zlib` par défaut sous MSVC. La recette manuelle choisit ce nom librement, et vcpkg produit de toute façon `zlib.lib` : c'est donc la recette manuelle qui est l'intruse. Écrire `/OUT:zlib.lib` et la variable disparaît des deux recettes du README, du paramètre par défaut de `dist-windows.ps1:21` et des deux appels à qmake.
- `DEFINES+=HUNSPELL_STATIC` pourrait être une ligne `win32:` dans le `.pro`. Il est sans effet sur le Hunspell de vcpkg, ce qui est justement l'argument pour le figer plutôt que de le répéter à trois endroits. **Contrepartie** : cela contredit le principe documenté selon lequel les `.pro` n'ont pas été modifiés pour Windows et ne devraient pas avoir à l'être. À arbitrer.

Les trois ensemble ramènent la ligne de compilation Windows à `qmake ..\..\respawnIrc\respawnIrc.pro`, de la même forme que sous Linux et macOS.

## 3. Il n'y a pas de script de compilation, seulement un script d'amorçage, un de distribution et un de lancement

Le README fait taper `mkdir` / `cd` / `qmake` / `nmake` à la main (`README.md:85-88`), puis une seconde fois pour les tests (`README.md:102-106`). Pendant ce temps `run-windows.ps1:27` refuse de travailler tant que cela n'a pas été fait, et `dist-windows.ps1:124-143` contient sa propre copie des mêmes quatre étapes.

Piste : un `build-windows.ps1 [-Tests]` extrait de `dist-windows.ps1`, appelé par le script de distribution, appelé par `run-windows.ps1` quand l'exécutable manque, et qui remplace les deux recettes du README.

Cela donnerait aussi un chemin scripté aux tests : aujourd'hui `tests/tests.pro` n'est compilé qu'à la main, et `dist-windows.ps1` fabrique une archive sans avoir lancé les 142 vérifications. Les faire tourner avant l'assemblage ne coûte presque rien puisque la chaîne d'outils est déjà chargée — avec un `-SkipTests` pour s'en passer.

`bootstrap-windows.ps1` a comblé l'autre bout de la chaîne, l'installation des outils, mais s'arrête volontairement là : il se termine en affichant la commande de `dist-windows.ps1` plutôt que de compiler. Un `build-windows.ps1` viendrait exactement entre les deux, et c'est lui qui manque encore pour qu'une machine vierge aille du dépôt à l'exécutable sans qu'on tape une seule ligne de `qmake`.

## 4. Une trentaine de lignes dupliquées mot pour mot entre les deux scripts PowerShell

Le bloc de résolution de Qt est identique : `dist-windows.ps1:30-41` et `run-windows.ps1:32-43`. La vérification de la présence d'OpenSSL est dans les deux également (`dist-windows.ps1:183` et `run-windows.ps1:47`), à la seule différence du `throw` contre le `Write-Warning`.

L'arrivée de `bootstrap-windows.ps1` a aggravé le constat : `Import-MsvcEnvironment` et `Invoke-BuildTool` y sont recopiés à l'identique depuis `dist-windows.ps1`, ce qui en fait deux exemplaires de chacune. La duplication est cette fois délibérée et commentée dans le script — un script d'amorçage tourne avant que quoi que ce soit d'autre n'existe sur la machine, et l'autonomie y a une valeur propre — mais elle reste de la duplication.

Piste : un `windows-common.ps1` chargé par point-sourcing, avec `Resolve-QtDir` et `Test-OpenSslDir`. Ce serait aussi le logement naturel de `Invoke-BuildTool` et `Import-MsvcEnvironment` une fois le script de compilation séparé. À arbitrer pour le script d'amorçage, qui est le seul des trois à avoir une raison de ne dépendre de rien.

## 5. Chaque distribution recompile tout, et efface ce dont `run-windows.ps1` a besoin

`dist-windows.ps1:125` fait `Remove-Item -Recurse -Force` sur `build\respawnIrc`, c'est-à-dire exactement le dossier que lit `run-windows.ps1:25`. Alterner entre essayer le programme et fabriquer une archive coûte donc une recompilation complète des 45 sources à chaque fois.

La compilation est déjà hors des sources, ce qui apporte l'essentiel de ce que cet effacement achetait. Piste : donner à la distribution son propre `build\dist`, ou faire de l'effacement une option `-Clean`.

## 6. Deux détails dans la section des bibliothèques d'exécution — **fait, et ce n'était pas un détail**

Cette section est résolue. Elle est conservée entière parce qu'elle est le meilleur exemple de ce fichier : une simplification classée « détail », dont la réalisation a révélé un bug qui rendait l'archive inutilisable.

Ce qui était écrit ici : « `Microsoft.VC143.CRT` est figé en dur dans `dist-windows.ps1`. Cela cassera au prochain changement de version des outils. Chercher `x64\Microsoft.VC*.CRT` et copier `*.dll` (ce dossier ne contient que les trois DLL voulues plus `concrt140.dll`) supprime le numéro figé, la liste des trois noms et la boucle `foreach`. »

**La parenthèse était fausse, et c'est elle qui cachait le bug.** Le dossier contient **dix** DLL, pas quatre : `concrt140.dll`, `msvcp140.dll`, `msvcp140_1.dll`, `msvcp140_2.dll`, `msvcp140_atomic_wait.dll`, `msvcp140_codecvt_ids.dll`, `vccorlib140.dll`, `vcruntime140.dll`, `vcruntime140_1.dll`, `vcruntime140_threads.dll`, pour 1,8 Mo. Et parmi celles que la liste de trois noms laissait de côté se trouvait `msvcp140_1.dll`, que `Qt5Core.dll` et `Qt5Widgets.dll` importent : **toute archive sortie de ce dépôt échouait au lancement sur une machine sans redistribuable Visual C++**, avec « The code execution cannot proceed because MSVCP140_1.dll was not found ».

Le changement est donc fait, sous sa forme la plus large : `Copy-Item` sur `*.dll` du dossier trouvé par glob. Sur les dix DLL, quatre sont réellement importées — `msvcp140.dll`, `msvcp140_1.dll`, `vcruntime140.dll` et `vcruntime140_1.dll` — et les six que rien n'importe sont copiées aussi, 1,1 Mo sur 158 au total, précisément pour ne pas retenir une liste de noms, qui est ce qui a échoué. S'y ajoute une vérification au `dumpbin` avant compression : aucun binaire de l'archive ne doit réclamer une DLL du runtime qu'elle n'embarque pas. **L'archive corrigée a été essayée sur une machine virtuelle vierge sous Windows 10 LTSC 2019 et le programme fonctionne.**

Le second point tombe de lui-même : `$crtDir` sert maintenant vraiment, puisque c'est de son chemin qu'on copie. Il n'y a plus de `Get-ChildItem` employé comme `Test-Path`.

Trois leçons, qui portent sur tout le reste de ce fichier :

- **une affirmation entre parenthèses est une affirmation.** « Ce dossier ne contient que les trois DLL voulues plus `concrt140.dll` » avait la forme d'une observation et n'était qu'une supposition ; un `dir` l'aurait démentie en une seconde. C'est exactement le défaut que le point 8 se reprochait déjà sous le nom de « ne pas figer de chiffres venant de l'installation » ;
- **classer une piste par son gain la classe mal quand le risque est ailleurs.** Ce point était rangé en « deux détails » à cause du gain — quelques lignes de script. Sa non-réalisation coûtait une archive qui ne démarrait pas ;
- la panne ne se voyait sur aucune machine où l'on pouvait essayer l'archive, parce qu'**installer les Build Tools pose ces DLL dans `System32`**. La machine qui fabrique l'archive ne peut donc jamais la valider. C'est le même mécanisme que celui décrit au point 8 pour l'Universal CRT — « la seule pièce dont l'absence ne se voyait sur aucune machine où l'on pouvait essayer l'archive » — et il a frappé deux fois.

## 7. macOS compile dans les sources, Windows non

`dist-macos.sh:51` a besoin d'un `rm -rf RespawnIRC.app` uniquement parce que la règle qmake qui fabrique `Info.plist` n'a aucune dépendance et que `make` la saute dès qu'un bundle est déjà là. Compiler dans `build/` comme sous Windows rend ce contournement inutile : un dossier neuf ne peut pas traîner un `Info.plist` périmé.

## 8. L'installation des outils Visual Studio, et ce qu'elle entraîne pour l'archive — **fait**

Cette section est résolue, elle est conservée parce que le soupçon était juste mais la cause fausse.

L'installation ciblée à deux composants a été essayée sur une machine virtuelle vierge et fonctionne : 3,3 Go contre environ 5 Go, sans WebView2 ni Edge, jusqu'à l'archive. Elle est devenue la commande principale du README. Au passage, les deux composants sont bien nécessaires : `VC.Tools.x86.x64` seul ne pose aucun `Windows Kits` et rien ne compile.

Le script échouait effectivement en silence, mais pas pour la raison supposée ici. La question n'était pas de savoir si l'installation ciblée pose l'arborescence `Redist\ucrt` : c'est la **version du SDK** qui décide, les récents versionnant ce dossier en `Redist\<version>\ucrt\DLLs\x64`. Avec le SDK 10.0.26100, le chemin sans version n'existe pas du tout, et `--includeRecommended` avec le même SDK donnerait sans doute le même résultat. Le raisonnement « voie allégée = risque » était donc mal orienté, même si sa conclusion était la bonne.

`dist-windows.ps1` a cherché les deux dispositions et levé une erreur s'il ne trouvait rien, comme il le fait déjà pour OpenSSL et les bibliothèques de MSVC. **Tout ce bloc a depuis été supprimé** avec l'abandon de Windows 7 : l'Universal CRT n'est plus copié du tout, et la question de savoir où le SDK range son redistribuable ne se pose plus. Le récapitulatif de fin de script, lui, reste une piste jamais faite.

Deux leçons transposables au reste de ce fichier survivent à cette suppression :

- **ne pas figer de chiffres** venant de l'installation. Le nombre de DLL de l'UCRT dépendait de la version du SDK, 41 relevées ici lors d'une compilation antérieure et 46 avec le SDK 10.0.26100 ; c'est exactement le défaut que le point 6 reprochait à `Microsoft.VC143.CRT`, et il a fini par coûter une archive qui ne démarrait pas — cette leçon-là a été écrite avant d'être appliquée ;
- **la meilleure façon de ne pas se tromper sur un chemin d'installation est de ne pas en dépendre.** Cette section a coûté deux corrections successives avant que la bonne réponse se révèle être la suppression pure et simple du code fautif.

## 9. Deux listes de « trois choses » qui ne se recouvrent pas — **fait**

`dist-windows.ps1` annonçait trois choses que Windows 10 fournit et que Windows 7 n'a pas — OpenSSL, les bibliothèques C++ de MSVC, l'Universal CRT — quand le README en annonçait trois autres — l'Universal CRT, les bibliothèques de MSVC et `D3Dcompiler_47.dll` — en traitant OpenSSL à part. Les deux listes ne se recouvraient que sur deux éléments, et `D3Dcompiler_47.dll` n'apparaissait nulle part dans le script : il arrivait par `windeployqt`, dans le lot ANGLE.

L'abandon de Windows 7 a résolu le désaccord en vidant les deux listes. Il ne reste qu'une seule pièce à copier, les bibliothèques C++ de MSVC, plus OpenSSL pour une raison sans rapport avec la version de Windows. `D3Dcompiler_47.dll` est maintenant nommé dans le script, mais pour être **retiré** de ce que `windeployqt` a copié.

## 10. Accessoire : la compression

`Compress-Archive` sur 159 Mo et quelques centaines de fichiers est l'étape la plus lente du script (`dist-windows.ps1:312`). `[System.IO.Compression.ZipFile]::CreateFromDirectory` fait la même chose nettement plus vite. Sans autre effet que le temps d'attente.

## 11. `opengl32sw.dll` : 20 Mo pour un repli que Windows fournit déjà — **fait**

C'était le plus gros fichier retirable de l'archive, et le seul point de cette liste dont le gain se comptait en dizaines de mégaoctets — 20 sur 178, soit 11 %. `dist-windows.ps1` le retire désormais après `windeployqt`, comme `D3Dcompiler_47.dll`.

La justification inscrite partout dans ce dépôt était fausse : « sans pilote OpenGL utilisable, le système ne donne que du 1.1, donc `opengl32sw.dll` est le seul recours ». La prémisse est juste, la conclusion non. Qt ne reste pas sur l'OpenGL de bureau : son défaut bascule sur ANGLE, qui traduit en Direct3D 11, et Direct3D sans GPU utilisable se rabat sur WARP, le rasteriseur logiciel livré avec Windows. Le repli logiciel est déjà dans le système, une couche plus bas.

Mesuré sur une machine virtuelle sans aucune accélération graphique, `GL_RENDERER` vaut `ANGLE (Microsoft Basic Render Driver Direct3D11 vs_5_0 ps_5_0)` en mode par défaut, et l'archive privée d'`opengl32sw.dll` affiche correctement une page dans QtWebEngine — c'est-à-dire précisément le cas que ce fichier était censé sauver.

Ce qu'il couvre encore : le cas où ANGLE lui-même échouerait, et le cas où `QT_OPENGL=software` serait forcé. Dans ce dernier, sans lui, Qt affiche « Failed to create OpenGL context » et le programme s'arrête — vérifié.

### Ce que disent les versions publiées en amont

La mesure ci-dessus est confirmée par l'historique des releases de `franckrj/respawnirc`, et c'est l'argument le plus solide de cette section parce qu'il porte sur de vrais utilisateurs :

| Release | Date | QtWebEngine | `opengl32sw.dll` |
| --- | --- | --- | --- |
| v3.1.6 à v3.1.10 | 2018 – mars 2019 | oui (49 Mo) | **non** |
| v3.1.11 et suivantes | juillet 2019 → | oui (57 Mo puis 72) | oui |

**Cinq versions ont été distribuées avec QtWebEngine et ANGLE mais sans aucun rendu OpenGL logiciel**, pendant environ un an et demi, sans que cela pose de problème signalé. C'est exactement la configuration que le point ci-dessus propose de rétablir.

Son apparition en v3.1.11 n'a rien d'une correction : elle est simultanée à un changement de version de Qt (`Qt5WebEngineCore.dll` passe de 49 à 57 Mo) et à celle de `vc_redist.x86.exe`, dans la même archive. Ce sont les deux fichiers que `windeployqt` ajoute tout seul — le second étant précisément celui que `--no-compiler-runtime` sert à écarter ici. `opengl32sw.dll` est donc arrivé comme effet de bord de l'outil de déploiement, jamais comme une réponse à une panne.

**Contrepartie assumée** : WARP est un composant de Windows, mais rien ne prouve qu'aucune machine cible n'a un Direct3D 11 cassé ou désactivé, et la panne serait alors totale et sans message utile. L'absence de plainte sur les versions 3.1.6 à 3.1.10 n'est pas une preuve non plus — qui obtient une fenêtre noire ne le signale pas forcément. Le retrait a été fait parce que c'est celui de cette liste qui s'appuie sur le plus de faits, pas parce que le risque est nul : le remettre est une ligne à supprimer dans `dist-windows.ps1`.

## Ce qu'il ne faut pas toucher

- `Invoke-BuildTool` (`dist-windows.ps1:90`) ressemble à de la cérémonie mais c'est la plus petite correction juste au fait que PowerShell 5.1 transforme la sortie d'erreur des outils natifs en erreurs fatales. S'en passer voudrait dire abandonner `$ErrorActionPreference = 'Stop'`, ce qui serait pire.
- `--no-compiler-runtime`, les DLL du runtime C++ de MSVC, le BOM de `dist-windows.ps1` : tous documentés, tous justifiés. Les DLL du runtime en particulier ne sont pas parties avec Windows 7 et ne doivent pas être confondues avec ce qui l'a fait — elles ne sont dans aucun Windows. Ne pas non plus chercher à les réduire à celles qui sont importées aujourd'hui : c'est cette économie-là, 1,1 Mo, qui a produit une archive incapable de démarrer (point 6).
- `opengl32sw.dll` figurait ici, au titre du rendu de secours des machines sans pilote OpenGL. Il en sort, et de l'archive avec : ce rôle-là est tenu par ANGLE et WARP, pas par lui (point 11). C'est le rappel utile de cette section — une entrée y était depuis des années sur une justification que personne n'avait vérifiée.
