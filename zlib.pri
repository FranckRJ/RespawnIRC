# Configuration de zlib, qui sert à décompresser le payload JSON des pages de
# jeuxvideo.com (voir payloadTool). Partagée entre le programme et les tests pour
# que la dépendance ne soit décrite qu'à un seul endroit.
#
# Sous Linux et macOS la zlib du système suffit, il n'y a rien à faire : sous
# Debian c'est le paquet zlib1g-dev.
#
# Sous Windows rien n'est fourni par le système. Deux cas :
#  - la toolchain contient déjà zlib (c'est le cas de la plupart des MinGW, dont
#    celui livré avec Qt) : rien à faire non plus ;
#  - sinon (MSVC notamment), compiler zlib et la placer dans un dossier `zlib` à
#    la racine du dépôt, à côté de `hunspell`, avec les en-têtes dans `include`
#    et la bibliothèque dans `lib`.
#
# Le nom de la bibliothèque peut être changé si celle que vous avez ne s'appelle
# pas comme prévu (`zlibstatic` avec un zlib compilé en statique par CMake, par
# exemple) : qmake ZLIB_LIB_NAME=zlibstatic

isEmpty(ZLIB_LIB_NAME) {
    win32-msvc* {
        ZLIB_LIB_NAME = zlib
    } else {
        ZLIB_LIB_NAME = z
    }
}

exists($$PWD/zlib/include/zlib.h) {
    INCLUDEPATH += $$PWD/zlib/include
    DEPENDPATH += $$PWD/zlib/include
    LIBS += -L$$PWD/zlib/lib/
}

LIBS += -l$$ZLIB_LIB_NAME
