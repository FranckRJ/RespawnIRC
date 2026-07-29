QT += core gui network
CONFIG += console c++14
CONFIG += strict_c++
CONFIG -= app_bundle

TARGET = respawnIrcTests
TEMPLATE = app

# Comme pour le programme, l'exécutable est produit à un endroit fixe quelle que soit la façon de
# compiler. Les tests n'ont pas besoin de resources/ ni de themes/, ils lisent leurs fixtures par
# le chemin absolu ci-dessous : ils restent donc dans build/ plutôt qu'à la racine.
DESTDIR = $$PWD/../build

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += FIXTURES_PATH=\\\"$$PWD/fixtures\\\"

include(../zlib.pri)

INCLUDEPATH += $$PWD/../respawnIrc

SOURCES += \
    main.cpp \
    testParsing.cpp \
    ../respawnIrc/parsingTool.cpp \
    ../respawnIrc/payloadTool.cpp \
    ../respawnIrc/pathTool.cpp \
    ../respawnIrc/logTool.cpp \
    ../respawnIrc/styleTool.cpp \
    ../respawnIrc/shortcutTool.cpp

HEADERS += \
    testTool.hpp
