QT += core gui network
CONFIG += console c++14
CONFIG += strict_c++
CONFIG -= app_bundle

TARGET = respawnIrcTests
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += FIXTURES_PATH=\\\"$$PWD/fixtures\\\"

include(../zlib.pri)

INCLUDEPATH += $$PWD/../respawnIrc

SOURCES += \
    main.cpp \
    testParsing.cpp \
    ../respawnIrc/parsingTool.cpp \
    ../respawnIrc/payloadTool.cpp \
    ../respawnIrc/logTool.cpp \
    ../respawnIrc/styleTool.cpp \
    ../respawnIrc/shortcutTool.cpp

HEADERS += \
    testTool.hpp
