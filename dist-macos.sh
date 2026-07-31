#!/bin/bash
# Fabrique une image disque distribuable de RespawnIRC pour macOS : le bundle RespawnIRC.app rendu
# autonome par macdeployqt (Qt et QtWebEngine embarqués), accompagné des dossiers resources/ et
# themes/ que le programme lit à côté du bundle. Ces deux dossiers ne sont jamais écrits, ce que le
# programme écrit allant dans ~/Library/Application Support et ~/Library/Caches : le bundle pourrait
# donc les embarquer dans Contents/Resources et l'image se réduire à un simple RespawnIRC.app, mais
# pathTool::dataDirPath() les cherche toujours à côté de lui.
#
# Usage : ./dist-macos.sh [chemin/vers/Qt/5.15.2/clang_64] [--skip-tests]
# À défaut d'argument, le Qt utilisé est celui dont le qmake est dans le PATH.
#
# La compilation elle-même est celle de build-unix.sh, à qui le Qt est passé : ce script ne garde pas
# sa propre copie des mêmes étapes. Les tests sont compilés et lancés au passage, l'archive n'ayant
# aucune raison de sortir sans eux ; --skip-tests s'en passe.

set -e

repoDir="$(cd "$(dirname "$0")" && pwd)"
distDir="$repoDir/dist"

qtDir=""
withTests="true"

while [ $# -gt 0 ]
do
    case "$1" in
        --skip-tests)
            withTests="false"
            ;;
        *)
            qtDir="$1"
            ;;
    esac

    shift
done

# Cette résolution est aussi dans build-unix.sh, et elle y reste : ce script en a besoin pour son
# compte, macdeployqt venant du même Qt que qmake. La descendre dans un fichier commun ferait un
# troisième fichier pour six lignes et deux appelants — c'est quand un troisième utilisateur est
# apparu que les scripts Windows ont eu leur windows-common.ps1.
if [ -z "$qtDir" ]
then
    if command -v qmake > /dev/null
    then
        qtDir="$(dirname "$(dirname "$(command -v qmake)")")"
    else
        echo "Qt introuvable : passez le chemin de Qt en argument, ou mettez son qmake dans le PATH." >&2
        exit 1
    fi
fi

# qmake n'est pas vérifié ici : build-unix.sh le fait pour son compte, et dès sa première ligne utile.
macdeployqtBin="$qtDir/bin/macdeployqt"

if [ ! -x "$macdeployqtBin" ]
then
    echo "$macdeployqtBin est introuvable ou non exécutable." >&2
    exit 1
fi

# version.pri est la seule source du numéro de version, et le .pro le pousse de là dans le programme
# comme dans Info.plist : le DMG, le bundle et le binaire ne peuvent pas annoncer trois numéros.
version="$(sed -n 's/^ *RESPAWNIRC_VERSION *= *\([0-9.]*\) *$/\1/p' "$repoDir/version.pri")"

if [ -z "$version" ]
then
    echo "Numéro de version introuvable dans version.pri." >&2
    exit 1
fi

# Le DESTDIR de respawnIrc.pro produit le bundle à la racine du dépôt, pas dans le dossier de
# compilation.
bundlePath="$repoDir/RespawnIRC.app"

echo "== Version distribuable $version"
optionsForBuild=(-q "$qtDir")

if [ "$withTests" = "false" ]
then
    echo "-- tests sautés"
else
    optionsForBuild+=(-t)
fi

# L'effacement du bundle déjà en place, que ce script faisait lui-même à cause de la règle sans
# dépendance qui fabrique Info.plist, est maintenant dans build-unix.sh : il y couvre le même piège
# pour qui compile sans passer par ici.
"$repoDir/build-unix.sh" "${optionsForBuild[@]}"

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
