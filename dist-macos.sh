#!/bin/bash
# Fabrique une image disque distribuable de RespawnIRC pour macOS : le bundle RespawnIRC.app rendu
# autonome par macdeployqt (Qt et QtWebEngine embarqués), accompagné des dossiers resources/ et
# themes/ que le programme lit à côté du bundle. Ces deux dossiers ne sont jamais écrits, ce que le
# programme écrit allant dans ~/Library/Application Support et ~/Library/Caches : le bundle pourrait
# donc les embarquer dans Contents/Resources et l'image se réduire à un simple RespawnIRC.app, mais
# pathTool::dataDirPath() les cherche toujours à côté de lui.
#
# Usage : ./dist-macos.sh [chemin/vers/Qt/5.15.2/clang_64]
# À défaut d'argument, le Qt utilisé est celui dont le qmake est dans le PATH.

set -e

repoDir="$(cd "$(dirname "$0")" && pwd)"
distDir="$repoDir/dist"

if [ -n "$1" ]
then
    qtDir="$1"
elif command -v qmake > /dev/null
then
    qtDir="$(dirname "$(dirname "$(command -v qmake)")")"
else
    echo "Qt introuvable : passez le chemin de Qt en argument, ou mettez son qmake dans le PATH." >&2
    exit 1
fi

qmakeBin="$qtDir/bin/qmake"
macdeployqtBin="$qtDir/bin/macdeployqt"

for thisBin in "$qmakeBin" "$macdeployqtBin"
do
    if [ ! -x "$thisBin" ]
    then
        echo "$thisBin est introuvable ou non exécutable." >&2
        exit 1
    fi
done

# version.pri est la seule source du numéro de version, et le .pro le pousse de là dans le programme
# comme dans Info.plist : le DMG, le bundle et le binaire ne peuvent pas annoncer trois numéros.
version="$(sed -n 's/^ *RESPAWNIRC_VERSION *= *\([0-9.]*\) *$/\1/p' "$repoDir/version.pri")"

if [ -z "$version" ]
then
    echo "Numéro de version introuvable dans version.pri." >&2
    exit 1
fi

# Le DESTDIR de respawnIrc.pro produit le bundle à la racine du dépôt, pas dans les sources.
bundlePath="$repoDir/RespawnIRC.app"

echo "== Compilation de RespawnIRC $version avec $qtDir"
cd "$repoDir/respawnIrc"
# La règle de qmake qui fabrique Info.plist n'a aucune dépendance : make la saute dès qu'un bundle
# est déjà là, et une version distribuable hériterait des informations du bundle précédent.
rm -rf "$bundlePath"
"$qmakeBin" CONFIG+=sdk_no_version_check
make -j"$(sysctl -n hw.ncpu)"

echo "== Embarquement de Qt dans le bundle"
# macdeployqt copie les frameworks Qt, les greffons et le processus QtWebEngine dans le bundle, et
# réécrit les chemins qui pointaient vers le Qt de la machine de compilation.
"$macdeployqtBin" "$bundlePath"

echo "== Signature ad hoc"
# Sans signature, macOS refuse de lancer un bundle dont macdeployqt a réécrit les binaires. Cette
# signature ad hoc ne vaut pas notarisation : au premier lancement il faudra passer par le menu
# contextuel « Ouvrir », ou retirer la mise en quarantaine (voir le README).
codesign --force --deep --sign - "$bundlePath"

echo "== Assemblage de l'image disque"
# L'image contient un unique dossier RespawnIRC, à glisser tel quel dans les Applications :
# l'application et ses données doivent rester ensemble.
rm -rf "$distDir"
mkdir -p "$distDir/image/RespawnIRC"
mv "$bundlePath" "$distDir/image/RespawnIRC/"
# resources/ et themes/ sont extraits de git et non copiés depuis le dossier de travail : celui-ci
# contient aussi ce que le mainteneur a accumulé en se servant du programme, à commencer par les
# stickers qu'une version antérieure téléchargeait dans resources/stickers/ et que rien ne distingue
# de ceux livrés. git archive ne sort que ce qui est commité, sans liste d'exclusion à tenir à jour.
# Ce que le programme écrit aujourd'hui vit dans userdata/, qui n'est simplement jamais copié.
git -C "$repoDir" archive HEAD resources themes | tar -x -C "$distDir/image/RespawnIRC"

dmgPath="$distDir/RespawnIRC-$version-macos.dmg"
hdiutil create -volname "RespawnIRC $version" -srcfolder "$distDir/image" -ov -format UDZO -quiet "$dmgPath"
rm -rf "$distDir/image"

echo "== Terminé : $dmgPath"
