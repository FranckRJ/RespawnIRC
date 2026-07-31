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

# Ce qui cloche avec le Qt dont on donne le qmake, s'il cloche quelque chose : 0 s'il convient, 1 si
# son qmake ne s'exécute pas, 2 s'il n'a pas QtWebEngine. Le module sert à la fenêtre de connexion et
# au navigateur interne, et non à l'affichage des messages, qui se fait dans un QTextBrowser — mais
# respawnIrc.pro le demande dans son QT, donc un Qt sans lui ne compile pas du tout.
# C'est qmake qu'on interroge et non un chemin qu'on devine : sur un Debian les mkspecs ne sont pas
# sous le dossier de Qt.
#
# Les deux échecs sont distingués parce qu'ils n'ont ni la même cause ni le même remède, et que le
# premier est tout sauf théorique sous macOS : le Qt 5.15.2 officiel n'existe qu'en x86_64, donc sur
# un Mac Apple Silicon son qmake ne démarre pas tant que Rosetta 2 n'est pas installé. Tant que les
# deux se confondaient, cette machine-là s'entendait dire que son Qt n'avait pas QtWebEngine et
# qu'il fallait installer avec aqtinstall celui qui était déjà là. Constaté en donnant à
# resolveQtDir un faux qmake qui rend 86, le code exact d'un « Bad CPU type in executable ».
qtSuitability()
{
    thisArchDataDir="$("$1" -query QT_INSTALL_ARCHDATA 2> /dev/null)"

    if [ -z "$thisArchDataDir" ]
    then
        return 1
    fi

    if [ ! -f "$thisArchDataDir/mkspecs/modules/qt_lib_webenginewidgets.pri" ]
    then
        return 2
    fi

    return 0
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

# Pendant du précédent pour un qmake qui ne s'exécute pas. Le cas connu est celui d'un Mac Apple
# Silicon sans Rosetta 2, et c'est la seule chose que le message affirme : un binaire x86_64 y est
# refusé par le noyau, sans que rien ne le dise autrement qu'un « Bad CPU type in executable » que
# le 2> /dev/null de qtSuitability avale. Sur une machine x86_64, ou si Rosetta est là, la cause est
# ailleurs — un Qt à moitié installé, des droits — et le message ne prétend pas la connaître.
reportQmakeThatDoesNotRun()
{
    echo "Ce qmake ne s'exécute pas : $1" >&2

    if [ "$(uname -s)" = "Darwin" ] && [ "$(uname -m)" = "arm64" ]
    then
        echo "Cette machine est un Mac Apple Silicon et le Qt 5.15.2 officiel n'existe qu'en x86_64 :" >&2
        echo "ses outils ont besoin de Rosetta 2, qui s'installe par" >&2
        echo "    softwareupdate --install-rosetta" >&2
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

        # Le || garde du set -e des deux appelants : sans lui, un Qt qui ne convient pas arrêterait
        # le script ici, avant le message qui dit pourquoi.
        thisSuitability=0
        qtSuitability "$1/bin/qmake" || thisSuitability=$?

        if [ "$thisSuitability" -eq 1 ]
        then
            reportQmakeThatDoesNotRun "$1/bin/qmake"
            return 1
        fi

        if [ "$thisSuitability" -eq 2 ]
        then
            reportQtWithoutWebEngine "$1"
            return 1
        fi

        qtDir="$1"
        return 0
    fi

    candidateQtDirs

    # Le premier qmake qui n'a pas démarré, pour le cas où aucun candidat ne convient : c'est une
    # cause bien plus précise que « aucun Qt avec QtWebEngine », et sur un Mac Apple Silicon sans
    # Rosetta 2 c'est la seule vraie.
    qmakeThatDoesNotRun=""

    for thisCandidate in "${candidateQtDirsResult[@]}"
    do
        thisSuitability=0
        qtSuitability "$thisCandidate/bin/qmake" || thisSuitability=$?

        if [ "$thisSuitability" -eq 0 ]
        then
            qtDir="$thisCandidate"
            return 0
        fi

        if [ "$thisSuitability" -eq 1 ] && [ -z "$qmakeThatDoesNotRun" ]
        then
            qmakeThatDoesNotRun="$thisCandidate/bin/qmake"
        fi
    done

    if [ "${#candidateQtDirsResult[@]}" -eq 0 ]
    then
        echo "Qt introuvable : passez son chemin, ou mettez son qmake dans le PATH." >&2
    elif [ -n "$qmakeThatDoesNotRun" ]
    then
        reportQmakeThatDoesNotRun "$qmakeThatDoesNotRun"
    else
        reportQtWithoutWebEngine ""
    fi

    return 1
}
