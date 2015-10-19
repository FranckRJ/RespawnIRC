SOURCES += \
    main.cpp \
    mainWindow.cpp \
    respawnIrc.cpp \
    connectWindow.cpp \
    parsingTool.cpp \
    selectTopicWindow.cpp \
    captchaWindow.cpp \
    showTopicMessages.cpp \
    ignoreListWindow.cpp \
    addPseudoWindow.cpp \
    settingTool.cpp \
    multiTypeTextBox.cpp \
    chooseNumberWindow.cpp \
    checkUpdate.cpp \
    accountListWindow.cpp \
    colorPseudoListWindow.cpp \
    showListOfTopic.cpp \
    styleTool.cpp \
    selectThemeWindow.cpp

QT += \
    network \
    widgets \
    multimedia

HEADERS += \
    mainWindow.hpp \
    respawnIrc.hpp \
    connectWindow.hpp \
    parsingTool.hpp \
    selectTopicWindow.hpp \
    captchaWindow.hpp \
    showTopicMessages.hpp \
    ignoreListWindow.hpp \
    addPseudoWindow.hpp \
    settingTool.hpp \
    multiTypeTextBox.hpp \
    chooseNumberWindow.hpp \
    checkUpdate.hpp \
    colorPseudoListWindow.hpp \
    accountListWindow.hpp \
    showListOfTopic.hpp \
    styleTool.hpp \
    selectThemeWindow.hpp

RC_FILE = respawnIrc.rc
