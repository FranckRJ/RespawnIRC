# RespawnIRC — pistes de refactorisation

Analyse du 29 juillet 2026, branche `windows`. Portée : `respawnIrc/`, 10 345 lignes
réparties sur 83 fichiers. Les gains en lignes sont des estimations faites à la lecture,
sans compilation ; rien de ce qui suit n'a été appliqué ni mesuré.

Classé par rapport gain / risque.

---

## A. Duplication supprimable mécaniquement

### 1. La table des smileys — `respawnIrc.cpp:302-373` (72 lignes)

72 `dataInHtml.replace("<img src=\"resources/smileys/N.gif\" />", ":code:")` à la suite,
déjà signalées `//à changer, si possible` par l'auteur d'origine.

C'est de la donnée, et la machinerie existe déjà : `shortcutTool::loadShortcutRule()` lit
exactement ce format (`code<espace>remplacement`) depuis `resources/`, et
`resources/stickerToSmiley.txt` fait la même chose dans l'autre sens. Un
`resources/smileyToCode.txt` plus un appel à `loadShortcutRule` ramène 72 lignes de C++ à
une.

**~70 lignes.**

### 2. Chargement des pixmaps — `showForum.cpp:31-65`

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

### 3. `createItemDependingOnTopic` — `showForum.cpp:171-303`

132 lignes de `if(topicType == X) { if(showXTag) { if(useIcon) setIcon(...) else prefixe =
"[X] " } }`, six fois, seules les chaînes changent. Avec la table du point 2
(`{type → pixmap, balise, "showXTagOnTopicInTopicList"}`), toute la cascade devient une
recherche.

**~70 lignes.**

### 4. `addButtonToButtonLayout` — `respawnIrc.cpp:460-516`

Neuf boutons, chacun avec trois lignes de réglages identiques et un `connect`. Un tableau
`static const` de `{libellé, objectName, &multiTypeTextBoxClass::addBold}` (pointeurs sur
membre) et une boucle.

**~35 lignes.**

### 5. `setTheseOptions` — `respawnIrc.cpp:554-682`

~130 lignes de `if((boolIte = newBoolOptions.find("nom")) != end()) { ... }`, dont les
corps se répartissent en quatre combinaisons de *(mettre à jour sendMessages / mettre à
jour tabView / recharger / effacer)*. Deux tables statiques (une bool, une int) de
`{nomOption, drapeaux}` et une boucle sur ce qui a réellement changé.

C'est aussi là que se cachent les écarts de comportement : la branche
`smartAvatarResizing`/`smartNoelshackResizing` (ligne 614) et les branches
`avatarSize`/`noelshackImageWidth` (lignes 651 et 660) ne font **jamais** l'`erase()` de
leur itérateur, contrairement à toutes leurs voisines. Une table rend cette asymétrie
impossible à écrire par inadvertance.

**~85 lignes.**

### 6. `getModelInfo` — `styleTool.cpp:56-138`

Deux listes parallèles numérotées à la main : 25 `push_back` de valeurs par défaut, puis
25 affectations `listOfLine.at(N)` dont l'indice doit correspondre à l'œil. Un seul
tableau `static const` de `{valeurParDéfaut, pointeurSurMembre}`.

Aujourd'hui, ajouter un champ de thème demande d'éditer deux endroits et de renuméroter ;
après, un seul.

**~45 lignes.**

### 7. Les quatre accesseurs de `settingTool.cpp:135-199`

`getThisBoolOption` / `getThisIntOption` / `getThisStringOption` / `getThisByteOption` sont
la même fonction quatre fois (recherche, repli sur le défaut, `qDebug` en cas d'absence).
Un patron sur le type de valeur.

Même chose pour `getListOfPseudoForTopic` / `getListOfIgnoredPseudo` /
`getListOfTopicLink` (lignes 221-283), identiques au nom de clé près.

**~55 lignes.**

### 8. Les trois fenêtres de liste

`ignoreListWindow.cpp` (96), `colorPseudoListWindow.cpp` (145) et `accountListWindow.cpp`
(122) partagent le même squelette : `QListView` + `QStringListModel`, `NoEditTriggers`,
une grille de boutons Ajouter/Éditer/Supprimer, un `updateList()` qui émet
`listHasChanged`, et la garde `if(currentIndex().row() != -1)` répétée dans chaque slot.

Une `listWindowBaseClass` portant la vue, le modèle et la garde ne laisserait à chaque
classe fille que ses propres boutons. Cela supprime au passage le
`listOfColorPseudo->at(view->currentIndex().row())` écrit **trois fois dans une seule
expression** (`colorPseudoListWindow.cpp:136-138`).

**~80 lignes.**

### 9. La cascade des champs cachés — `parsingTool.cpp:317-354`

38 lignes de `if(captured(1) == "type")` / `if(captured(3) == "name")` imbriqués pour gérer
trois ordres d'attributs possibles. Comme `expForInput` capture trois paires
`nom="valeur"` fixes, parcourir `{captured(1),captured(2)}, {captured(3),captured(4)},
{captured(5),captured(6)}` et y repérer `name`/`value` traite tous les ordres en ~10
lignes — y compris ceux que le code actuel ne gère pas.

**~28 lignes.**

### 10. Petits motifs répétés

- La boucle de rognage des `<br />` en tête et en queue apparaît trois fois
  (`parsingTool.cpp:266-276`, `1097-1107`, et `stringModificatorRemoveFirstsAndLastsPAndBr`).
  Une fonction.
- `networkManager->clearAccessCache(); setCookieJar(new QNetworkCookieJar(this));
  cookieJar()->setCookiesFromUrl(...)` est répété tel quel dans `showTopic`, `showForum`,
  `sendMessages`, `messageActions` et `getTopicMessages`. Un
  `utilityTool::applyCookie(manager, cookie, website)`.
- Le motif `if(reply->isOpen()) connect(...) else { analyze(); manager->deleteLater();
  manager = nullptr; }` apparaît quatre fois à l'identique.

**Total réaliste pour la section A : 500 à 600 lignes**, environ 6 % du code, sans
changement de comportement.

---

## B. Chaînes de caractères qui devraient être des énumérations

`pseudoInfoStruct::pseudoType` est un `QString` contenant `"user"`/`"modo"`/`"admin"`/
`"staff"` ; `topicStruct::topicType` contient `"topic-pin-on"`/`"topic-lock"`/…

Ces valeurs pilotent des chaînes de comparaisons dans `showTopic.cpp:642-674`,
`showForum.cpp:189-300` et `parsingTool.cpp:816-863`. En `enum class`, elles deviennent des
indices de tableau ou des `switch` dont le compilateur vérifie l'exhaustivité — et une
faute de frappe dans `"topic-resolved"` cesserait d'être un repli silencieux vers
`topic-folder`.

C'est le changement qui rend les points 3 et 6 naturels plutôt qu'astucieux.

---

## C. Code mort

- `parsingTool::generateNewUserAgent()` — déclaré (`parsingTool.hpp:97`), jamais défini,
  jamais appelé.
- `parsingTool::getErrorMessageInJSON()` — défini (`parsingTool.cpp:384-403`) mais appelé
  de nulle part ; `getErrorOfMessageSending` l'a remplacé. ~20 lignes, plus
  `expForErrorInJSON`.
- `showTopicClass::replaceTextOrRemoveIt` — enrobage de deux lignes autour de
  `replace`/`remove`, utilisé dans une seule fonction.

---

## D. C++ plus récent

Le projet est en `CONFIG += c++14` avec Qt 5.15.2.

**Passer à C++17 est sans danger** : Qt 5.15 le prend entièrement en charge, et les trois
chaînes d'outils du projet (gcc Debian, clang Homebrew/aqtinstall, MSVC 2019) le digèrent
sans nouvelle dépendance. C++20 est à éviter ici : `strict_c++` plus MSVC plus les en-têtes
de Qt 5.15, pour peu de gain.

Mais le point important est ailleurs : **C++11 n'est pas encore utilisé**, et encore moins
C++14.

- **`std::bind` — `parsingTool.cpp:1004-1077`.** Sept appels à
  `std::bind(f, std::placeholders::_1)` où `f` est déjà une fonction `QString(QString)`. Le
  `bind` ne fait rien : passer `f` directement. Des lambdas là où un `bind` est réellement
  nécessaire.
- **Types d'itérateurs écrits en toutes lettres**, une quarantaine de fois :
  `QMap<QString, imageDownloadRuleStruct>::iterator ruleIte = listOfRulesForImage.find(ruleName);`
  `auto` réduit ces lignes de moitié et les rend lisibles.
- **`== true` / `== false` — 477 occurrences.** C'est le style maison assumé (voir
  `CLAUDE.md`), donc c'est signalé et non recommandé : ce sont 477 endroits où `if(x)` se
  lirait pareil, et c'est ce qui fait déborder des lignes comme `parsingTool.cpp:461`.
  Décision du mainteneur, mais c'est la première source de bruit horizontal.
- **`std::optional<QString>`** (C++17) pour la douzaine de fonctions qui renvoient `""`
  pour dire « rien » : `getFirstPageOfTopic`, `getNextPageOfTopic`, `buildLinkForThisPage`,
  `getForumOfTopic`, `jvfLinkToJvcLink`. Cela remplacerait aussi le drapeau
  `editFormValuesStruct::isValid` écrit à la main.
- **`[[nodiscard]]`** sur les accesseurs de `parsingTool` — gratuit, attrape de vraies
  erreurs.
- **Définitions de namespaces imbriqués** (`namespace parsingTool::detail`) et **variables
  `inline`** pour les constantes de regex partagées.
- **Liaisons structurées** pour les boucles d'itération sur `QJsonObject`/`QMap`
  (`parsingTool.cpp:292` et `431`).

---

## E. Dépréciations Qt déjà signalées à la compilation

`DEFINES += QT_DEPRECATED_WARNINGS` est actif, ces avertissements sortent donc déjà :

| Appel déprécié | Remplacement | Emplacements |
| --- | --- | --- |
| `layout->setMargin(0)` | `setContentsMargins(0, 0, 0, 0)` | `showTopic.cpp:44`, `showForum.cpp:85`, `sendMessages.cpp:30`, `preferencesWindow.cpp:37` |
| `QMessageBox::setButtonText` | `addButton` | `showTopic.cpp:392-393` |
| `QSound` (déprécié en 5.15) | `QSoundEffect` | `respawnIrc.cpp:775`, `792` |
| `static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged)` | `QOverload<int>::of(...)` | `preferencesWindow.cpp:352`, `376` |

---

## F. Ce qui n'est pas une affaire de taille de code

`parsingTool.cpp:1081-1086`, `1090`, `1126` et `1144-1148` construisent des
`QRegularExpression` **en ligne, à chaque appel**. `parsingMessages` tourne une fois par
message et `parsingAjaxMessages` une fois par erreur : cela fait une dizaine de
compilations de regex par message, sur un topic rafraîchi toutes les dix secondes.

Tous les autres motifs du fichier sont déjà des constantes de portée namespace ; ces
dix-là n'ont simplement pas été remontées. Les déplacer ne coûte rien et c'est le seul gain
de performance gratuit repéré.

---

## Ordre suggéré

1. **Code mort et regex en ligne** (sections C et F) — minuscule, sans risque, immédiat.
2. **Les extractions de pure donnée** : smileys, pixmaps, `getModelInfo`, patrons de
   `settingTool`. Mécanique, et `tests/` couvre déjà le versant parsing.
3. **`enum class` pour `pseudoType` / `topicType`** — débloque l'étape 4.
4. **`createItemDependingOnTopic` et `setTheseOptions` en tables.**
5. **La classe de base des fenêtres de liste.**
6. **C++17 dans les deux `.pro`**, puis nettoyage `auto` / `std::bind` au fil des fichiers
   touchés.

Les étapes 1 et 2 se font sans compilation disponible. À partir de la 3, il faut d'abord
faire tourner `tests/respawnIrcTests` : les 142 vérifications couvrent le parsing, mais
rien dans `showForum` ni `respawnIrc`.
