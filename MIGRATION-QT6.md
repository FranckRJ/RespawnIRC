# RespawnIRC — passage à Qt 6.11

Analyse du 29 juillet 2026, branche `windows`. Portée : `respawnIrc/`, `tests/`, les deux
`.pro` et les scripts de distribution. **Rien n'a été compilé contre Qt 6** : ce qui suit
vient de la lecture du code et de la documentation de Qt 6.11. Les chiffres de taille, eux,
sont mesurés dans `dist/RespawnIRC-3.1.17-windows.zip`.

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
| `QPalette::Background` | `QPalette::Window` | `selectStickerWindow.cpp:130`, `453`, `456` |
| `QSound` | `QSoundEffect` | `respawnIrc.cpp:775`, `792` |
| `CONFIG += c++14` | `c++17`, exigé par les en-têtes de Qt 6 | `respawnIrc.pro:32`, `tests.pro:2` |

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

**Windows 7 est abandonné.** Comme `CLAUDE.md` le dit franchement, cette compatibilité a été
raisonnée d'après la documentation de Microsoft mais **jamais vérifiée sur une vraie machine
Windows 7** : elle est peut-être bon marché à abandonner. C'est une décision, pas un détail
technique.

À noter aussi : Qt 6.12 sera la dernière version à prendre en charge Windows 10. Cette mise à
jour achète quelques années, pas la tranquillité définitive.

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

## Ordre suggéré

1. **`tests/` d'abord.** La cible ne dépend que de `core gui network` et couvre
   `parsingTool`, `payloadTool`, `styleTool` et `shortcutTool`. Voir les 142 vérifications
   passer sous Qt 6 valide le travail sur les regex avant qu'aucune interface ne soit en
   jeu.
2. **Le programme** : les substitutions de la table A, puis `QSoundEffect` à part.
3. **`dist-windows.ps1` et `dist-macos.sh`**, puis remesurer l'archive et reprendre le
   passage de `CLAUDE.md` sur la répartition des 184 Mo.

Le point de vigilance, s'il ne devait y en avoir qu'un, ce sont les regex du correcteur
orthographique.
