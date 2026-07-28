#!/bin/bash
# Fabrique une image disque distribuable de RespawnIRC pour macOS : le bundle RespawnIRC.app rendu
# autonome par macdeployqt (Qt et QtWebEngine embarqués), accompagné des dossiers resources/ et
# themes/ que le programme lit et écrit à côté de lui, comme sous Windows et Linux. Le programme
# téléchargeant les stickers dans resources/, ces dossiers ne peuvent pas être enfermés en lecture
# seule dans le bundle : l'application et ses données restent côte à côte dans un même dossier.
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

version="$(sed -n 's/.*currentVersionName("v\([0-9.]*\)").*/\1/p' "$repoDir/respawnIrc/respawnIrc.cpp")"

if [ -z "$version" ]
then
    echo "Numéro de version introuvable dans respawnIrc.cpp." >&2
    exit 1
fi

echo "== Compilation de RespawnIRC $version avec $qtDir"
cd "$repoDir/respawnIrc"
# La règle de qmake qui fabrique Info.plist n'a aucune dépendance : make la saute dès qu'un bundle
# est déjà là, et une version distribuable hériterait des informations du bundle précédent.
rm -rf RespawnIRC.app
"$qmakeBin" CONFIG+=sdk_no_version_check
make -j"$(sysctl -n hw.ncpu)"

echo "== Embarquement de Qt dans le bundle"
# macdeployqt copie les frameworks Qt, les greffons et le processus QtWebEngine dans le bundle, et
# réécrit les chemins qui pointaient vers le Qt de la machine de compilation.
"$macdeployqtBin" RespawnIRC.app

echo "== Signature ad hoc"
# Sans signature, macOS refuse de lancer un bundle dont macdeployqt a réécrit les binaires. Cette
# signature ad hoc ne vaut pas notarisation : au premier lancement il faudra passer par le menu
# contextuel « Ouvrir », ou retirer la mise en quarantaine (voir le README).
codesign --force --deep --sign - RespawnIRC.app

echo "== Assemblage de l'image disque"
# L'image contient un unique dossier RespawnIRC, à glisser tel quel dans les Applications :
# l'application et ses données doivent rester ensemble.
rm -rf "$distDir"
mkdir -p "$distDir/image/RespawnIRC"
mv RespawnIRC.app "$distDir/image/RespawnIRC/"
cp -R "$repoDir/resources" "$repoDir/themes" "$distDir/image/RespawnIRC/"

dmgPath="$distDir/RespawnIRC-$version-macos.dmg"
hdiutil create -volname "RespawnIRC $version" -srcfolder "$distDir/image" -ov -format UDZO -quiet "$dmgPath"
rm -rf "$distDir/image"

echo "== Terminé : $dmgPath"
