# RespawnIRC — passage à Qt 6

Analyse du 29 juillet 2026, branche `windows`. Portée : `respawnIrc/`, `tests/`, les deux
`.pro` et les scripts de distribution. **Rien n'a été compilé contre Qt 6** : ce qui suit
vient de la lecture du code et de la documentation de Qt 6.11. Les chiffres de taille, eux,
sont mesurés dans `dist/RespawnIRC-3.1.17-windows.zip`.

Relu ligne à ligne le 31 juillet 2026 sur la branche `macos2`, code en main. Trois choses en
sont sorties, et elles sont signalées à leur place : **des numéros de ligne avaient dérivé**
depuis le 29, **la table A n'était pas complète** comme elle le prétendait, et **deux
justifications étaient fausses** — celle du retrait de FFmpeg en section C et celle de la durée
de vie libre d'une LTS en section F. Les conclusions, elles, tiennent toutes.

**Version visée : la série Qt 6.12 LTS**, dont la première sort le 22 septembre 2026, dernière
version à prendre en charge Windows 10 — et destinée à être gardée ensuite. Le gel porte sur la
**série 6.12.x, pas sur la 6.12.0** : tant que la branche reste ouverte aux utilisateurs libres
on prend chaque correctif publié, et c'est la dernière 6.12.x parue qui devient la version figée.
La section F explique ce que ce gel implique ; tout le reste du document vaut pour n'importe
quelle version de Qt 6, le portage étant le même.

Le résumé tient en une phrase : **le portage du C++ est petit — dix-sept fichiers touchés, dont
un seul, `respawnIrc.cpp`, demande de réécrire plutôt que de substituer — et tout le coût réel est
dans la distribution et les plates-formes abandonnées.** Le décompte disait « huit fichiers », ce
qui était deux fois trop peu ; ça ne change pas la conclusion, les quatorze `setMargin` étant à eux
seuls répartis sur dix fichiers pour une ligne chacun. Les deux `.pro` s'ajoutent aux dix-sept.

La section E de [REFACTORING.md](REFACTORING.md) relève déjà `setMargin` et `QSound` comme
dépréciations signalées à la compilation. Sous Qt 6 ce ne sont plus des avertissements mais
des suppressions : les deux listes se recouvrent, et celle ci-dessous se veut complète — elle ne
l'était pas, et ce qui lui manquait est signalé sous la table.

---

## A. Ce que le code source impose

| Appel supprimé dans Qt 6 | Remplacement | Emplacements |
| --- | --- | --- |
| `QRegExp` | `QRegularExpression` | `addPseudoWindow.cpp:6`, `43`, `highlighter.cpp:5`, `105`, `123`, `126`, `spellTextEdit.cpp:10`, `107-108` |
| `QTextCodec` | `QStringEncoder`, plus un `QStringDecoder` pour le seul `toUnicode` | `highlighter.hpp:7`, `32`, `highlighter.cpp:51`, `63`, `88`, `143`, `spellTextEdit.hpp:12`, `40`, `spellTextEdit.cpp:58`, `85`, `99`, `153`, `163`, `239`, `279`, `292` |
| `QTextStream::setCodec` | `setEncoding(QStringConverter::Utf8)` | `tests/main.cpp:20` |
| `QLayout::setMargin` | `setContentsMargins`, **et pas toujours avec des zéros** | `containerForTopicsInfos.cpp:21`, `multiTypeTextBox.cpp:31`, `preferencesWindow.cpp:37`, `selectStickerWindow.cpp:57`, `68`, `80`, `85`, `sendMessages.cpp:30`, `showForum.cpp:85`, `showTopic.cpp:44`, `tabViewTopicInfos.cpp:28`, `viewThemeInfos.cpp:13`, `webNavigator.cpp:54`, `62` |
| `QPalette::Background` | `QPalette::Window` | `selectStickerWindow.cpp:130`, `468`, `471` |
| `QSound` | `QSoundEffect` | `respawnIrc.cpp:1`, `782`, `799` |
| `CONFIG += c++14` | `c++17` au minimum ; voir « Jusqu'où va le C++ » en section B | `respawnIrc.pro:128`, `tests.pro:2` |

Les emplacements sont ceux de la branche `macos2` au 31 juillet 2026. **Ils avaient dérivé** dans
la première version de cette table, et de deux façons différentes qu'il vaut mieux connaître avant
de s'y fier : `respawnIrc.pro:45` était devenu `:128`, le `.pro` ayant grossi entre-temps, et les
deux `QSound` étaient donnés sept lignes trop haut. **Vingt-neuf des trente-deux emplacements
étaient justes**, et c'est le pire cas de figure — une table qu'on sonde au hasard paraît bonne.
Les vérifier tous, ou mieux, les retrouver au `grep` le jour du portage.

Quatorze `setMargin` sur des layouts, donc, et non quatre. Deux pièges dans cette seule ligne :

- **quatre de ces quatorze ne mettent pas 0**, et une substitution mécanique vers
  `setContentsMargins(0, 0, 0, 0)` changerait l'apparence sans rien casser à la compilation, donc
  sans que rien ne le signale : `preferencesWindow.cpp:37` met 5, `selectStickerWindow.cpp:57` met
  1, `selectStickerWindow.cpp:80` met 5 et `webNavigator.cpp:54` met 2. `setMargin(n)` vaut
  `setContentsMargins(n, n, n, n)` ;
- `selectStickerWindow.cpp:133` est un `QLabel::setMargin`, qui **existe toujours** dans Qt 6 et
  ne doit pas être touché. C'est le seul des quinze `setMargin` du programme dans ce cas.

`QTextCodec` sert seize fois, et non six : la première table ne comptait que les deux déclarations
et les quatre affectations, en laissant de côté les deux `#include`, les sept `fromUnicode` et
l'unique `toUnicode` qui sont la raison d'être du membre. Cela change le remplacement : le code
**encode** presque toujours — du Qt vers ce qu'attend Hunspell — donc c'est `QStringEncoder`
qu'il faut, `QStringDecoder`
n'étant nécessaire que pour `spellTextEdit.cpp:163`, qui relit les suggestions de Hunspell. Deux
membres à la place d'un, et tous deux non copiables et à `operator()` non-`const` : les méthodes
qui s'en servent ne pourront pas être `const`. Aucune ne l'est aujourd'hui.

`tests/main.cpp:20` manquait complètement, et c'est celui qui mord le plus tôt : la cible `tests`
est la première étape de l'ordre suggéré en fin de document, et elle ne compilerait pas.

`parsingTool.cpp` est déjà entièrement en `QRegularExpression` (une bonne centaine
d'occurrences) : l'idiome est en place, et toutes les surcharges employées ailleurs
(`contains`, `split`, `count`, `indexOf`, `lastIndexOf`) existent pour `QRegularExpression`.

`QT += core5compat` ferait compiler `QRegExp` et `QTextCodec` sans y toucher, ce qui permet
d'étaler le portage. Pour ce que ces deux classes représentent ici — cinq fichiers, tous du
correcteur orthographique —, ça ne vaut pas la peine de
traîner le module ; et il **ne couvre pas tout** : son `qtextcodec.h` ne déclare que `QTextCodec`,
`QTextEncoder` et `QTextDecoder`, rien qui touche à `QTextStream`. Un module de compatibilité
n'ajoute pas de méthode à une classe de `qtbase`, donc le `setCodec` de `tests/main.cpp:20` reste à
corriger dans tous les cas. Ce n'est pas une objection décisive, c'est une raison de ne pas croire
que le module rende le portage optionnel.

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
`QStringConverter` ne sait pas traiter du tout alors que `QTextCodec` le faisait. Une nuance qui
compte si le cas se présente : `QStringConverter` ne connaît que l'UTF-8, l'UTF-16, l'UTF-32, le
**Latin-1** et l'encodage du système. Un dictionnaire en ISO-8859-**1** passerait donc, ce qui est
l'encodage non-UTF-8 le plus courant des dictionnaires Hunspell ; c'est le 15 et les autres qui
tombent. La perte est donc plus étroite qu'un « QTextCodec savait tout faire » ne le laisse croire,
mais elle est réelle et silencieuse — d'où les neuf gardes du paragraphe suivant.

Un détail de forme qui va avec : `QTextCodec::codecForName()` rendait un pointeur nul sur un nom
inconnu, et c'est ce nul que le code teste partout — `codec != nullptr` ou
`codecUsed != nullptr`, **neuf fois**, en plus des seize emplacements de la table. Les nouvelles
classes n'ont pas de nul : c'est `QStringEncoder::isValid()` qui répond, ou
`QStringConverter::encodingForName()` qui rend un `std::optional`. Ces neuf tests sont donc à
réécrire eux aussi, et ce ne sont pas des déclarations mais des gardes — les laisser tomber
rendrait muet le cas du dictionnaire à l'encodage inconnu, qui est précisément celui du paragraphe
précédent.

### QtWebEngine

Rien à changer : pas de `QWebEngineSettings::globalSettings()`, aucune API supprimée dans ce
qui est utilisé. `QWebEnginePage::createWindow`, `QWebEnginePage::action` et le constructeur
`QWebEngineProfile(QObject*)` du profil hors enregistrement existent tous encore. À surveiller
quand même, `connectWindow.cpp:76-92` — l'analyse disait `64-82`, la même dérive qu'ailleurs —
où un profil hors enregistrement est rattaché à la fenêtre et la page à la vue : Qt 6 est plus
bruyant quand un profil survit à ses pages.

---

## B. Compilation

qmake est toujours livré, et le manuel de qmake de Qt 6.11 ne le déclare ni obsolète ni
déconseillé — le « legacy, non recommandé » qu'on lit ailleurs vise la compilation de Qt
lui-même, pas son utilisation. Les `.pro` survivent donc tels quels, à la ligne du standard C++
près.

**C'est néanmoins la première chose à vérifier, avant même de toucher au code**, et la
vérification tient en un test de présence de fichier : `QT += webenginewidgets` a besoin d'un
`mkspecs/modules/qt_lib_webenginewidgets.pri` dans le Qt installé, et QtWebEngine a quitté les
modules pour les « Extensions » de l'installateur depuis Qt 6.8 (voir section F). Que le `.pri`
de qmake accompagne encore ce paquet-là est une supposition, pas un constat. `unix-common.sh`
teste déjà exactement ce fichier pour écarter les Qt sans QtWebEngine : la vérification est donc
un `aqt install-qt`, puis le script du dépôt lancé sur ce qu'il a posé. Tout le reste du document
suppose la réponse oui ; si elle est non, la question n'est plus « qmake ou CMake un jour » mais
« CMake maintenant ».

Car c'est de toute façon rester sur un chemin que Qt promet de retirer depuis Qt 6.0, et si le
projet doit passer à CMake un jour, c'est **cette** mise à jour qui est le bon moment, pas la
suivante.

### Jusqu'où va le C++ sous Qt 6.12

- **Le minimum est C++17.** « Qt 6 officially requires C++17 », dit encore la page *C++20
  Overview* de la documentation de 6.11, et **rien n'a été publié pour 6.12** : ni le wiki de la
  version, ni l'annonce de la bêta 1, ni la page des plates-formes prises en charge ne parlent de
  C++20 comme minimum. Le gel des fonctionnalités a eu lieu le 2 juin 2026 et la sortie est prévue
  le 22 septembre. Le passage du minimum à C++20 est discuté sur la liste de développement depuis
  2023, avec « 6.12+ » cité comme cible envisagée ; **c'est une intention, pas une décision
  publiée**, et il ne faut pas l'écrire autrement. À revérifier à la sortie ;
- **QtWebEngine demande un compilateur C++20 quoi qu'il arrive**, à cause de Chromium, et ses
  notes de plate-forme le disent explicitement : MSVC ≥ 14.36 avec Visual Studio 2022, GCC ou
  Clang ≥ 10. Le projet utilise déjà des Build Tools 2022 et le SDK 10.0.26100, donc la contrainte
  est satisfaite d'avance et l'incertitude du point précédent est sans conséquence pratique ici :
  le compilateur suivra dans les deux cas ;
- **le plafond, côté qmake, est `CONFIG += c++20`.** Le manuel de qmake ne documente que `c++11`,
  `c++14`, `c++17` (`c++1z` en alias obsolète), `c++20` (`c++2a` en alias obsolète) et
  `c++latest`. **Il n'y a pas de `c++23`.** `c++latest` donne `/std:c++latest` sous MSVC et « le
  dernier standard connu de Qt » ailleurs : à éviter ici, il ne veut pas dire la même chose d'un
  compilateur à l'autre, ce qui est exactement ce qu'un projet à trois plates-formes ne veut pas.
  Qt lui-même se configure en C++23 par CMake (`c++2b`), mais c'est la compilation de Qt et pas la
  nôtre ;
- ce que C++20 achèterait côté Qt : `QSpan`, `operator<=>` et l'intégration `std::chrono` ne sont
  exposés qu'en mode C++20. **Rien dont RespawnIRC ait besoin** — c'est une porte, pas un gain.

En pratique : écrire `c++17` dans les deux `.pro`. C'est le minimum documenté, et c'est aussi le
**défaut de qmake sous Qt 6** — la ligne pourrait même disparaître, mais autant la garder
explicite, elle documente ce que le code exige. `CONFIG += strict_c++` reste bon tel quel. Passer
à `c++20` le jour où 6.12 l'exigera coûtera un mot dans chaque `.pro` : ce n'est pas une décision
à prendre d'avance.

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
depuis septembre 2023 s'en vont tous ensemble. C'est le gain le plus net de l'opération. Le
mécanisme est bien celui-là : « Qt will attempt to load the backends in order (with OpenSSL
attempted first) until one is successfully loaded », et Schannel est le greffon natif de Windows.

Une réserve à lever plutôt qu'à supposer, et elle est propre à ce projet : le programme parle à
jeuxvideo.com en **HTTP/2**, ce qui « apaise Cloudflare » (voir `CLAUDE.md`), et HTTP/2 se négocie
par ALPN. Tous les greffons TLS de Qt n'implémentent pas tout — la documentation prend justement
l'ALPN pour exemple de ce qu'un greffon peut laisser de côté. Celui de Schannel annonce bien
`QSsl::ClientSideAlpn` dans son `supportedFeatures()`, donc rien n'indique de régression ; mais
c'est lu dans le code de Qt, et ça se constate sur une vraie requête à JVC, pas dans une liste de
fonctionnalités. À mettre en tête des essais, avant la moindre mesure de taille d'archive : c'est
le seul point de cette section qui puisse rendre le programme inutilisable.

**La pile de l'Universal CRT a déjà disparu**, avec l'abandon de Windows 7, et indépendamment
de Qt : ce n'est plus un gain à porter au crédit de la migration. Voir la section D.

**QtMultimedia devient un sujet.** Aujourd'hui il coûte 0,95 Mo sur 158, mesuré dans
l'archive : `Qt5Multimedia.dll` 0,71 Mo, `audio/qtaudio_wasapi.dll` et
`audio/qtaudio_windows.dll` 0,15 Mo, `mediaservice/qtmedia_audioengine.dll` 0,06 Mo,
`playlistformats/qtmultimedia_m3u.dll` 0,03 Mo. Sous Qt 6.5 et suivants le moteur par défaut
est FFmpeg, et `windeployqt` copie `avcodec`, `avformat`, `avutil`, `swresample` et
`swscale` : des dizaines de mégaoctets pour deux bips.

Ils sont très probablement supprimables après `windeployqt`, comme les traductions le sont déjà —
mais **pas pour la raison qui était écrite ici**. On lisait que la documentation promettait un
repli sur le moteur natif du système si ces bibliothèques manquaient à l'exécution : elle ne le
promet nulle part. Elle dit qu'on peut **choisir** un autre moteur par `QT_MEDIA_BACKEND`, et que
« applications must either bundle FFmpeg binaries in their installer or depend on FFmpeg being
installed on the operating system ». Ce qui arrive quand elles manquent n'est pas documenté, et
supposer un repli était le genre de raisonnement que ce dépôt refuse ailleurs.

La vraie raison est meilleure, et elle rend la question presque sans objet : **`QSoundEffect` ne
passe par aucun moteur.** « The implementation of core APIs, including QMediaDevices,
QAudioDevice, QSoundEffect, QAudioSink, and QAudioSource are integrated into the main Qt
Multimedia library, eliminating the need for additional libraries », dit la page de QtMultimedia.
Or `QSoundEffect` est tout ce dont le programme se sert — deux `QSound::play` aujourd'hui, rien
d'autre de multimédia dans les 45 sources. On n'attend donc pas un repli, on n'utilise simplement
pas la partie qui a besoin de FFmpeg. **À vérifier plutôt qu'à supposer** quand même : archive
faite, sur une machine sans FFmpeg installé, il faut écouter les deux sons. C'est cet essai-là qui
tranche, pas la citation.

Le reste ne bouge pas dans son principe : `--no-compiler-runtime` reste nécessaire pour la
même raison, l'allègement des traductions et la fusion des deux `resources/` aussi, avec des
noms de fichiers renumérotés (`Qt6WebEngineCore.dll` et compagnie). La répartition des 159 Mo
documentée dans `CLAUDE.md` sera à refaire de zéro. À noter que les retraits d'`opengl32sw.dll` et
de `D3Dcompiler_47.dll` seront à revérifier plutôt qu'à reconduire : Qt 6 a abandonné ANGLE, et le
chemin de rendu par défaut sous Windows n'est plus le même.

La copie des bibliothèques d'exécution de MSVC, elle, n'a rien à changer : `dist-windows.ps1` copie
tout le dossier `Microsoft.VC*.CRT` sans nommer de fichier, ce qui vaut pour n'importe quelle version
des outils. **Garder cette forme.** Réduire la copie aux DLL que les binaires importent est ce qui a
déjà livré une archive qui ne démarrait pas, `msvcp140_1.dll` manquant à un `Qt5Core.dll` qui la
réclame ; rien ne dit quelles DLL de la famille les binaires de Qt 6 réclameront. La vérification au
`dumpbin` en fin de script continuera de répondre à la question sans qu'on ait à la deviner.

---

## D. Ce que ça coûte

| | Qt 5.15.2 | Qt 6.11 |
| --- | --- | --- |
| Windows minimum | Windows 7 | **Windows 10 1809** |
| macOS minimum | 10.13 | **macOS 13**, Xcode 15 et SDK macOS 14 |
| Compilateur Windows | MSVC 2019 | MSVC 2022, SDK Windows 11 ≥ 10.0.26100 |
| QtWebEngine avec MinGW | non | **toujours non** |

Les minimums de 6.12 ne sont pas encore publiés ; ceux de 6.11 sont donnés ici comme
approximation, seule la fin de la prise en charge de Windows 10 après 6.12 étant annoncée. Les
trois premières lignes sont relues dans la page *Supported Platforms* de 6.11, qui porte
d'ailleurs la phrase en question : « Qt 6.12 will be the last version to support Windows 10. »

**Windows 7 était le principal coût de cette ligne, et il est déjà payé** : la cible du dépôt
est passée à Windows 10 avant toute migration, la compatibilité Windows 7 ayant été raisonnée
d'après la documentation de Microsoft mais jamais vérifiée sur une vraie machine. La ligne
« Windows minimum » du tableau ne coûte donc plus rien à Qt 6 — mais elle ne lui rapporte plus
rien non plus : la pile de DLL de l'Universal CRT et `D3Dcompiler_47.dll` sont sorties de
l'archive à cette occasion, pas à celle-ci. Les bibliothèques C++ de MSVC, elles, restent
nécessaires quoi qu'il arrive, Qt 6 ou non.

**La ligne du compilateur est payée elle aussi**, et il faut le dire pour la même raison :
`bootstrap-windows.ps1` télécharge `https://aka.ms/vs/17/release/vs_BuildTools.exe`, donc les Build
Tools **2022**, et installe `Microsoft.VisualStudio.Component.Windows11SDK.26100`. Les deux exigences
de la troisième ligne sont donc déjà satisfaites sur une machine amorcée par le dépôt. « MSVC 2019 »
dans la colonne Qt 5 n'est pas le compilateur utilisé mais le nom du paquet de Qt 5.15.2, qui n'existe
que sous cette étiquette : ce sont des binaires du jeu d'outils v142, avec lesquels un compilateur
2022 s'édite en liens sans difficulté. Ce que la migration change ici tient donc en deux chaînes de
caractères de `bootstrap-windows.ps1`, `$QtArch = 'win64_msvc2019_64'` et le `msvc2019_64` du chemin
qu'il construit, plus les six lignes de commande du README qui recopient ce chemin.

Ce qui reste à décider ici est plus étroit : Qt 6.11 demande Windows 10 **1809**, là où le dépôt
ne dit aujourd'hui que « Windows 10 ».

---

## E. Ce que ça rapporte

- la disparition d'OpenSSL décrite en C ;
- un Chromium de cette décennie au lieu de celui de 5.15.2, dont le README déconseille
  lui-même de se servir comme navigateur généraliste ;
- **arm64 natif sur les Mac Apple Silicon** : plus de Rosetta 2, et la note du README sur une
  application x86_64 seulement disparaît ;
- **Homebrew fournit QtWebEngine en version 6**, ce que son `qt@5` ne fait plus : le détour par
  `aqtinstall` devient facultatif sous macOS. Deux précisions qui manquaient, et la seconde compte
  plus que la première. C'est une formule **à part**, `qtwebengine`, et non le `qt` tout court —
  6.11.1 au moment où ceci est écrit, avec des bouteilles arm64 (Tahoe, Sequoia, Sonoma) et
  x86_64 (Sonoma). Et surtout : **cet avantage et le gel de la section F s'excluent.** Homebrew
  suit l'amont et passera à 6.13 puis au-delà en quelques mois ; un projet arrêté sur 6.12.x
  retombe sur `aqtinstall` ou sur une formule épinglée dès la première mise à jour. Le compter
  comme un gain durable serait se tromper : c'est un confort de la phase de portage, pendant
  laquelle 6.11 ou 6.12 sera effectivement ce que Homebrew propose ;
- « la dernière version librement téléchargeable » était une contrainte du LTS Qt 5. Les
  versions intermédiaires de Qt 6, dont 6.11, sont libres d'accès ;
- **un plancher macOS plus haut rouvre le choix du format de l'image disque**, et c'est le
  seul endroit où le coût de la ligne « macOS minimum » du tableau ci-dessus rapporte quelque
  chose. `dist-macos.sh` fabrique le DMG en ULFO (lzfse), qui demande macOS 10.11 et tient
  sous le plancher de 10.13 que Qt 5.15.2 impose. ULMO (lzma) ferait une image d'un cinquième
  plus petite mais demande 10.15, donc au-dessus de ce que l'application annonce
  aujourd'hui — et le passage à Qt 6, qui monte le plancher à macOS 13, lève cette objection
  d'un coup. Elle est la seule à disparaître : ULMO coûte toujours près de deux minutes de
  compression contre une quinzaine de secondes pour ULFO, et c'est ce qu'il faudra peser à ce
  moment-là. Les chiffres mesurés en juillet 2026 sont dans le commentaire de `dist-macos.sh`
  et dans la piste 16 de `POSSIBLE-BUILD-SIMPLIFICATIONS.md`. À vérifier plutôt qu'à
  supposer, le jour venu : `man hdiutil` ne connaît aujourd'hui aucun format plus récent
  qu'ULMO, mais c'est exactement le genre de liste qui bouge avec le système, et le plancher
  de Qt 6.12 n'est de toute façon pas encore publié.

---

## F. Licence, et ce que « rester sur 6.12 » veut dire

**Qt 6 n'est pas commercial.** Double licence LGPLv3 / commerciale, exactement comme Qt 5, et
aucune version de Qt 6 n'est hors de portée du projet. Ce qui est restreint, ce sont les
**branches LTS** : leurs premiers correctifs sont publiés pour tout le monde, puis les suivants
partent dans un dépôt réservé aux licences commerciales. C'est exactement le mécanisme qui a fait
de 5.15.2 la dernière Qt 5 librement téléchargeable — 5.15 était une LTS. Le README décrit le
symptôme sans le nommer.

**Attention à la durée**, parce que la première rédaction de cette page se trompait de rythme et
que l'erreur est très facile à refaire : elle disait « jusqu'à la sortie de la version **majeure**
suivante », ce qui donnerait à 6.12 une vie libre jusqu'à Qt 7. C'est faux, et c'était d'ailleurs
contredit par le calendrier de la même section, qui lui donnait la bonne date sans s'apercevoir de
la contradiction — c'est ce qui rendait l'erreur difficile à voir. La règle de Qt est la
version **mineure**
suivante : les utilisateurs libres reçoivent les correctifs d'une LTS jusqu'à ce que la mineure
d'après sorte. Le cas de 5.15 n'était pas une exception à cette règle mais sa forme dégénérée —
il n'y avait pas de 5.16 à attendre, la mineure suivante était Qt 6.0. Retenir « majeure » ferait
croire à des années de correctifs libres là où il y en a environ six mois, et c'est toute la
différence entre le gel décrit ici et un projet qui découvrirait la coupure sans l'avoir prévue.

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
- depuis Qt 6.8, WebEngine et QtPdf ont quitté les modules pour la zone « Extensions » de
  l'installateur, et les vieilles versions d'`aqtinstall` ne savent pas les y chercher — le
  symptôme est qu'ils **n'apparaissent pas** dans la liste des modules, donc qu'ils ne sont pas
  installables, et non un message d'erreur. Les versions récentes s'en chargent avec le
  `--modules` ordinaire ; le `--extensions` d'autrefois ne doit **pas** être ressorti. À vérifier
  avec `aqt list-qt windows desktop --modules 6.12 win64_msvc2022_64` avant d'écrire quoi que ce
  soit dans le README, plutôt que de recopier la commande de Qt 5 — et avec un `aqt` à jour, ce
  qui fait partie de la vérification et non de ses préalables.

### Le calendrier du gel, et ce qui est gelé au juste

**Ce qu'on gèle est la série 6.12.x, pas une 6.12.0.** La différence n'est pas de vocabulaire :
tant que la branche est ouverte aux utilisateurs libres, chaque correctif publié est pris, et le
projet suit 6.12.1, 6.12.2 et la suite comme il suivrait n'importe quelle mise à jour. Le gel
n'est pas une date à laquelle on cesse de bouger, c'est un plafond : on ne monte pas en 6.13. La
version figée pour de bon est simplement **la dernière 6.12.x librement publiée**, et on ne saura
laquelle qu'après coup.

6.12.0 sort le 22 septembre 2026 ; la branche reste ouverte jusqu'à la sortie de 6.13, soit vers
mars-avril 2027 si l'on en juge par la fin de prise en charge de 6.11 le 17 mars 2027. C'est là
qu'on s'arrête, sur ce que la branche aura donné entre-temps.

Deux conséquences pratiques :

- **rien à décider en septembre.** Prendre 6.12.0 dès sa sortie et la remplacer par chaque
  correctif est la conduite normale, et c'est aussi ce qui donne le plus d'essais avant la
  coupure. Le seul rendez-vous est celui de mars-avril 2027, où il faut penser à récupérer la
  dernière avant qu'elle ne devienne commerciale ;
- **fixer le numéro exact quelque part dans le dépôt** le jour de l'arrêt, et non « la dernière
  6.12 ». `bootstrap-windows.ps1` a déjà un `$QtVersion` pour ça, et le README des chemins qui
  portent le numéro. Un gel qui ne s'écrit nulle part n'est pas un gel.

Et puisque « pour toujours » veut dire que la compilation doit encore fonctionner dans cinq
ans : **archiver le paquet Qt exact**, quelque part de durable. Les serveurs de Qt élaguent
les vieilles versions, et une chaîne de compilation qui dépend d'un téléchargement finit par
dépendre d'un 404. Cet archivage n'a de sens qu'une fois, sur la version d'arrêt — inutile de
garder les 6.12.x intermédiaires.

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

Ce que rien de distribué ne contient, c'est **le moindre texte de licence** : ni la LGPLv3 de Qt,
ni les mentions des bibliothèques tierces, **ni celle du programme lui-même**. Ce dernier point
manquait à la première rédaction de cette section, qui se contentait de noter que « la licence
zlib du programme n'entre en conflit avec rien » — c'est vrai, mais à côté du sujet : le dépôt
**a** un fichier `LICENSE` à sa racine, la licence zlib au nom de FranckRJ, et **il ne part avec
aucune des deux distributions**. `dist-macos.sh` n'extrait de `git archive HEAD` que `resources` et
`themes`, `dist-windows.ps1` fait de même, et ni l'un ni l'autre ne nomme `LICENSE` nulle part.

Les deux moitiés du trou ne sont pas de même nature, et les confondre ferait mal juger l'urgence :

- **la LGPLv3 de Qt est une obligation.** Livrer des bibliothèques sous cette licence demande de
  joindre le texte, d'offrir les sources correspondantes et de permettre la réédition de liens —
  cette dernière étant déjà satisfaite par l'édition dynamique, DLL à côté de l'exécutable et
  frameworks dans le bundle ;
- **le `LICENSE` du dépôt, non.** La licence zlib ne réclame le maintien de sa notice que sur les
  distributions **de sources** (« must not be removed or altered from any source distribution »),
  et sa clause d'attribution dit elle-même qu'un remerciement « is appreciated but is not
  required ». Une archive binaire qui ne l'emporte pas n'est donc en faute de rien. C'est de
  l'hygiène, pas de la conformité — mais c'est aussi le plus facile des deux à réparer, le fichier
  étant déjà commité et les deux scripts sachant déjà extraire de `HEAD`.

C'est vrai de l'archive Qt 5 d'aujourd'hui : ce n'est donc pas un argument contre la mise à
jour, mais c'est la seule chose de cette page qui soit un manquement plutôt qu'un arbitrage,
et une migration est le bon moment pour ajouter un dossier `licenses/` aux deux scripts de
distribution — en y mettant le `LICENSE` du dépôt au passage, et sans attendre la migration pour
celui-là.

Le compte de « 464 entrées » qui ouvrait cette section a été retiré : il datait d'avant le passage
à Windows 10, qui a retiré 48 fichiers de l'archive, et il ne s'accordait plus avec les 426
fichiers relevés depuis dans `CLAUDE.md`. Le point ne dépendait de toute façon pas du nombre.

---

## Ordre suggéré

**Le portage est repoussé, et cette section décrit ce qu'il faudrait faire, pas ce qui est
engagé.** Décision du mainteneur, prise le 30 juillet 2026 : le projet reste sur Qt 5.15.2 et
OpenSSL 1.1.1 pour l'instant. Aucune **conclusion** de ce document n'est démentie — la relecture
du 31 juillet a corrigé des numéros de ligne, complété la table A et remplacé deux justifications
fausses par les bonnes, sans qu'aucun arbitrage ne change de sens. L'ordre de travail ci-dessous
est toujours le bon, et l'exposition d'une bibliothèque TLS sans maintenance
depuis septembre 2023 est connue et assumée, non pas ignorée. Ne pas entamer le portage sans le
mainteneur : la lecture naturelle des paragraphes qui suivent est qu'il n'y a aucune raison
d'attendre, et c'est précisément ce que cet avertissement corrige.

La version visée sort en septembre, mais il n'y a aucune raison d'attendre : le portage est
le même pour toute version de Qt 6, et 6.11 est disponible aujourd'hui. Le faire maintenant
transforme septembre en simple changement de numéro de version.

**Maintenant, sur 6.11 :**

0. **Installer le Qt et regarder si `mkspecs/modules/qt_lib_webenginewidgets.pri` est là.**
   Cinq minutes, avant d'ouvrir le moindre `.cpp` : c'est la seule question dont une mauvaise
   réponse invaliderait le plan entier plutôt qu'une de ses étapes (voir section B).
1. **`tests/` d'abord.** La cible ne dépend que de `core gui network` et couvre
   `parsingTool`, `payloadTool`, `styleTool` et `shortcutTool`. Voir les 142 vérifications
   passer sous Qt 6 valide le travail sur les regex avant qu'aucune interface ne soit en
   jeu. Elle a en plus le seul `QTextStream::setCodec` du dépôt, donc elle ne compilera pas
   du premier coup.
2. **Le programme** : les substitutions de la table A, puis `QSoundEffect` à part.
3. **Une page de JVC réellement chargée**, avant de toucher aux scripts : c'est là qu'on
   apprend si le TLS de Schannel passe Cloudflare en HTTP/2 (section C).
4. **`dist-windows.ps1` et `dist-macos.sh`**, puis remesurer l'archive et reprendre le
   passage de `CLAUDE.md` sur la répartition des 159 Mo.

**À partir de septembre 2026 :** passer sur la série 6.12 dès sa sortie, ce qui ne devrait
demander qu'un changement de chemin, vérifier qu'aucune dépréciation n'est apparue entre les
deux versions, puis suivre les 6.12.x au fur et à mesure.

**Vers mars-avril 2027, à la sortie de 6.13 :** prendre la dernière 6.12.x librement
publiée, l'archiver, écrire son numéro dans le dépôt, et s'y arrêter.

Le point de vigilance, s'il ne devait y en avoir qu'un, ce sont les regex du correcteur
orthographique.
