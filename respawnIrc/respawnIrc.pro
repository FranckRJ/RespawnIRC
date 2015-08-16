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
    settingTool.cpp

QT += \
    network

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
    settingTool.hpp

RC_FILE = respawnIrc.rc
