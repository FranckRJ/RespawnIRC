# Pistes de simplification de la compilation et de la distribution

État des lieux des redondances et des frottements de la chaîne de compilation, relevés en juillet 2026 sur `respawnIrc.pro`, `tests/tests.pro`, `zlib.pri`, `dist-windows.ps1`, `run-windows.ps1`, `dist-macos.sh` et la section « Compilation » du README. **Rien n'a été modifié** : ce fichier est une liste de pistes, classées par rapport entre l'effort et le gain, pas un plan arrêté.

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

## 3. Il n'y a pas de script de compilation, seulement un script de distribution et un script de lancement

Le README fait taper `mkdir` / `cd` / `qmake` / `nmake` à la main (`README.md:85-88`), puis une seconde fois pour les tests (`README.md:102-106`). Pendant ce temps `run-windows.ps1:27` refuse de travailler tant que cela n'a pas été fait, et `dist-windows.ps1:124-143` contient sa propre copie des mêmes quatre étapes.

Piste : un `build-windows.ps1 [-Tests]` extrait de `dist-windows.ps1`, appelé par le script de distribution, appelé par `run-windows.ps1` quand l'exécutable manque, et qui remplace les deux recettes du README.

Cela donnerait aussi un chemin scripté aux tests : aujourd'hui `tests/tests.pro` n'est compilé qu'à la main, et `dist-windows.ps1` fabrique une archive sans avoir lancé les 142 vérifications. Les faire tourner avant l'assemblage ne coûte presque rien puisque la chaîne d'outils est déjà chargée — avec un `-SkipTests` pour s'en passer.

## 4. Une trentaine de lignes dupliquées mot pour mot entre les deux scripts PowerShell

Le bloc de résolution de Qt est identique : `dist-windows.ps1:30-41` et `run-windows.ps1:32-43`. La vérification de la présence d'OpenSSL est dans les deux également (`dist-windows.ps1:183` et `run-windows.ps1:47`), à la seule différence du `throw` contre le `Write-Warning`.

Piste : un `windows-common.ps1` chargé par point-sourcing, avec `Resolve-QtDir` et `Test-OpenSslDir`. Ce serait aussi le logement naturel de `Invoke-BuildTool` et `Import-MsvcEnvironment` une fois le script de compilation séparé.

## 5. Chaque distribution recompile tout, et efface ce dont `run-windows.ps1` a besoin

`dist-windows.ps1:125` fait `Remove-Item -Recurse -Force` sur `build\respawnIrc`, c'est-à-dire exactement le dossier que lit `run-windows.ps1:25`. Alterner entre essayer le programme et fabriquer une archive coûte donc une recompilation complète des 45 sources à chaque fois.

La compilation est déjà hors des sources, ce qui apporte l'essentiel de ce que cet effacement achetait. Piste : donner à la distribution son propre `build\dist`, ou faire de l'effacement une option `-Clean`.

## 6. Deux détails dans la section des bibliothèques d'exécution

- `Microsoft.VC143.CRT` est figé en dur dans `dist-windows.ps1:192` et `:201`. Cela cassera au prochain changement de version des outils. Chercher `x64\Microsoft.VC*.CRT` et copier `*.dll` (ce dossier ne contient que les trois DLL voulues plus `concrt140.dll`) supprime le numéro figé, la liste des trois noms et la boucle `foreach`.
- `dist-windows.ps1:192` affecte un `Get-ChildItem` à `$crtDir` pour ne s'en servir que comme test d'existence. `Test-Path` dit la même chose sans construire un listage de dossier.

## 7. macOS compile dans les sources, Windows non

`dist-macos.sh:51` a besoin d'un `rm -rf RespawnIRC.app` uniquement parce que la règle qmake qui fabrique `Info.plist` n'a aucune dépendance et que `make` la saute dès qu'un bundle est déjà là. Compiler dans `build/` comme sous Windows rend ce contournement inutile : un dossier neuf ne peut pas traîner un `Info.plist` périmé.

## 8. L'installation des outils Visual Studio, et ce qu'elle entraîne pour l'archive — **fait**

Cette section est résolue, elle est conservée parce que le soupçon était juste mais la cause fausse.

L'installation ciblée à deux composants a été essayée sur une machine virtuelle vierge et fonctionne : 3,3 Go contre environ 5 Go, sans WebView2 ni Edge, jusqu'à l'archive. Elle est devenue la commande principale du README. Au passage, les deux composants sont bien nécessaires : `VC.Tools.x86.x64` seul ne pose aucun `Windows Kits` et rien ne compile.

Le script échouait effectivement en silence, mais pas pour la raison supposée ici. La question n'était pas de savoir si l'installation ciblée pose l'arborescence `Redist\ucrt` : c'est la **version du SDK** qui décide, les récents versionnant ce dossier en `Redist\<version>\ucrt\DLLs\x64`. Avec le SDK 10.0.26100, le chemin sans version n'existe pas du tout, et `--includeRecommended` avec le même SDK donnerait sans doute le même résultat. Le raisonnement « voie allégée = risque » était donc mal orienté, même si sa conclusion était la bonne.

`dist-windows.ps1` cherche maintenant les deux dispositions et lève une erreur s'il ne trouve rien, comme il le fait déjà pour OpenSSL et les bibliothèques de MSVC. Le récapitulatif de fin de script n'a pas été fait et reste une piste, mais l'échec franc en couvre l'essentiel.

Leçon transposable au reste de ce fichier : le nombre de DLL de l'UCRT dépend lui aussi de la version du SDK, 41 relevées ici lors d'une compilation antérieure et 46 avec le SDK 10.0.26100. **Ne pas figer ces chiffres**, ni dans le script ni dans la documentation.

## 9. Deux listes de « trois choses » qui ne se recouvrent pas

`dist-windows.ps1:13-15` annonce trois choses que Windows 10 fournit et que Windows 7 n'a pas, et ses sections livrent OpenSSL, les bibliothèques C++ de MSVC et l'Universal CRT. `README.md:120-124` annonce trois choses aussi, mais énumère l'Universal CRT, les bibliothèques de MSVC et `D3Dcompiler_47.dll`, OpenSSL étant traité à part dans sa propre sous-section.

`D3Dcompiler_47.dll` n'apparaît nulle part dans le script : il arrive par `windeployqt`, dans le lot ANGLE. Rien n'est cassé, le fichier finit bien dans l'archive, mais les deux listes ne se recouvrent que sur deux éléments et l'une des trois du README n'est pas le fait du script. À aligner si l'on touche à cette section, pour éviter au prochain lecteur d'aller chercher une copie qui n'existe pas.

## 10. Accessoire : la compression

`Compress-Archive` sur 184 Mo et plusieurs milliers de fichiers est l'étape la plus lente du script (`dist-windows.ps1:227`). `[System.IO.Compression.ZipFile]::CreateFromDirectory` fait la même chose nettement plus vite. Sans autre effet que le temps d'attente.

## Ce qu'il ne faut pas toucher

- `Invoke-BuildTool` (`dist-windows.ps1:90`) ressemble à de la cérémonie mais c'est la plus petite correction juste au fait que PowerShell 5.1 transforme la sortie d'erreur des outils natifs en erreurs fatales. S'en passer voudrait dire abandonner `$ErrorActionPreference = 'Stop'`, ce qui serait pire.
- `--no-compiler-runtime`, la pile de DLL de l'Universal CRT, `opengl32sw.dll`, le BOM de `dist-windows.ps1` : tous documentés, tous justifiés.
