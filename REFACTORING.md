# RespawnIRC — pistes de refactorisation

Analyse refaite le 31 juillet 2026 sur la branche `macos2`. Portée : `respawnIrc/`, **12 273
lignes réparties sur 84 fichiers** (42 `.cpp` pour 10 321 lignes, 42 `.hpp` pour 1 952). La
première version de ce fichier annonçait « 10 345 lignes sur 83 fichiers » : ce nombre était celui
des seuls `.cpp`, et le compte de fichiers était faux. **Rien de ce qui suit n'a été appliqué** —
vérifié à l'historique, aucun commit n'a touché `respawnIrc/` depuis le 29 juillet en dehors des
chemins et de la version.

Les gains en lignes sont des estimations faites à la lecture. Ce qui a été **mesuré** est signalé
comme tel ; le reste ne l'est pas.

Classé par rapport gain / risque.

---

## A. Duplication supprimable mécaniquement

### 1. La table des stickers — `selectStickerWindow.cpp:142-441` (300 lignes)

**C'est le plus gros bloc de donnée déguisée en code du dépôt**, et il ne figurait pas dans la
version précédente de ce fichier. `loadAndUseListOfStickers` est un `switch` de quinze `case`, un
par famille de stickers, chacun n'étant qu'une suite d'appels identiques :

```cpp
case 6:
{
    //chat
    generateAndInsertStickerCode("1kky.png", listOfStickerTypeContent[stickerType]);
    generateAndInsertStickerCode("1kkz.png", listOfStickerTypeContent[stickerType]);
    ... quatorze fois ...
}
```

Environ **230 lignes** ne portent qu'un nom de fichier, `listOfStickerTypeContent[stickerType]`
étant répété à chaque ligne. Et ces quinze `case` sont appariés **à la main, par leur numéro**, aux
seize `createQLabelForStickerTypeWithThesesInfos("stickers/1kl6.png", ...)` du constructeur
(lignes 39-54) : rien ne relie l'icône à sa famille sauf l'ordre et un commentaire (`//chat`).
Ajouter une famille demande d'éditer deux endroits et de ne pas se tromper de numéro — exactement
le défaut reproché au point 6 ci-dessous.

Une table `static const` de `{nomDeLIcône, {liste des fichiers}}` remplace le constructeur **et** le
`switch` par deux boucles. Le `case 0` est le seul à part : il liste les dossiers au lieu de porter
une liste, ce qu'une liste vide exprime très bien.

**~250 lignes**, et `selectStickerWindow.cpp` passe de 487 lignes à environ 240.

### 2. La table des smileys — `respawnIrc.cpp:309-380` (72 lignes)

72 `dataInHtml.replace("<img src=\"resources/smileys/N.gif\" />", ":code:")` à la suite, déjà
signalées `//à changer, si possible` par l'auteur d'origine.

C'est de la donnée, et **la machinerie existe et convient** — vérifié en relisant
`shortcutTool::loadShortcutRule` : elle lit un fichier `resources/<règle>.txt` au format
`base<espace>remplacement` et sait envelopper la base et le remplacement
(`beforeBase`/`afterBase`/`beforeNew`/`afterNew`). Un `resources/smileyToCode.txt` chargé avec
`beforeBase = "<img src=\"resources/smileys/"`, `afterBase = ".gif\" />"` et `useRegex = false`
fait exactement le travail. `resources/stickerToSmiley.txt` (61 lignes) est déjà servi ainsi.

Seule contrainte : la règle doit être chargée par `initializeAllShortcutsRules()`, appelé au
démarrage, alors que la substitution a lieu dans `clipboardChanged`. C'est déjà le cas.

**~70 lignes.**

### 3. Chargement des pixmaps — `showForum.cpp:31-65`

```cpp
if(pinnedOnTagImage.isNull() == true)
{
    pinnedOnTagImage.reset(new QPixmap);
    pinnedOnTagImage->load(...);
}
```

Sept fois. Une `QMap<QString, QPixmap>` remplie depuis une table statique
`{topicType, nomDeFichier, balise}` et une boucle.

**~28 lignes.**

### 4. `createItemDependingOnTopic` — `showForum.cpp:171-303`

132 lignes de `if(topicType == X) { if(showXTag) { if(useIcon) setIcon(...) else prefixe = "[X] " } }`,
six fois, seules les chaînes changent. Avec la table du point 3
(`{type → pixmap, balise, "showXTagOnTopicInTopicList"}`), toute la cascade devient une recherche.

**~70 lignes.**

### 5. Les neuf décorations de texte, écrites **trois fois**

La version précédente ne voyait que la première de ces trois listes. La même énumération —
gras, italique, souligné, barré, liste, liste ordonnée, citation, code, spoil — est recopiée dans
trois fichiers :

- `respawnIrc.cpp:465-524` : neuf `QPushButton`, chacun avec trois lignes de réglages identiques,
  puis neuf `addWidget`, puis neuf `connect` ;
- `mainWindow.cpp:55-63` et `176-184` : neuf `QAction` et leurs neuf `connect` ;
- `multiTypeTextBox.cpp:149-213` : neuf méthodes `addBold()`… qui se ramènent à **deux** formes,
  « entourer la sélection et reculer de N » (six fois) et « préfixer la ligne » (trois fois).

Une seule table `{libellé, objectName, ouverture, fermeture}` alimente les trois, et les neuf
méthodes se réduisent à `wrapSelection(avant, après)` et `prefixLine(préfixe)`.

**~35 lignes** pour les seuls boutons, **~90** pour les trois endroits.

### 6. `setTheseOptions` — `respawnIrc.cpp:561-689`

~130 lignes de `if((boolIte = newBoolOptions.find("nom")) != end()) { ... }`, dont les corps se
répartissent en quatre combinaisons de *(mettre à jour sendMessages / mettre à jour tabView /
recharger / effacer)*. Deux tables statiques (une bool, une int) de `{nomOption, drapeaux}` et une
boucle sur ce qui a réellement changé.

**⚠ La version précédente présentait l'absence d'`erase()` comme un oubli — c'est faux, et refaire
ce point sur cette base casserait le rechargement des topics.** Le `erase()` n'est pas un
nettoyage : c'est ce qui **consomme** l'option. Le bloc final ne s'exécute que si la map n'est pas
vide :

```cpp
if(newBoolOptions.isEmpty() == false || newIntOptions.isEmpty() == false)
{
    tabViewTopicInfos->updateSettingInfoForList();
    if(reloadForAllTopicNeeded == true) tabViewTopicInfos->reloadAllTopic();
}
```

Les branches qui **n'effacent pas** sont exactement celles qui posent `reloadForAllTopicNeeded` :
laisser l'entrée en place est le seul moyen de faire entrer dans ce bloc. Les faire toutes effacer
rendrait `reloadAllTopic()` inatteignable quand une de ces options change seule, **sans qu'aucun
test ne le voie**.

Il y en a de plus **cinq et non trois** — la version précédente en manquait deux :
`smartAvatarResizing`/`smartNoelshackResizing` (621), `downloadHighDefAvatar` (630), `avatarSize`
(658), `noelshackImageWidth` (667) et `typeOfEdit` (676). La table doit donc porter un drapeau
« recharger » explicite plutôt que de reproduire l'effacement, et c'est justement ce qui rend le
point intéressant : le mécanisme actuel est correct mais illisible.

**~85 lignes.**

### 7. `getModelInfo` — `styleTool.cpp:49-141`

Deux listes parallèles numérotées à la main : 25 `push_back` de valeurs par défaut, puis 25
affectations `listOfLine.at(N)` dont l'indice doit correspondre à l'œil. Ajouter un champ de thème
demande aujourd'hui d'éditer deux endroits et de renuméroter.

**La solution proposée auparavant — « un seul tableau `static const` de `{valeurParDéfaut,
pointeurSurMembre}` » — ne peut pas marcher telle quelle**, pour deux raisons relevées en relisant
la fonction :

- les 25 valeurs vont dans **deux destinations différentes** : les indices 0-6, 11-13, 16, 18-20 et
  23 dans le `modelInfoStruct` retourné, les indices 7-10, 14-15, 17, 21-22 et 24 dans le
  `colorInfoForMessageAndOther` de portée fichier. Un seul type de pointeur sur membre ne les couvre
  pas ; il faut deux tables, ou une table de petites fonctions ;
- **`getModelInfo` n'est pas un accesseur : c'est lui qui publie les couleurs globales.**
  `mainWindow.cpp:334` l'appelle en **jetant sa valeur de retour**, uniquement pour cet effet de
  bord. Le supprimer en croyant nettoyer ferait cesser la mise à jour des couleurs au changement de
  thème.

Le point reste bon, la recette non. Séparer explicitement les deux tables — et pendant qu'on y est,
renommer en quelque chose qui ne prétende pas être un `get` — est la vraie forme de ce nettoyage.

**~45 lignes.**

### 8. Les quatre accesseurs de `settingTool.cpp:135-199`

`getThisBoolOption`, `getThisStringOption` et `getThisByteOption` sont la même fonction trois fois
(recherche, repli sur le défaut, `qDebug` en cas d'absence). Un patron sur le type de valeur.

Nuance que la version précédente écrasait : **`getThisIntOption` n'est pas de la même forme.** Il
recopie le `intSettingStruct` trouvé et n'en remplace que le `.value`, parce qu'il doit rendre aussi
`minValue` et `maxValue`. Le patron demandera une spécialisation, ou de le laisser de côté.

Même remarque pour `getListOfPseudoForTopic` / `getListOfIgnoredPseudo` / `getListOfTopicLink`
(221-283), presque identiques au nom de clé près — `getListOfTopicLink` porte en plus une boucle de
réécriture `http://` → `https://` qu'il faudra garder.

Et `initializeDefaultListsOption` (21-133) écrit trois lignes par option entière
(`.value`, `.minValue`, `.maxValue`) ; une initialisation agrégée en rend une seule, soit ~34 lignes
sur les 113.

**~55 lignes**, ~90 avec les valeurs par défaut.

### 9. Les trois fenêtres de liste

`ignoreListWindow.cpp` (95), `colorPseudoListWindow.cpp` (144) et `accountListWindow.cpp` (121)
partagent le même squelette : `QListView` + `QStringListModel`, `NoEditTriggers`, une grille de
boutons Ajouter/Éditer/Supprimer, un `updateList()` qui émet `listHasChanged`, et la garde
`if(currentIndex().row() != -1)` répétée dans chaque slot.

Une `listWindowBaseClass` portant la vue, le modèle et la garde ne laisserait à chaque classe fille
que ses propres boutons. Cela supprime au passage l'expression
`listOfColorPseudo->at(view->currentIndex().row())` écrite **six fois** dans une seule fonction
(`colorPseudoListWindow.cpp:136-141`, et non trois comme écrit auparavant : trois `at()` puis trois
`operator[]`).

`manageShortcutWindow.cpp` partage la même garde mais sur un `QTreeView` + `QStandardItemModel` :
assez différent pour rester dehors.

**~80 lignes.**

### 10. La cascade des champs cachés — `parsingTool.cpp:317-354`

38 lignes de `if(captured(1) == "type")` / `if(captured(3) == "name")` imbriqués pour gérer trois
ordres d'attributs possibles. Comme `expForInput` capture trois paires `nom="valeur"` fixes,
parcourir `{captured(1),captured(2)}, {captured(3),captured(4)}, {captured(5),captured(6)}` et y
repérer `name`/`value` traite tous les ordres en ~10 lignes — y compris ceux que le code actuel ne
gère pas.

**~28 lignes.**

### 11. Petits motifs répétés

- **`listOfContainerForTopicsInfos.back()` dix-neuf fois dans une seule fonction**
  (`tabViewTopicInfos.cpp:163-219`), dont quinze `connect` consécutifs. Une variable locale suffit,
  et rend le bloc lisible. Toujours dans ce fichier, la boucle « trouver le conteneur dont le
  `showTopic` est l'émetteur » est écrite trois fois (356-366, 369-380, 382-398), et le triplet
  `resetSearchPath()` + deux `addSearchPath()` trois fois aussi.
- **Deux `QVector` parallèles indexés ensemble** dans `getTopicMessages.hpp:57-58`
  (`listOfTimeoutForReplys` et `listOfReplys`) : toujours redimensionnés ensemble, toujours
  parcourus ensemble, et six boucles de nettoyage presque identiques les traversent
  (`getTopicMessages.cpp:51-60, 98-104, 141-149, 397-404, 406-413, 469-478`). Un seul vecteur de
  `{reply, timeout}` supprime la moitié de ces boucles.
- **Trois `QVector<QAction*>` parallèles** dans `mainWindow.cpp` (`vectorOfUseFavorite`,
  `vectorOfAddFavorite`, `vectorOfDelFavorite`), toujours indexés par le même entier, avec le
  triplet « police en italique + `setText` » recopié dans `addFavoriteClicked` et
  `delFavoriteClicked` (286-329).
- **`keyPressEvent` — `mainWindow.cpp:205-255`** : dix `else if(keyPressed == "&")` →
  `selectThisTab(0)`, un par touche de la rangée des chiffres en AZERTY. Une `QStringList` statique
  et un `indexOf` ramènent 45 lignes à cinq. (Au passage : la table est en dur en AZERTY, donc ces
  raccourcis ne font rien sur un autre agencement — un tableau rend au moins le fait visible.)
- **`addOrUpdateRule` — dix paramètres positionnels** (`imageDownloadTool.hpp:37-39`), d'où des
  appels comme
  `addOrUpdateRule("noelshack", "/img/", true, false, "", "", false, W, H, false)`. Le
  `imageDownloadRuleStruct` existe déjà : le passer directement supprime la traduction ligne à ligne
  du corps de la fonction et rend les booléens nommés. **Attention en le faisant** : l'ordre des
  champs de la structure n'est pas celui des paramètres.
- **Deux `QMap` parallèles** dans `imageDownloadTool` (`listOfRulesForImage` et
  `listOfExistingImagesForRules`), toutes deux clés par `ruleName` et toujours cherchées ensemble
  (lignes 102-106, 158, 265-266). Ranger la `QStringList` dans la structure de règle supprime une
  map et la moitié des recherches. Dans `analyzeLatestImageDownloaded`, `ruleIte.value()` est écrit
  douze fois et `listOfImagesUrlNeedDownload.front()` quatre fois : deux références locales.
- Le motif `if(reply->isOpen()) connect(...) else { analyze(); manager->deleteLater(); manager =
  nullptr; }` apparaît **six fois** à l'identique — `showForum.cpp:345`, `sendMessages.cpp:135`,
  `imageDownloadTool.cpp:229`, `messageActions.cpp:77` et `148`, `checkUpdate.cpp:28` — plus une
  variante dans `getTopicMessages.cpp:437-460`. (La version précédente en comptait quatre.)
- `networkManager->clearAccessCache(); setCookieJar(new QNetworkCookieJar(this));
  cookieJar()->setCookiesFromUrl(...)` est répété tel quel dans **quatre** endroits :
  `showForum.cpp:133`, `sendMessages.cpp:89`, `getTopicMessages.cpp:76`, `messageActions.cpp:42`.
  Un `utilityTool::applyCookie(manager, cookie, website)`. (La version précédente en annonçait cinq
  et citait `showTopic`, qui ne fait pas cet appel.)
- La boucle de rognage des `<br />` en tête et en queue apparaît trois fois
  (`parsingTool.cpp:266-276`, `1097-1107`, et `stringModificatorRemoveFirstsAndLastsPAndBr` en 83),
  et `stringModificatorMakeCodeBlockGreatAgain` (151) en est une quatrième variante sur `\n`. Une
  fonction paramétrée par le marqueur.
- `preferencesWindow.cpp` : seize fois le motif `QGroupBox* g = new QGroupBox(titre, this);
  QVBoxLayout* v = new QVBoxLayout(); … v->addStretch(1); g->setLayout(v);`, et quatre fois la même
  queue de six lignes en fin de `createWidgetFor*Tab`. Le fichier est par ailleurs déjà bien
  factorisé (`makeNewCheckBox`/`makeNewSpinBox`/`makeNewComboBox`) — c'est ce qui reste.
  `valueOfFastModeCheckBoxChanged` (411-451) est deux blocs symétriques activer/désactiver qu'un
  paramètre booléen fusionne.

**Total réaliste pour la section A : 900 à 1 000 lignes**, environ 8 % du code, sans changement de
comportement — le point 1 à lui seul en fait le quart.

---

## B. Le correcteur orthographique est chargé deux fois

Absent de la version précédente, et c'est probablement la meilleure piste **structurelle** du dépôt.

`spellTextEditClass::setDic` (`spellTextEdit.cpp:72-103`) et `highlighterClass::setDic`
(`highlighter.cpp:38-76`) sont la même fonction d'une trentaine de lignes, recopiée : même test
d'existence du `.dic`, même construction de `Hunspell` sur les mêmes fichiers, même `add_dic` du
dictionnaire utilisateur, même choix de codec par `get_dic_encoding()`. Les deux classes portent
chacune leur `spellChecker`, leur `codec`, leur `delete` écrit à la main dans le destructeur, et un
`checkWord()` identique au mot près.

Or `multiTypeTextBox.cpp:125-126` appelle les deux : `textEdit->setDic("fr")` **et**
`highlighter->setDic("fr")`. **Deux instances de Hunspell sont donc chargées sur le même
`resources/fr.dic`**, pour le même champ de saisie.

Un petit `spellCheckerClass` partagé — ou simplement l'instance du `spellTextEdit` passée au
`highlighter` — supprime une soixantaine de lignes dupliquées, un chargement de dictionnaire, et
deux `delete` manuels au profit d'un `std::unique_ptr`.

---

## C. Chaînes de caractères qui devraient être des énumérations

`pseudoInfoStruct::pseudoType` est un `QString` contenant `"user"`/`"modo"`/`"admin"`/`"staff"` ;
`topicStruct::topicType` contient `"topic-pin-on"`/`"topic-lock"`/…

Ces valeurs pilotent des chaînes de comparaisons dans `showTopic.cpp:642-674`,
`showForum.cpp:189-300` et sont fabriquées par `parsingTool.cpp:816-863`. En `enum class`, elles
deviennent des indices de tableau ou des `switch` dont le compilateur vérifie l'exhaustivité — et
une faute de frappe dans `"topic-resolved"` cesserait d'être un repli silencieux vers
`topic-folder`.

C'est le changement qui rend les points A4 et A7 naturels plutôt qu'astucieux.

Une contrainte à ne pas oublier : `stateOfTopicToTopicType` et `roleToPseudoType` sont **couvertes
par les tests** (`testParsing.cpp`), qui comparent leur sortie à des chaînes littérales. Passer à
une énumération demande de toucher aussi ces vérifications — ce qui est une bonne chose, mais n'est
pas gratuit.

---

## D. Code mort

**Un seul des trois points de la version précédente tient.** Les deux autres étaient faux, et agir
dessus aurait cassé quelque chose.

- `parsingTool::generateNewUserAgent()` — déclaré (`parsingTool.hpp:97`), **jamais défini, jamais
  appelé**. Une ligne à supprimer. C'est le seul vrai code mort trouvé.
- ~~`parsingTool::getErrorMessageInJSON()` est appelé de nulle part~~ — **faux.** Il n'est plus
  appelé par le programme, mais **les tests l'exercent** (`tests/testParsing.cpp:371, 374, 377`, trois
  vérifications). Le supprimer casse la compilation des tests. Le choix réel est donc : le garder
  comme fonction publique testée, ou le supprimer **avec** ses trois vérifications en assumant de
  perdre la couverture de ce format d'erreur.
- ~~`showTopicClass::replaceTextOrRemoveIt` est un enrobage de deux lignes~~ — exact, mais il est
  **appelé huit fois** (`showTopic.cpp:702-738`). L'enlever remplacerait huit appels d'une ligne par
  huit `if/else` de cinq. C'est une perte, pas un gain.

---

## E. C++ plus récent

Le projet est en `CONFIG += c++14` (`respawnIrc.pro:128` et `tests.pro:2`) avec Qt 5.15.2.

**« Passer à C++17 est sans danger » : maintenant mesuré, et pas tout à fait sans effet.** Compilé
ici le 31 juillet 2026, macOS, clang d'Xcode, Qt 5.15.2 officiel, hors des sources :

- le programme compile **sans erreur** en `-std=c++1z`, 74 unités de compilation ;
- les tests compilent et **142 vérifications passent, 0 échec** — le même compte qu'en C++14 ;
- mais C++17 **fait apparaître 9 avertissements que C++14 masquait** (0 en C++14, 9 en C++17, à
  lignes de commande par ailleurs identiques). Tous portent sur des **valeurs d'énumération** Qt
  dépréciées : `QRegularExpression::OptimizeOnFirstUsageOption` (`configDependentVar.hpp:12`, vu
  depuis chaque unité qui l'inclut) et `QPalette::Background` (`selectStickerWindow.cpp:130, 468,
  471`). La cause est dans Qt : `Q_DECL_ENUMERATOR_DEPRECATED_X` ne devient un vrai
  `[[deprecated]]` que si `__cpp_enumerator_attributes` est défini, ce que C++14 ne fait pas.

Ce n'est donc pas un coût mais un gain caché : ces deux dépréciations sont exactement du genre que
Qt 6 supprime, et C++14 empêchait de les voir. À rapprocher de `MIGRATION-QT6.md`.

C++20 reste à éviter ici : `strict_c++` plus MSVC plus les en-têtes de Qt 5.15, pour peu de gain.

Le point important est ailleurs : **C++11 n'est pas encore utilisé**, et encore moins C++14.

- **`std::bind` — `parsingTool.cpp:1004-1077`.** Sept appels à `std::bind(f, std::placeholders::_1)`
  où `f` est déjà une fonction `QString(QString)`. Le `bind` ne fait rien : passer `f` directement.
  (Le huitième `std::bind` du dépôt, `webNavigator.cpp:104`, lie un membre et `this` : celui-là est
  légitime, une lambda serait seulement plus lisible.)
- **Types d'itérateurs écrits en toutes lettres, 23 fois** — la version précédente disait « une
  quarantaine », c'est 23, réparties sur six fichiers dont `imageDownloadTool` (8) et `settingTool`
  (4) :
  `QMap<QString, imageDownloadRuleStruct>::iterator ruleIte = listOfRulesForImage.find(ruleName);`
  `auto` réduit ces lignes de moitié. Le dépôt n'utilise `auto` que **deux fois** aujourd'hui, les
  deux dans `parsingTool.cpp`.
- **`== true` / `== false` — 482 occurrences.** C'est le style maison assumé (voir `CLAUDE.md`),
  donc c'est signalé et non recommandé.
- **`std::optional<QString>`** (C++17) pour la douzaine de fonctions qui renvoient `""` pour dire
  « rien » : `getFirstPageOfTopic`, `getNextPageOfTopic`, `buildLinkForThisPage`, `getForumOfTopic`,
  `jvfLinkToJvcLink`. Cela remplacerait aussi le drapeau `editFormValuesStruct::isValid` écrit à la
  main.
- **`[[nodiscard]]`** sur les accesseurs de `parsingTool` — gratuit, attrape de vraies erreurs.
  Deux réserves : `styleTool::getModelInfo` ne peut **pas** le recevoir, son résultat étant
  légitimement jeté par `mainWindow.cpp:334` (voir A7) ; et sous MSVC c'est `/std:c++17` qui rend
  aussi `[[maybe_unused]]` utilisable, ce qui ferait disparaître le `C5051` de `utilityTool.cpp:32`
  relevé dans `CLAUDE.md`.
- **Variables `inline`** pour les constantes partagées : `settingTool.hpp:45` déclare
  `static const int fastModeSpeedRefresh`, donc une copie par unité de compilation.
- **Liaisons structurées** pour les boucles sur `QJsonObject`/`QMap` (`parsingTool.cpp:292` et
  `431`).

---

## F. Dépréciations Qt

La version précédente donnait quatre entrées et en manquait la moitié. Le relevé complet, dont
`DEFINES += QT_DEPRECATED_WARNINGS` fait déjà sortir une partie :

| Appel déprécié | Remplacement | Emplacements |
| --- | --- | --- |
| `layout->setMargin(n)` | `setContentsMargins(n, n, n, n)` | **15 sites, 11 fichiers** : `showTopic.cpp:44`, `showForum.cpp:85`, `sendMessages.cpp:30`, `preferencesWindow.cpp:37`, `selectStickerWindow.cpp:57,68,80,85,133`, `tabViewTopicInfos.cpp:28`, `webNavigator.cpp:54,62`, `viewThemeInfos.cpp:13`, `multiTypeTextBox.cpp:31`, `containerForTopicsInfos.cpp:21` |
| `QMessageBox::setButtonText` | `addButton` | `showTopic.cpp:392-393` |
| `QSound` (déprécié en 5.15) | `QSoundEffect` | `respawnIrc.cpp:782`, `799` |
| `static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged)` | `QOverload<int>::of(...)` | `preferencesWindow.cpp:352`, `376` |
| `QPalette::Background` | `QPalette::Window` | `selectStickerWindow.cpp:130`, `468`, `471` |
| `QRegularExpression::OptimizeOnFirstUsageOption` | sans effet depuis Qt 5.12, à retirer | `configDependentVar.hpp:12` |
| `QRegExp` | `QRegularExpression` | `highlighter.cpp:105,123,126`, `spellTextEdit.cpp:107,108`, `addPseudoWindow.cpp:43` |

Les deux dernières lignes ne sortent aujourd'hui **qu'en C++17** (voir section E) ou pas du tout.
`QRegExp` n'est pas signalé par `QT_DEPRECATED_WARNINGS` en 5.15 mais quitte le cœur de Qt 6 : c'est
une entrée de `MIGRATION-QT6.md` autant que d'ici.

---

## G. Ce qui n'est pas une affaire de taille de code

**Le point le plus chaud n'était pas celui qui était cité.** `highlighterClass::spellCheck`
(`highlighter.cpp:98-137`) construit **deux `QRegExp` par mot mal orthographié** (lignes 123 et 126,
dans une boucle sur tous les mots du bloc), plus un par bloc (ligne 105). `highlightBlock` est
appelé par Qt **à chaque frappe** dans la zone de saisie. C'est le seul endroit du dépôt où une
compilation de motif est dans une boucle imbriquée sur une action de l'utilisateur. Les deux motifs
des lignes 123 et 126 sont d'ailleurs identiques, construits deux fois de suite.

Ensuite seulement viennent les regex de `parsingTool` : `parsingTool.cpp:1081-1086`, `1090` et
`1126` en construisent sept par message dans `parsingMessages`/`makeBasicPreParseOfMessage`, et
`1144-1148` cinq de plus par appel de `parsingAjaxMessages`. Sur un topic rafraîchi toutes les dix
secondes, cela fait une centaine de compilations de motif par rafraîchissement. Tous les autres
motifs de ces fichiers sont déjà des constantes de portée namespace ; celles-là n'ont simplement pas
été remontées, et le faire ne coûte rien.

Deux mesures à faire avant de toucher au reste, faute de quoi ce serait de l'optimisation à
l'aveugle :

- `payloadTool::getPayload` calcule `qHash(source)` sur **toute la page** à chaque appel, et il est
  appelé une dizaine de fois par chargement (`getAjaxInfo`, `getTopicLocked`, `getPagerInfo`,
  `getNameOfTopic`, `getNumberOfConnected`, `getListOfEntireMessagesWithoutMessagePars`,
  `getListOfHiddenInputFromThisForm`…). Le cache évite bien la décompression et l'analyse JSON, mais
  paie un parcours complet du million de caractères à chaque interrogation. Gratuit en attendant :
  `getListOfHiddenInputFromThisForm` appelle `getPayload(source)` **deux fois de suite**
  (`parsingTool.cpp:288` et `299`) ;
- `styleTool::getColorInfo()` est appelé une quinzaine de fois par message dans `parsingMessages`.
  **Ce n'est pas un problème** — vérifié, il rend une référence constante — et il ne faut pas
  « corriger » ce qui n'est pas cassé.

---

## H. Défauts de comportement croisés en chemin

Ce ne sont pas des refactorisations, mais chacun est la raison pour laquelle la refactorisation
correspondante vaut le coup. Aucun n'a été corrigé.

- **`colorPseudoListWindow.cpp:136-141`** : `QColorDialog::getColor` rend une `QColor` invalide
  quand l'utilisateur annule, et le code écrit quand même `newColor.red()/green()/blue()`, qui valent
  0. **Annuler la boîte de couleur repeint donc le pseudo en noir.** Il manque un
  `if(newColor.isValid())`.
- **`mainWindow.cpp:328`** : `delFavoriteClicked` réécrit le libellé en
  `"Emplacement " + QString::number(index)` alors que le constructeur (ligne 108) écrit
  `QString::number(i + 1)`. Supprimer le premier favori renomme donc son emplacement
  « Emplacement 0 ». C'est exactement ce qu'un tableau et une fonction unique de nommage
  empêcheraient (voir A11).
- **`imageDownloadTool.cpp:131`** : `listOfImagesUrlNeedDownload.erase(ite++)` sur une `QList`.
  C'est l'idiome des conteneurs à nœuds (`std::map`, `std::list`) ; sur un conteneur contigu
  l'itérateur post-incrémenté est invalidé par l'`erase`. La forme correcte est
  `ite = list.erase(ite)`. Le chemin est rare (vidage du cache pendant un téléchargement), ce qui
  explique probablement qu'il n'ait pas mordu.

---

## Ordre suggéré

1. **Code mort et regex sorties des boucles** (sections D et G) — minuscule, sans risque, immédiat.
   Commencer par `highlighter.cpp:123-126`, qui est le seul à toucher une boucle de frappe.
2. **Les extractions de pure donnée** : stickers (A1), smileys (A2), pixmaps (A3), `getModelInfo`
   (A7), patrons de `settingTool` (A8). Mécanique, et c'est là que se trouve la moitié du gain.
3. **Le correcteur orthographique partagé** (section B) — indépendant de tout le reste.
4. **`enum class` pour `pseudoType` / `topicType`** (section C) — débloque l'étape 5, et demande de
   toucher les tests.
5. **`createItemDependingOnTopic` et `setTheseOptions` en tables** (A4 et A6). Relire A6 en entier
   avant de commencer : le `erase()` manquant est un mécanisme, pas un oubli.
6. **La classe de base des fenêtres de liste** (A9), et les tableaux parallèles de A11.
7. **C++17 dans les deux `.pro`**, puis les 9 avertissements qu'il révèle, puis nettoyage `auto` /
   `std::bind` au fil des fichiers touchés.

**Une remarque de méthode qui vaut pour tout ce qui précède.** Les 142 vérifications de
`tests/respawnIrcTests` ne couvrent que `parsingTool` (26 fonctions) et `payloadTool` (2). Or les
plus gros gains de cette liste — les stickers, `getModelInfo`, `showForum`, `mainWindow`, le
correcteur orthographique, `imageDownloadTool` — sont **tous en dehors de cette couverture**. Les
tests protègent l'étape 4 et une partie de l'étape 5 ; ailleurs, la seule protection est de relire
et de lancer le programme. Les points A1, A3 et A7 s'y prêtent bien : leur résultat se voit à
l'écran en ouvrant la fenêtre des stickers, la liste des topics et un thème.
