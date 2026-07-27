# RespawnIRC (version PC, Qt)

Client alternatif pour les forums de jeuxvideo.com, en C++/Qt5 Widgets. Le pendant
Android, **beaucoup plus à jour**, est dans `../../repo-android` : c'est la
meilleure référence quand JVC change quelque chose. Regarder son `JVCParser.java` et
son historique git avant de deviner quoi que ce soit.

## Compiler et tester

```bash
cd respawnIrc && qmake && make -j4    # l'exécutable est déplacé à la racine du projet
cd tests && qmake && make -j4 && ./respawnIrcTests
```

Dépendances Debian : `qtbase5-dev qtmultimedia5-dev libhunspell-dev qtwebengine5-dev
zlib1g-dev`. zlib sert à décompresser le payload des pages (voir plus bas).

L'exécutable doit tourner depuis la racine du dépôt (il y cherche `themes/`,
`resources/` et `config.ini`).

## Logs et diagnostic

Rien n'est journalisé par défaut. Avec `RESPAWNIRC_DEBUG=1` :

```bash
RESPAWNIRC_DEBUG=1 ./RespawnIRC
```

- toutes les catégories `respawnirc.*` passent en debug,
- les logs sont écrits dans `logs/respawnirc.log`,
- les pages dont l'analyse échoue sont sauvegardées dans `logs/page-*.html` (20 max).

Pour n'activer qu'une catégorie sans écrire de fichier :
`QT_LOGGING_RULES="respawnirc.parsing.debug=true" ./RespawnIRC`.
Catégories : `respawnirc.network`, `.parsing`, `.topic`, `.forum` (voir `logTool.hpp`).

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

## Conventions

- Commentaires et messages de commit en français, sans retour à la ligne manuel.
- Style du dépôt : `if(x == true)`, accolades sur leur propre ligne, noms en
  `camelCase` suffixés (`...Class`, `...Struct`, `expFor...`).
- Le `.gitignore` couvre les objets de compilation ; ne pas committer `RespawnIRC`,
  `config.ini`, ni `logs/`.
