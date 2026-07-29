# RespawnIRC — passage à Qt 6

Analyse du 29 juillet 2026, branche `windows`. Portée : `respawnIrc/`, `tests/`, les deux
`.pro` et les scripts de distribution. **Rien n'a été compilé contre Qt 6** : ce qui suit
vient de la lecture du code et de la documentation de Qt 6.11. Les chiffres de taille, eux,
sont mesurés dans `dist/RespawnIRC-3.1.17-windows.zip`.

**Version visée : Qt 6.12 LTS**, attendue en septembre 2026, dernière version à prendre en
charge Windows 10 — et destinée à être gardée telle quelle ensuite. La section F explique ce
que ce gel implique ; tout le reste du document vaut pour n'importe quelle version de Qt 6,
le portage étant le même.

Le résumé tient en une phrase : **le portage du C++ est petit, une poignée de substitutions
mécaniques dans huit fichiers, et tout le coût réel est dans la distribution et les
plates-formes abandonnées.**

La section E de [REFACTORING.md](REFACTORING.md) relève déjà `setMargin` et `QSound` comme
dépréciations signalées à la compilation. Sous Qt 6 ce ne sont plus des avertissements mais
des suppressions : les deux listes se recouvrent, celle ci-dessous est complète.

---

## A. Ce que le code source impose

| Appel supprimé dans Qt 6 | Remplacement | Emplacements |
| --- | --- | --- |
| `QRegExp` | `QRegularExpression` | `addPseudoWindow.cpp:43`, `highlighter.cpp:105`, `123`, `126`, `spellTextEdit.cpp:107-108` |
| `QTextCodec` | `QStringDecoder` | `highlighter.hpp:32`, `highlighter.cpp:51`, `63`, `spellTextEdit.hpp:40`, `spellTextEdit.cpp:85`, `99` |
| `QLayout::setMargin` | `setContentsMargins(0, 0, 0, 0)` | `containerForTopicsInfos.cpp:21`, `multiTypeTextBox.cpp:31`, `preferencesWindow.cpp:37`, `selectStickerWindow.cpp:57`, `68`, `80`, `85`, `sendMessages.cpp:30`, `showForum.cpp:85`, `showTopic.cpp:44`, `tabViewTopicInfos.cpp:28`, `viewThemeInfos.cpp:13`, `webNavigator.cpp:54`, `62` |
| `QPalette::Background` | `QPalette::Window` | `selectStickerWindow.cpp:130`, `468`, `471` |
| `QSound` | `QSoundEffect` | `respawnIrc.cpp:775`, `792` |
| `CONFIG += c++14` | `c++17`, exigé par les en-têtes de Qt 6 | `respawnIrc.pro:45`, `tests.pro:2` |

Quatorze `setMargin` sur des layouts, donc, et non quatre. Attention en revanche à
`selectStickerWindow.cpp:133` : celui-là est un `QLabel::setMargin`, qui **existe toujours**
dans Qt 6 et ne doit pas être touché.

`parsingTool.cpp` est déjà entièrement en `QRegularExpression` (une bonne centaine
d'occurrences) : l'idiome est en place, et toutes les surcharges employées ailleurs
(`contains`, `split`, `count`, `indexOf`, `lastIndexOf`) existent pour `QRegularExpression`.

`QT += core5compat` ferait compiler `QRegExp` et `QTextCodec` sans y toucher, ce qui permet
d'étaler le portage. Pour six appels, ça ne vaut pas la peine de traîner le module.

### Deux pièges

**`\w` change de sens.** `QRegExp` reconnaissait les lettres Unicode ; le `\w` de PCRE est
limité à l'ASCII tant qu'on ne passe pas `QRegularExpression::UseUnicodePropertiesOption`.
Or tous les motifs de frontière de mot du correcteur orthographique en dépendent
(`[^\w'-]` en `spellTextEdit.cpp:107-108`, `[^\w'-]+` et `\b…\b` en `highlighter.cpp:105`,
`123`, `126`) — sur un dictionnaire **français**. Porté sans l'option, « café » devient
silencieusement deux mots. C'est de loin la régression la plus probable de tout le
portage.

**`QSoundEffect` n'est pas un remplacement en une ligne.** Pas de `play()` statique : il
faut des objets membres, `setSource(QUrl::fromLocalFile(...))`, et le chargement est
asynchrone. C'est le seul point de la table A qui demande de réécrire plutôt que de
substituer.

Tant qu'on est dans `highlighter.cpp:123`, le mot est interpolé tel quel dans `\b…\b`. Il
ne l'était pas davantage sous `QRegExp`, mais `QRegularExpression::escape()` est juste à
côté.

### Le décodeur et les dictionnaires

`resources/fr.aff` déclare `SET UTF-8`, et `QStringConverter` couvre l'UTF-8 : la
substitution de `QTextCodec` est indolore telle que le programme est livré. Elle ne mord que
si quelqu'un dépose un dictionnaire en ISO-8859-15 dans `resources/`, encodage que
`QStringDecoder` ne sait pas traiter du tout alors que `QTextCodec` le faisait.

### QtWebEngine

Rien à changer : pas de `QWebEngineSettings::globalSettings()`, aucune API supprimée dans ce
qui est utilisé. À surveiller quand même, `connectWindow.cpp:64-82`, où un profil hors
enregistrement est rattaché à la fenêtre et la page à la vue : Qt 6 est plus bruyant quand un
profil survit à ses pages.

---

## B. Compilation

qmake est toujours livré et `QT += webenginewidgets` fonctionne toujours pour une
application — le « legacy, non recommandé » de la documentation vise la compilation de Qt
lui-même, pas son utilisation. Les `.pro` survivent donc tels quels, au `c++17` près.

Mais c'est rester sur un chemin que Qt promet de retirer depuis Qt 6.0, et si le projet doit
passer à CMake un jour, c'est **cette** mise à jour qui est le bon moment, pas la suivante.

**Sans effet sur le choix de vcpkg.** Hunspell et zlib ne dépendent pas de Qt : les mesures
de `CLAUDE.md` — quinze secondes et 44 Mo à la main contre dix minutes et 912 Mo par vcpkg,
dont les deux tiers pour un libiconv inutile ici — restent valables mot pour mot sous Qt 6.
La seule chose qui rapprocherait vcpkg serait le passage à CMake, dont il est le compagnon
naturel ; tant qu'on reste sur qmake, les deux décisions sont indépendantes.

---

## C. Distribution

C'est là qu'est le travail, et l'essentiel est du retrait.

**OpenSSL disparaît complètement.** Qt 6 a des greffons de chiffrement interchangeables et
se rabat sur Schannel quand OpenSSL est absent. En livrant `qschannelbackend`, le dossier
`openssl/`, les deux DLL, la vérification en tête de `run-windows.ps1` et de
`dist-windows.ps1`, et le paragraphe d'excuses du README sur un OpenSSL 1.1.1 non maintenu
depuis septembre 2023 s'en vont tous ensemble. C'est le gain le plus net de l'opération.

**La pile de l'Universal CRT disparaît aussi** — mais seulement parce que Windows 7 n'est
plus une cible, voir la section D.

**QtMultimedia devient un sujet.** Aujourd'hui il coûte 0,95 Mo sur 184, mesuré dans
l'archive : `Qt5Multimedia.dll` 0,71 Mo, `audio/qtaudio_wasapi.dll` et
`audio/qtaudio_windows.dll` 0,15 Mo, `mediaservice/qtmedia_audioengine.dll` 0,06 Mo,
`playlistformats/qtmultimedia_m3u.dll` 0,03 Mo. Sous Qt 6.5 et suivants le moteur par défaut
est FFmpeg, et `windeployqt` copie `avcodec`, `avformat`, `avutil`, `swresample` et
`swscale` : des dizaines de mégaoctets pour deux bips. La documentation dit que si ces
bibliothèques manquent au démarrage, Qt se rabat sur le moteur natif du système — donc
`dist-windows.ps1` peut les supprimer après `windeployqt`, exactement comme il allège déjà
les traductions. **À vérifier plutôt qu'à supposer** : il faut écouter les deux sons.

Le reste ne bouge pas dans son principe : `--no-compiler-runtime` reste nécessaire pour la
même raison, l'allègement des traductions et la fusion des deux `resources/` aussi, avec des
noms de fichiers renumérotés (`Qt6WebEngineCore.dll` et compagnie). La répartition des 184 Mo
documentée dans `CLAUDE.md` sera à refaire de zéro.

---

## D. Ce que ça coûte

| | Qt 5.15.2 | Qt 6.11 |
| --- | --- | --- |
| Windows minimum | Windows 7 | **Windows 10 1809** |
| macOS minimum | 10.13 | **macOS 13**, Xcode 15 et SDK macOS 14 |
| Compilateur Windows | MSVC 2019 | MSVC 2022, SDK Windows 11 ≥ 10.0.26100 |
| QtWebEngine avec MinGW | non | **toujours non** |

Les minimums de 6.12 ne sont pas encore publiés ; ceux de 6.11 sont donnés ici comme
approximation, seule la fin de la prise en charge de Windows 10 après 6.12 étant annoncée.

**Windows 7 est abandonné.** Comme `CLAUDE.md` le dit franchement, cette compatibilité a été
raisonnée d'après la documentation de Microsoft mais **jamais vérifiée sur une vraie machine
Windows 7** : elle est peut-être bon marché à abandonner. C'est une décision, pas un détail
technique.

Toute la pile de DLL de l'Universal CRT (`ucrtbase.dll` et la quarantaine de
`api-ms-win-crt-*.dll`) part avec Windows 7, puisqu'elle n'existait que pour lui. Les
bibliothèques C++ de MSVC, elles, restent nécessaires.

---

## E. Ce que ça rapporte

- la disparition d'OpenSSL décrite en C ;
- un Chromium de cette décennie au lieu de celui de 5.15.2, dont le README déconseille
  lui-même de se servir comme navigateur généraliste ;
- **arm64 natif sur les Mac Apple Silicon** : plus de Rosetta 2, et la note du README sur une
  application x86_64 seulement disparaît ;
- le `qt` de Homebrew, en version 6, **fournit QtWebEngine** : le détour par `aqtinstall`
  devient facultatif sous macOS ;
- « la dernière version librement téléchargeable » était une contrainte du LTS Qt 5. Les
  versions intermédiaires de Qt 6, dont 6.11, sont libres d'accès.

---

## F. Licence, et ce que « rester sur 6.12 » veut dire

**Qt 6 n'est pas commercial.** Double licence LGPLv3 / commerciale, exactement comme Qt 5, et
aucune version de Qt 6 n'est hors de portée du projet. Ce qui est restreint, ce sont les
**branches LTS** : elles restent ouvertes aux utilisateurs libres jusqu'à la sortie de la
version majeure suivante, après quoi les correctifs partent dans un dépôt réservé aux
licences commerciales. C'est exactement le mécanisme qui a fait de 5.15.2 la dernière Qt 5
librement téléchargeable — 5.15 était une LTS. Le README décrit le symptôme sans le nommer.

Rien de tout cela ne change la façon de compiler : l'édition de liens est dynamique, les DLL
sont posées à côté de l'exécutable et les frameworks dans le bundle, ce qui est l'arrangement
LGPL habituel et déjà celui de 5.15.2. Aucune étape ne gagne d'identification, `aqtinstall`
continue de fonctionner sans compte Qt.

Trois détails changent quand même dans la commande d'installation :

- la version et l'architecture (`win64_msvc2022_64` sous Windows, une architecture arm64
  native sous macOS) ;
- **les modules à nommer.** `-m qtwebengine` suffisait sous Qt 5 ; Qt 6 sort les dépendances
  de WebEngine en modules distincts, QtWebChannel, QtPositioning et QtDeclarative, à demander
  explicitement ;
- depuis Qt 6.8, WebEngine a migré vers la zone « Extensions » de l'installateur et les
  vieilles versions d'`aqtinstall` ne savent pas l'y chercher. À vérifier avec
  `aqt list-qt windows desktop --modules 6.12.0 win64_msvc2022_64` avant d'écrire quoi que ce
  soit dans le README, plutôt que de recopier la commande de Qt 5.

### Le calendrier du gel

6.12.0 sort en septembre 2026 ; la branche reste ouverte jusqu'à la sortie de 6.13, soit vers
mars-avril 2027 si l'on en juge par la fin de prise en charge de 6.11 le 17 mars 2027. **Il
faut donc prendre la dernière 6.12.x publiée à ce moment-là**, et c'est celle-là qui est
gelée, pas la 6.12.0 de septembre.

Et puisque « pour toujours » veut dire que la compilation doit encore fonctionner dans cinq
ans : **archiver le paquet Qt exact**, quelque part de durable. Les serveurs de Qt élaguent
les vieilles versions, et une chaîne de compilation qui dépend d'un téléchargement finit par
dépendre d'un 404.

En échange, le tapis roulant n'existe pas : c'est un portage unique suivi d'une dépendance
figée, nettement moins cher que de suivre les versions intermédiaires tous les six mois.

### Ce qu'il faut assumer : un Chromium figé

C'est le seul vrai coût du gel, et il mérite d'être mesuré plutôt que redouté.

L'exposition est plus étroite qu'il n'y paraît. `useInternalNavigatorForLinks` vaut **`false`
par défaut** (`settingTool.cpp:65`) : dans la configuration livrée, les liens des messages
partent vers le navigateur du système, qui est à jour. Le Chromium embarqué ne rend que la
page de connexion de jeuxvideo.com et ce que l'utilisateur ouvre délibérément dans
« RespawnIRC Navigator ». Ce n'est pas nul — la fenêtre a une barre d'adresse, les pages de
JVC portent des publicités tierces, et l'option est à une case à cocher — mais ce n'est pas
un navigateur exposé à des URL arbitraires choisies par un tiers.

Deux choses achèvent de relativiser :

- **Windows 10 n'est lui-même plus maintenu.** Viser cette plate-forme, c'est déjà servir un
  système qui ne reçoit plus de correctifs ; un Chromium figé n'y est pas le risque dominant.
- **C'est une amélioration, pas une régression.** Le programme tourne aujourd'hui sur le
  Chromium de 5.15.2, figé depuis 2020 et déjà non maintenu — c'est la raison pour laquelle
  Homebrew a retiré WebEngine de son `qt@5`, comme le rappelle `CLAUDE.md`. Un Chromium de
  2026 gelé en 2027 reste des années meilleur que la situation actuelle.

La conclusion honnête est donc que le gel se défend. La seule mesure qu'il appelle est
gratuite : **garder `useInternalNavigatorForLinks` à `false`**, et le traiter désormais comme
un défaut de sécurité et non comme une simple préférence d'affichage.

Une nuance à ne pas mal lire, enfin : le code de WebEngine reste sous ses licences d'origine
même pendant la phase commerciale d'une LTS, parce qu'une partie n'existe que sous LGPL. Les
sources des correctifs sont donc publiées — mais il n'y a pas de binaires libres, et
reconstruire QtWebEngine sous MSVC, c'est compiler Chromium. Disponible en droit, hors de
portée en pratique pour ce projet.

---

## G. Conformité LGPL : un trou qui précède tout ça

L'archive distribuée compte 464 entrées et **aucun texte de licence** : ni la LGPLv3 de Qt,
ni les mentions des bibliothèques tierces. Livrer des bibliothèques LGPLv3 s'accompagne
d'obligations — joindre la licence, offrir les sources correspondantes, permettre la
réédition de liens, ce que l'édition dynamique satisfait déjà.

La licence zlib du programme lui-même n'entre en conflit avec rien de tout cela.

C'est vrai de l'archive Qt 5 d'aujourd'hui : ce n'est donc pas un argument contre la mise à
jour, mais c'est la seule chose de cette page qui soit un manquement plutôt qu'un arbitrage,
et une migration est le bon moment pour ajouter un dossier `licenses/` aux deux scripts de
distribution.

---

## Ordre suggéré

La version visée sort en septembre, mais il n'y a aucune raison d'attendre : le portage est
le même pour toute version de Qt 6, et 6.11 est disponible aujourd'hui. Le faire maintenant
transforme septembre en simple changement de numéro de version.

**Maintenant, sur 6.11 :**

1. **`tests/` d'abord.** La cible ne dépend que de `core gui network` et couvre
   `parsingTool`, `payloadTool`, `styleTool` et `shortcutTool`. Voir les 142 vérifications
   passer sous Qt 6 valide le travail sur les regex avant qu'aucune interface ne soit en
   jeu.
2. **Le programme** : les substitutions de la table A, puis `QSoundEffect` à part.
3. **`dist-windows.ps1` et `dist-macos.sh`**, puis remesurer l'archive et reprendre le
   passage de `CLAUDE.md` sur la répartition des 184 Mo.

**En septembre 2026 :** recompiler sur 6.12.0, qui ne devrait demander qu'un changement de
chemin, et vérifier qu'aucune dépréciation n'est apparue entre les deux versions.

**Vers mars-avril 2027, à la sortie de 6.13 :** prendre la dernière 6.12.x librement
publiée, l'archiver, et s'y arrêter.

Le point de vigilance, s'il ne devait y en avoir qu'un, ce sont les regex du correcteur
orthographique.
