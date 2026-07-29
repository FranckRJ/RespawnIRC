QT += \
    core \
    gui \
    network \
    widgets \
    multimedia \
    webenginewidgets

TARGET = RespawnIRC
TEMPLATE = app

# Le programme cherche resources/ et themes/ à côté de son exécutable : on le produit donc
# directement à la racine du dépôt, où ces dossiers sont déjà, plutôt que de demander un
# déplacement à la main après chaque compilation. $$PWD est le dossier de ce .pro, la cible est
# donc la même que la compilation ait lieu dans les sources ou dans build/.
DESTDIR = $$PWD/..

DEFINES += QT_DEPRECATED_WARNINGS

RC_FILE = respawnIrc.rc

# Informations du bundle macOS. L'icône est celle de Windows convertie, elle plafonne donc à 128
# pixels : elle est un peu molle dans les grands affichages du Finder, il faudrait une source plus
# grande pour y remédier.
macx {
    ICON = rirc.icns
    QMAKE_BUNDLE = RespawnIRC
    QMAKE_INFO_PLIST = Info.plist

    # La version affichée par le Finder est lue dans respawnIrc.cpp pour n'avoir qu'une seule source ;
    # qmake la remplace ensuite dans Info.plist à la place de @FULL_VERSION@. L'extraction se fait
    # sans passer par le shell, dont les quotes ne survivent pas à l'analyse de qmake : la ligne
    # cherchée est celle de currentVersionName, on enlève tout jusqu'au v du numéro puis tout ce qui
    # suit le numéro lui-même.
    linesOfSource = $$cat($$PWD/respawnIrc.cpp, lines)
    lineOfVersion = $$find(linesOfSource, currentVersionName..v[0-9])
    versionAfterTheV = $$replace(lineOfVersion, .*v, )
    VERSION = $$replace(versionAfterTheV, [^0-9.].*, )

    isEmpty(VERSION) {
        error("Version introuvable dans respawnIrc.cpp, le bundle serait marqué 1.0.0.")
    }
}

CONFIG += c++14
CONFIG += strict_c++

QMAKE_CXXFLAGS_RELEASE += -O2

# Hunspell est attendu dans un dossier `hunspell` à la racine du dépôt (voir le wiki). Sous macOS
# on se rabat sur celui de Homebrew, dont la bibliothèque porte un nom versionné contrairement à
# celle de Debian ; ce nom peut être précisé : qmake HUNSPELL_LIB_NAME=hunspell-1.8
HUNSPELL_DIR = $$PWD/../hunspell

macx:!exists($$HUNSPELL_DIR/include/hunspell/hunspell.hxx) {
    HUNSPELL_DIR = $$system(brew --prefix hunspell)
    isEmpty(HUNSPELL_LIB_NAME): HUNSPELL_LIB_NAME = hunspell-1.7
}

isEmpty(HUNSPELL_LIB_NAME): HUNSPELL_LIB_NAME = hunspell

LIBS += -L$$HUNSPELL_DIR/lib/ -l$$HUNSPELL_LIB_NAME
INCLUDEPATH += $$HUNSPELL_DIR/include
DEPENDPATH += $$HUNSPELL_DIR/include

# zlib sert à décompresser le payload JSON des pages de jeuxvideo.com
include(../zlib.pri)

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
    logTool.cpp \
    payloadTool.cpp \
    addCookieWindow.cpp \
    clickableLabel.cpp \
    baseDialog.cpp \
    pathTool.cpp

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
    logTool.hpp \
    payloadTool.hpp \
    addCookieWindow.hpp \
    clickableLabel.hpp \
    baseDialog.hpp \
    pathTool.hpp
