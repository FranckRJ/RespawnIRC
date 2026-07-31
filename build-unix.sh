#!/bin/bash
# Compile RespawnIRC sous macOS et sous Linux, et ses tests avec -t. C'est le pendant de
# build-windows.ps1 : dist-macos.sh l'appelle plutôt que de garder sa propre copie des mêmes étapes,
# et la compilation n'existe donc qu'à un seul endroit par famille de systèmes.
#
# Usage : ./build-unix.sh [-q chemin/vers/Qt/5.15.2/clang_64] [-t] [-c]
#   -q, --qt-dir   le Qt à utiliser ; à défaut, celui dont le qmake est dans le PATH
#   -t, --tests    compile aussi les tests et les lance
#   -c, --clean    efface les dossiers de compilation au lieu de reprendre leurs objets
#
# Un seul script pour les deux systèmes, et non un par plateforme : ils ne diffèrent que par
# l'option qmake du SDK, la façon de compter les processeurs et le nom de ce qui sort — un bundle
# d'un côté, un exécutable de l'autre. Les .pro, eux, ne distinguent rien.
#
# La compilation se fait hors des sources, dans build/respawnIrc et build/tests, comme sous Windows ;
# seuls les objets intermédiaires y restent, le DESTDIR des .pro déposant le programme à la racine du
# dépôt et respawnIrcTests dans build/.

set -e

repoDir="$(cd "$(dirname "$0")" && pwd)"

qtDir=""
withTests="false"
withClean="false"

while [ $# -gt 0 ]
do
    case "$1" in
        -q|--qt-dir)
            if [ -z "$2" ]
            then
                echo "$1 attend un chemin." >&2
                exit 1
            fi
            qtDir="$2"
            shift 2
            ;;
        -t|--tests)
            withTests="true"
            shift
            ;;
        -c|--clean)
            withClean="true"
            shift
            ;;
        -h|--help)
            sed -n '2,17p' "$0"
            exit 0
            ;;
        *)
            echo "Option inconnue : $1" >&2
            exit 1
            ;;
    esac
done

if [ -z "$qtDir" ]
then
    if command -v qmake > /dev/null
    then
        qtDir="$(dirname "$(dirname "$(command -v qmake)")")"
    else
        echo "Qt introuvable : passez son chemin avec -q, ou mettez son qmake dans le PATH." >&2
        exit 1
    fi
fi

qmakeBin="$qtDir/bin/qmake"

if [ ! -x "$qmakeBin" ]
then
    echo "$qmakeBin est introuvable ou non exécutable." >&2
    exit 1
fi

# Un Qt trouvé n'est pas un Qt utilisable : celui de Homebrew et un Debian sans qtwebengine5-dev ont
# tout ce qu'il faut sauf le module dont dépend tout l'affichage des messages. Sans ce contrôle, ça ne
# se voit qu'au « Project ERROR: Unknown module(s) in QT: webenginewidgets » de qmake, qui ne dit ni
# quel Qt a été pris, ni pourquoi celui-là n'ira jamais, ni lequel prendre. Le cas n'a rien de
# théorique sous macOS : le qmake du PATH y est celui de Homebrew dès qu'il est installé, donc le
# comportement par défaut du script tombe droit dessus. C'est qmake qu'on interroge et non un chemin
# qu'on devine, les mkspecs n'étant pas sous $qtDir sur un Debian.
archDataDir="$("$qmakeBin" -query QT_INSTALL_ARCHDATA)"

if [ ! -f "$archDataDir/mkspecs/modules/qt_lib_webenginewidgets.pri" ]
then
    echo "Ce Qt n'a pas QtWebEngine, dont RespawnIRC a besoin : $qtDir" >&2

    if [ "$(uname -s)" = "Darwin" ]
    then
        echo "Le qt@5 de Homebrew en est dépourvu, son Chromium ayant des failles non corrigées." >&2
        echo "Installez le Qt 5.15.2 officiel avec aqtinstall (voir le README), puis désignez-le :" >&2
        # Les deux lignes, et non celle de ce script-ci : dist-macos.sh passe par ici, et conseiller
        # alors build-unix.sh ferait taper autre chose que ce qu'on voulait faire.
        echo "    ./build-unix.sh -q ~/Qt/5.15.2/clang_64" >&2
        echo "    ./dist-macos.sh ~/Qt/5.15.2/clang_64" >&2
    else
        echo "Installez le paquet qtwebengine5-dev (voir le README pour les autres distributions)." >&2
    fi

    exit 1
fi

# Tout ce qui distingue les deux systèmes tient ici. CONFIG+=sdk_no_version_check fait taire
# l'avertissement de Qt 5.15.2, qui n'a été testé qu'avec le SDK 10.15 alors que Xcode en fournit un
# bien plus récent. Le programme est un bundle sous macOS parce que le système de fichiers y ignore
# la casse : un exécutable RespawnIRC ne pourrait pas cohabiter avec le dossier de sources
# respawnIrc.
if [ "$(uname -s)" = "Darwin" ]
then
    optionsForProgram=(CONFIG+=sdk_no_version_check)
    builtProgram="$repoDir/RespawnIRC.app"
    cpuCount="$(sysctl -n hw.ncpu)"
else
    optionsForProgram=()
    builtProgram="$repoDir/RespawnIRC"
    cpuCount="$(nproc 2> /dev/null || getconf _NPROCESSORS_ONLN 2> /dev/null || echo 4)"
fi

# Une cible, c'est un dossier de compilation, un .pro, ses options qmake et ce qu'on attend à
# l'arrivée : le programme et les tests ne diffèrent que par ces quatre choses.
buildThisTarget()
{
    thisBuildDir="$1"
    thisProFile="$2"
    thisBuiltFile="$3"
    shift 3

    if [ "$withClean" = "true" ]
    then
        rm -rf "$thisBuildDir"
    fi

    # Ce qui est en place est toujours effacé, et ce n'est pas un détail. Le DESTDIR des .pro ne
    # distingue pas les dossiers de compilation : ce qui est à la racine peut venir d'ailleurs, make
    # le comparerait à ses objets, le trouverait à jour et n'éditerait aucun lien. Sous macOS cette
    # même ligne règle un second piège, celui que dist-macos.sh traitait pour son compte : la règle
    # qmake qui fabrique Info.plist n'a aucune dépendance, si bien qu'un bundle déjà là garde le
    # numéro de version de la compilation précédente. Compiler dans un dossier neuf n'y change rien,
    # la cible de cette règle étant le bundle de la racine.
    rm -rf "$thisBuiltFile"

    mkdir -p "$thisBuildDir"
    (cd "$thisBuildDir" && "$qmakeBin" "$thisProFile" "$@" && make -j"$cpuCount")

    # make rend 0 sans rien faire quand il juge sa cible à jour : c'est justement ce que l'effacement
    # ci-dessus évite, et le vérifier ferme la question au lieu de laisser la suite manipuler un
    # fichier absent.
    if [ ! -e "$thisBuiltFile" ]
    then
        echo "$thisBuiltFile n'a pas été produit alors que make a réussi." >&2
        exit 1
    fi
}

echo "== Compilation de RespawnIRC avec $qtDir"
buildThisTarget "$repoDir/build/respawnIrc" "$repoDir/respawnIrc/respawnIrc.pro" "$builtProgram" \
    "${optionsForProgram[@]}"

if [ "$withTests" = "true" ]
then
    testsBin="$repoDir/build/respawnIrcTests"

    echo "== Compilation des tests"
    buildThisTarget "$repoDir/build/tests" "$repoDir/tests/tests.pro" "$testsBin"

    echo "== Vérifications"
    # Les tests sont liés à Qt comme le programme et ne démarrent pas sans ses bibliothèques. Sous
    # macOS les frameworks portent leur chemin absolu, aucun réglage n'est nécessaire ; sous Linux,
    # un Qt installé ailleurs que dans le système doit être annoncé, et le faire quand il est dans le
    # système ne coûte rien. Le chemin des fixtures, lui, est figé au moment de compiler
    # (FIXTURES_PATH dans tests.pro) : le binaire se lance depuis n'importe où.
    if [ "$(uname -s)" != "Darwin" ]
    then
        export LD_LIBRARY_PATH="$qtDir/lib:$LD_LIBRARY_PATH"
    fi

    "$testsBin"
fi
