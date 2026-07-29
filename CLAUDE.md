# RespawnIRC (version PC, Qt)

Client alternatif pour les forums de jeuxvideo.com, en C++/Qt5 Widgets. Le pendant
Android, **beaucoup plus à jour**, est dans `../../repo-android` : c'est la
meilleure référence quand JVC change quelque chose. Regarder son `JVCParser.java` et
son historique git avant de deviner quoi que ce soit.

## Compiler et tester

```bash
cd respawnIrc && qmake && make -j4    # l'exécutable est produit à la racine du projet
cd tests && qmake && make -j4 && ../build/respawnIrcTests
```

Les `.pro` ont un `DESTDIR` : quelle que soit la plateforme et l'endroit d'où `qmake` est lancé,
le programme atterrit à la racine du dépôt et les tests dans `build/`. Il n'y a plus rien à
déplacer à la main après compilation, seuls les objets intermédiaires suivent la façon de compiler.

Dépendances Debian : `qtbase5-dev qtmultimedia5-dev libhunspell-dev qtwebengine5-dev
zlib1g-dev`. zlib sert à décompresser le payload des pages (voir plus bas).

L'exécutable doit tourner depuis la racine du dépôt (il y cherche `themes/` et `resources/`), ce que
le `DESTDIR` lui donne gratuitement.

### macOS

Trois pièges, tous documentés dans le README :

- le `qt@5` de Homebrew n'a **plus** QtWebEngine (Chromium troué), il faut le Qt 5.15.2
  officiel via `aqtinstall` ; Hunspell vient de Homebrew, avec une bibliothèque au nom
  versionné (`-lhunspell-1.7`), et zlib du système ;
- le système de fichiers ignore la casse, donc `RespawnIRC` ne peut pas cohabiter avec le
  dossier `respawnIrc` : on compile un bundle `RespawnIRC.app` posé à la racine du dépôt, et
  les données restent à côté du bundle. C'est à ça que sert `pathTool::dataDirPath()`, à
  utiliser partout plutôt que `QCoreApplication::applicationDirPath()` ;
- pour la même raison la règle `/RespawnIRC` du `.gitignore` attrape le dossier de sources ;
  l'exception `!/respawnIrc/` la neutralise, sans quoi les fichiers ajoutés dans `respawnIrc/`
  n'apparaissent pas dans `git status`.

`./dist-macos.sh ~/Qt/5.15.2/clang_64` fabrique le DMG distribuable : `macdeployqt`, signature ad
hoc, puis un dossier `RespawnIRC` contenant l'application **et** `resources/` et `themes/`. Cette
disposition datait de l'époque où `imageDownloadTool` écrivait les stickers dans
`resources/stickers/`, ce qui interdisait d'enfermer ces dossiers en lecture seule dans le bundle.
Ce n'est plus le cas depuis que les stickers vont dans le cache : **le bundle pourrait maintenant
être autonome**, `resources/` et `themes/` placés dans `Contents/Resources/` et le DMG réduit à un
simple `RespawnIRC.app`. Ça n'a pas été fait, `pathTool::dataDirPath()` cherchant toujours à côté du
bundle.

### Windows

Le point de départ, dont tout le reste découle : **QtWebEngine n'existe pas pour MinGW**, Chromium
ne se compilant qu'avec MSVC. Le README détaille la mise en place ; les pièges à connaître :

- les `.pro` n'ont **pas** été modifiés pour Windows, et ne devraient pas avoir à l'être : tout passe
  par des variables de `qmake`, `HUNSPELL_LIB_NAME`, `ZLIB_LIB_NAME` et `DEFINES+=HUNSPELL_STATIC` ;
- l'installation ciblée des Build Tools demande **deux** composants, `VC.Tools.x86.x64` et un
  Windows SDK. Le premier seul pose `cl.exe` mais aucun `Windows Kits`, et plus rien ne compile :
  depuis Visual Studio 2015 les en-têtes de la bibliothèque C appartiennent au SDK et pas au
  compilateur, un `#include <stdio.h>` suffit à s'en rendre compte ;
- Hunspell et zlib se compilent **à la main**, c'est la méthode documentée : deux petites
  bibliothèques sans dépendance, une quinzaine de secondes. vcpkg reste décrit en second choix. La
  recette ne produit qu'une bibliothèque release, donc `nmake debug` échoue en `LNK2038` sur
  `RuntimeLibrary` et `_ITERATOR_DEBUG_LEVEL` — il faudrait un `hunspelld.lib` compilé en `/MDd` ;
- `HUNSPELL_STATIC` est nécessaire au Hunspell compilé à la main, dont le `hunvisapi.h` teste
  vraiment la macro, mais sans effet sur celui de vcpkg, dont le port engendre un en-tête au test
  figé à `#if 1`. Le passer systématiquement marche donc dans les deux cas, et c'est ce que fait
  `dist-windows.ps1` ;
- la compilation se fait **hors des sources**, dans `build/`, contrairement à Linux et macOS ; seuls
  les objets intermédiaires y restent, le `DESTDIR` envoyant l'exécutable à la racine comme ailleurs ;
- `windeployqt` crée un dossier `resources/` pour QtWebEngine, exactement le nom du dossier de
  données du programme, et au même endroit puisque `pathTool::dataDirPath()` renvoie le dossier de
  l'exécutable. Les deux contenus doivent **fusionner**, pas se remplacer ;
- `dist-windows.ps1` est en UTF-8 **avec BOM** : PowerShell 5.1 lit un `.ps1` comme de l'ANSI sans
  lui, et tous les accents des messages sont abîmés. Ne pas réenregistrer le fichier sans le BOM ;
- toujours dans PowerShell 5.1, `qmake`, `nmake` et `windeployqt` écrivent leur progression sur la
  sortie d'erreur : avec `$ErrorActionPreference = 'Stop'` chaque ligne devient une erreur fatale
  alors que la commande a réussi. D'où `Invoke-BuildTool`, qui juge sur le code de retour.

#### Ce qui manque et ne se voit pas

`windeployqt` ne copie **ni OpenSSL ni les bibliothèques d'exécution de MSVC**. Les trois pièges,
par ordre de méchanceté :

- **sans OpenSSL, aucune page n'est joignable.** Qt 5.15.2 charge `libssl-1_1`/`libcrypto-1_1` à
  l'exécution ; en leur absence `QSslSocket::supportsSsl()` est faux et tout échoue silencieusement.
  Qt ne distribue plus le 1.1.1, seulement un OpenSSL 3 incompatible. Pour vérifier, un programme de
  trois lignes affichant `supportsSsl()` et `sslLibraryVersionString()` suffit et évite de deviner ;
- sans `vcruntime140.dll` et compagnie, le programme ne démarre pas là où le redistribuable n'a
  jamais été installé ;
- sous **Windows 7**, l'Universal CRT n'est pas dans le système : il faut embarquer `ucrtbase.dll`
  et les 45 DLL de redirection qui l'accompagnent, 15 `api-ms-win-crt-*` et 30 `api-ms-win-core-*`
  — les secondes comptent autant que les premières malgré leur nom. C'est l'explication de la pile
  de DLL historiquement distribuée avec le programme — elle était justifiée, ce n'est pas du gras.

**Ce redistribuable se trouve à deux endroits selon l'âge du SDK**, et c'est un piège qui a déjà
coûté une archive : `Redist\ucrt\DLLs\x64` pour les SDK anciens, `Redist\<version>\ucrt\DLLs\x64`
pour les récents (10.0.26100.0 par exemple). `dist-windows.ps1` ne connaissait que le premier et
n'émettait qu'un `Write-Warning`, invisible au milieu de la sortie de `windeployqt` : il assemblait
et compressait quand même une archive sans un seul fichier de l'UCRT, complète pour tout le reste
et incompatible avec Windows 7 sans que rien ne le dise. Il cherche maintenant les deux, le plus
récent d'abord, et `throw` s'il ne trouve rien. **Ne pas rétrograder ce throw en avertissement** :
c'est la seule pièce Windows 7 dont l'absence ne se voit sur aucune machine où l'on peut tester.

Le gras est ailleurs, et `dist-windows.ps1` s'en occupe : traductions de QtWebEngine et de Qt
réduites au français, outils de développement de Chromium retirés, une vingtaine de mégaoctets.
`opengl32sw.dll` (20 Mo) est gardé **volontairement**, c'est le rendu logiciel de secours sur les
machines sans pilote OpenGL, cas courant de la cible Windows 7.

Le plus gros morceau était plus sournois : `windeployqt` embarque `vc_redist.x64.exe`, 24 Mo que
rien ne lance jamais et qui font doublon avec les DLL du runtime copiées à côté de l'exécutable. Il
ne le fait que si `VCINSTALLDIR` est définie, donc seulement quand le script tourne après
`vcvars64.bat` — d'où une archive dont le poids dépendait de la façon de l'appeler, et un fichier
qui n'apparaît pas si on essaie `windeployqt` à la main dans un shell neuf. D'où le
`--no-compiler-runtime`, à ne pas retirer.

Répartition de ce qui reste, pour situer les ordres de grandeur : sur 184 Mo décompressés, **environ
148 tiennent à QtWebEngine**, soit 80 %. Chromium lui-même en fait 112 (`Qt5WebEngineCore.dll` seul
en pèse 97, le reste étant `icudtl.dat` et ses fichiers `.pak`), ANGLE et le rendu logiciel 28
(`opengl32sw.dll`, `libGLESv2.dll`, `libEGL.dll`, `d3dcompiler_47.dll`), et QtQuick, QML et
WebChannel une dizaine. Attention au raisonnement : RespawnIRC est une application Widgets, qui
dessine en raster et n'utilise ni QML ni OpenGL — tout cela n'est là que parce que WebEngine s'en
sert. Le client lui-même, avec Qt Core, Gui, Widgets, Network, OpenSSL et les runtimes, pèse une
trentaine de mégaoctets.

**Rien n'a été essayé sur un vrai Windows 7** : le contenu de l'archive suit les règles documentées
par Microsoft et le programme démarre depuis l'archive sous Windows 10 — revérifié sur une machine
virtuelle vierge, archive décompressée et lancée telle quelle — mais la validation sous Windows 7
reste à faire. Ne pas présenter cette compatibilité comme vérifiée.

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

## Où le programme range ses données

`resources/` et `themes/` sont **livrés avec le programme et jamais écrits**, toujours à côté de
l'exécutable — à côté du bundle sous macOS. Ce que le programme écrit se répartit en **trois
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
  fauter à l'endroit exact. Comme `release` n'a pas de symboles, recompiler avec
  `CONFIG+=force_debug_info` pour obtenir une pile lisible plutôt que des `image00007ff6+0x...`.

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
