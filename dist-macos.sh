#!/bin/bash
# Fabrique une image disque distribuable de RespawnIRC pour macOS : un bundle RespawnIRC.app
# autonome, avec Qt et QtWebEngine embarqués par macdeployqt, accompagné d'un lien vers
# /Applications pour l'installer d'un glisser-déposer. resources/ et themes/ sont dans
# Contents/Resources, où le .pro les met déjà à la compilation et où pathTool::dataDirPath() les
# cherche : ce script ne fait que les remplacer par leur version commitée. Ces deux dossiers ne sont
# jamais écrits — ce que le programme écrit va dans ~/Library/Application Support et
# ~/Library/Caches — c'est ce qui permet de les enfermer dans le bundle.
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

# Résolution de Qt, partagée avec build-unix.sh. Il est essentiel que les deux tombent sur le même :
# macdeployqt vient d'ici, et il remplit le bundle des frameworks de son propre Qt.
. "$repoDir/unix-common.sh"

if ! resolveQtDir "$qtDir"
then
    exit 1
fi

# Le Qt résolu est passé explicitement plus bas, et non laissé à la résolution de build-unix.sh : les
# deux donneraient le même résultat, mais le compter sur une coïncidence est exactement ce qu'il ne
# faut pas faire ici.
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

# Le DESTDIR de respawnIrc.pro produit le bundle dans build/, à côté de respawnIrcTests, et non dans
# le dossier des objets qu'est build/respawnIrc.
bundlePath="$repoDir/build/RespawnIRC.app"

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
# pour qui compile sans passer par ici. Le bundle sort de la compilation dans build/ ; c'est le mv
# plus bas qui l'en retire, l'image disque n'ayant pas à contenir un chemin de compilation.
"$repoDir/build-unix.sh" "${optionsForBuild[@]}"

echo "== Embarquement de Qt dans le bundle"
# macdeployqt copie les frameworks Qt, les greffons et le processus QtWebEngine dans le bundle, et
# réécrit les chemins qui pointaient vers le Qt de la machine de compilation.
"$macdeployqtBin" "$bundlePath"

echo "== Données livrées reprises de git"
# La compilation a déjà mis resources/ et themes/ dans le bundle, mais tels qu'ils sont dans le
# dossier de travail : celui-ci contient aussi ce que le mainteneur a accumulé en se servant du
# programme, à commencer par les stickers qu'une version antérieure téléchargeait dans
# resources/stickers/ et que rien ne distingue de ceux livrés. On les remplace donc par ce que git
# archive sort de HEAD, c'est-à-dire uniquement ce qui est commité, sans liste d'exclusion à tenir à
# jour. Ce que le programme écrit aujourd'hui vit dans les dossiers du système, jamais copiés.
#
# Ce remplacement doit précéder la signature, qui scelle le contenu du bundle et ne survivrait pas à
# un fichier changé après elle.
rm -rf "$bundlePath/Contents/Resources/resources" "$bundlePath/Contents/Resources/themes"
git -C "$repoDir" archive HEAD resources themes | tar -x -C "$bundlePath/Contents/Resources"

echo "== Signature ad hoc"
# Sans signature, macOS refuse de lancer un bundle dont macdeployqt a réécrit les binaires. Cette
# signature ad hoc ne vaut pas notarisation : au premier lancement il faudra passer par le menu
# contextuel « Ouvrir », ou retirer la mise en quarantaine (voir le README).
codesign --force --deep --sign - "$bundlePath"

echo "== Assemblage de l'image disque"
# L'image contient le seul RespawnIRC.app et un lien vers /Applications, la disposition attendue
# sous macOS : on ouvre l'image et on glisse l'application sur le lien. hdiutil recopie ce lien
# symbolique tel quel, il pointe donc vers les Applications de la machine qui monte l'image.
rm -rf "$distDir"
mkdir -p "$distDir/image"
mv "$bundlePath" "$distDir/image/"
ln -s /Applications "$distDir/image/Applications"

dmgPath="$distDir/RespawnIRC-$version-macos.dmg"
hdiutil create -volname "RespawnIRC $version" -srcfolder "$distDir/image" -ov -format UDZO -quiet "$dmgPath"
rm -rf "$distDir/image"

echo "== Terminé : $dmgPath"
