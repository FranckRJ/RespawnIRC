QT += \
    core \
    gui \
    network \
    widgets \
    multimedia \
    webenginewidgets

TARGET = RespawnIRC
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

RC_FILE = respawnIrc.rc

CONFIG += c++14
CONFIG += strict_c++

QMAKE_CXXFLAGS_RELEASE += -O2

LIBS += -L$$PWD/../hunspell/lib/ -llibhunspell
INCLUDEPATH += $$PWD/../hunspell/include
DEPENDPATH += $$PWD/../hunspell/include

SOURCES += \
    customWebPage.cpp \
    main.cpp \
    mainWindow.cpp \
    respawnIrc.cpp \
    connectWindow.cpp \
    parsingTool.cpp \
    selectTopicWindow.cpp \
    ignoreListWindow.cpp \
    addPseudoWindow.cpp \
    settingTool.cpp \
    multiTypeTextBox.cpp \
    checkUpdate.cpp \
    accountListWindow.cpp \
    colorPseudoListWindow.cpp \
    styleTool.cpp \
    selectThemeWindow.cpp \
    spellTextEdit.cpp \
    highlighter.cpp \
    autoTimeoutReply.cpp \
    containerForTopicsInfos.cpp \
    sendMessages.cpp \
    selectStickerWindow.cpp \
    getTopicMessages.cpp \
    preferencesWindow.cpp \
    shortcutTool.cpp \
    imageDownloadTool.cpp \
    webNavigator.cpp \
    customWebView.cpp \
    viewThemeInfos.cpp \
    showForum.cpp \
    showTopic.cpp \
    messageActions.cpp \
    tabViewTopicInfos.cpp \
    manageShortcutWindow.cpp \
    addShortcutWindow.cpp \
    utilityTool.cpp \
    addCookieWindow.cpp \
    clickableLabel.cpp \
    baseDialog.cpp

HEADERS += \
    customWebPage.hpp \
    mainWindow.hpp \
    respawnIrc.hpp \
    connectWindow.hpp \
    parsingTool.hpp \
    selectTopicWindow.hpp \
    ignoreListWindow.hpp \
    addPseudoWindow.hpp \
    settingTool.hpp \
    multiTypeTextBox.hpp \
    checkUpdate.hpp \
    colorPseudoListWindow.hpp \
    accountListWindow.hpp \
    styleTool.hpp \
    selectThemeWindow.hpp \
    spellTextEdit.hpp \
    highlighter.hpp \
    autoTimeoutReply.hpp \
    containerForTopicsInfos.hpp \
    sendMessages.hpp \
    selectStickerWindow.hpp \
    getTopicMessages.hpp \
    preferencesWindow.hpp \
    shortcutTool.hpp \
    imageDownloadTool.hpp \
    webNavigator.hpp \
    customWebView.hpp \
    viewThemeInfos.hpp \
    configDependentVar.hpp \
    showForum.hpp \
    showTopic.hpp \
    messageActions.hpp \
    tabViewTopicInfos.hpp \
    manageShortcutWindow.hpp \
    addShortcutWindow.hpp \
    utilityTool.hpp \
    addCookieWindow.hpp \
    clickableLabel.hpp \
    baseDialog.hpp
