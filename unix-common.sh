# Résolution du Qt à utiliser, chargée par point-sourcing dans build-unix.sh et dist-macos.sh. C'est
# le pendant de windows-common.ps1.
#
# Elle est ici et pas recopiée dans les deux parce que **les deux doivent tomber sur le même Qt** :
# build-unix.sh compile contre celui qu'il résout, dist-macos.sh prend macdeployqt dans celui qu'il
# résout. S'ils divergeaient, le bundle serait rempli des frameworks d'un autre Qt que celui contre
# lequel il a été compilé — une panne silencieuse à l'exécution, chez l'utilisateur, et pas une erreur
# de compilation. Tant que ce n'était que six lignes on pouvait les recopier ; depuis qu'il faut
# écarter les Qt sans QtWebEngine et chercher ailleurs, la correction ne tient plus que si les deux
# scripts font exactement la même chose.

# Vrai si le Qt dont on donne le qmake contient QtWebEngine, dont dépend tout l'affichage des
# messages. C'est qmake qu'on interroge et non un chemin qu'on devine : sur un Debian les mkspecs ne
# sont pas sous le dossier de Qt.
qtHasWebEngine()
{
    thisArchDataDir="$("$1" -query QT_INSTALL_ARCHDATA 2> /dev/null)"

    if [ -z "$thisArchDataDir" ]
    then
        return 1
    fi

    [ -f "$thisArchDataDir/mkspecs/modules/qt_lib_webenginewidgets.pri" ]
}

# Les Qt où regarder quand l'appelant n'en a désigné aucun, dans l'ordre. Celui du PATH d'abord, qui
# est le bon sous Linux ; puis, sous macOS, ceux qu'aqtinstall pose là où le README le fait installer
# — parce que le qmake du PATH y est celui de Homebrew dès qu'il est installé, et que ce qt@5 n'a
# justement plus QtWebEngine. Sans ce second endroit, le comportement par défaut des deux scripts
# tombe droit sur le seul Qt de la machine qui ne peut pas convenir.
candidateQtDirs()
{
    candidateQtDirsResult=()

    if command -v qmake > /dev/null
    then
        candidateQtDirsResult+=("$(dirname "$(dirname "$(command -v qmake)")")")
    fi

    if [ "$(uname -s)" = "Darwin" ]
    then
        for thisDir in "$HOME"/Qt/*/clang_64
        do
            if [ -x "$thisDir/bin/qmake" ]
            then
                candidateQtDirsResult+=("$thisDir")
            fi
        done
    fi
}

# Dit ce qui manque et comment l'obtenir, puis rend la main à l'appelant qui s'arrêtera. Le message
# vaut la peine : sans lui, l'absence du module ne se voit qu'au « Project ERROR: Unknown module(s) in
# QT: webenginewidgets » de qmake, qui ne dit ni quel Qt a été pris, ni pourquoi celui-là n'ira
# jamais, ni lequel prendre.
reportQtWithoutWebEngine()
{
    if [ -n "$1" ]
    then
        echo "Ce Qt n'a pas QtWebEngine, dont RespawnIRC a besoin : $1" >&2
    else
        echo "Aucun Qt avec QtWebEngine trouvé, et RespawnIRC en a besoin." >&2
    fi

    if [ "$(uname -s)" = "Darwin" ]
    then
        echo "Le qt@5 de Homebrew en est dépourvu, son Chromium ayant des failles non corrigées." >&2
        echo "Installez le Qt 5.15.2 officiel avec aqtinstall (voir le README) : une fois dans ~/Qt," >&2
        echo "les scripts le trouvent seuls. Sinon, désignez-le :" >&2
        # Les deux commandes, et pas seulement celle du script qui appelle : c'est dist-macos.sh qui
        # passe le plus souvent par ici, et lui conseiller build-unix.sh ferait taper autre chose que
        # ce qu'on voulait faire.
        echo "    ./build-unix.sh -q ~/Qt/5.15.2/clang_64" >&2
        echo "    ./dist-macos.sh ~/Qt/5.15.2/clang_64" >&2
    else
        echo "Installez le paquet qtwebengine5-dev (voir le README pour les autres distributions)." >&2
    fi
}

# Écrit dans $qtDir le Qt à utiliser, celui de l'argument s'il y en a un. Rend 1 sans rien écrire si
# rien ne convient, le message ayant déjà été affiché.
resolveQtDir()
{
    qtDir=""

    # Un Qt désigné à la main n'est jamais remplacé en douce : s'il ne convient pas, on le dit plutôt
    # que d'en compiler un autre dans son dos.
    if [ -n "$1" ]
    then
        if [ ! -x "$1/bin/qmake" ]
        then
            echo "$1/bin/qmake est introuvable ou non exécutable." >&2
            return 1
        fi

        if ! qtHasWebEngine "$1/bin/qmake"
        then
            reportQtWithoutWebEngine "$1"
            return 1
        fi

        qtDir="$1"
        return 0
    fi

    candidateQtDirs

    for thisCandidate in "${candidateQtDirsResult[@]}"
    do
        if qtHasWebEngine "$thisCandidate/bin/qmake"
        then
            qtDir="$thisCandidate"
            return 0
        fi
    done

    if [ "${#candidateQtDirsResult[@]}" -eq 0 ]
    then
        echo "Qt introuvable : passez son chemin, ou mettez son qmake dans le PATH." >&2
    else
        reportQtWithoutWebEngine ""
    fi

    return 1
}
