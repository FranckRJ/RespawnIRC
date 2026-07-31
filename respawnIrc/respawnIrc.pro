QT += \
    core \
    gui \
    network \
    widgets \
    multimedia \
    webenginewidgets

TARGET = RespawnIRC
TEMPLATE = app

# Le programme et ses données sortent ensemble dans build/, comme respawnIrcTests : rien n'atterrit
# plus à la racine du dépôt, qui ne porte donc que des sources. C'est resources/ et themes/ que le
# programme cherche à côté de son exécutable, et ils l'y suivent — dans le bundle sous macOS, par la
# copie du bloc !macx plus bas ailleurs. $$PWD est le dossier de ce .pro, la cible est donc la même
# que la compilation ait lieu dans les sources ou dans build/.
DESTDIR = $$PWD/../build

DEFINES += QT_DEPRECATED_WARNINGS

# Le numéro de version vient de version.pri, qui en est la seule source, et arrive dans le programme
# par ce DEFINES : respawnIrc.cpp le préfixe d'un v pour en faire currentVersionName.
include(../version.pri)

isEmpty(RESPAWNIRC_VERSION) {
    error("RESPAWNIRC_VERSION est vide : version.pri manque ou n'a pas la forme attendue.")
}

DEFINES += RESPAWNIRC_VERSION=\\\"$$RESPAWNIRC_VERSION\\\"

RC_FILE = respawnIrc.rc

# Informations du bundle macOS. L'icône est celle de Windows convertie, elle plafonne donc à 128
# pixels : elle est un peu molle dans les grands affichages du Finder, il faudrait une source plus
# grande pour y remédier.
macx {
    ICON = rirc.icns
    QMAKE_BUNDLE = RespawnIRC
    QMAKE_INFO_PLIST = Info.plist

    # La version affichée par le Finder est la même que celle du programme, celle de version.pri :
    # qmake remplace @FULL_VERSION@ par cette VERSION dans Info.plist. Elle reste dans ce bloc parce
    # que c'est le seul endroit qui la lit, la VERSION de qmake n'ayant pas d'usage ailleurs ici.
    VERSION = $$RESPAWNIRC_VERSION

    # Les données livrées entrent dans le bundle, qui est ainsi autonome et se déplace d'un bloc :
    # c'est ce que pathTool::dataDirPath() y cherche, et la seule disposition qui existe sous macOS,
    # une compilation ordinaire donnant le même bundle que celui qu'on distribue. dist-macos.sh
    # remplace ensuite ces deux dossiers par leur version commitée, le dossier de travail contenant
    # aussi ce que le mainteneur a accumulé en se servant du programme.
    dataOfBundle.files = $$PWD/../resources $$PWD/../themes
    dataOfBundle.path = Contents/Resources
    QMAKE_BUNDLE_DATA += dataOfBundle
}

# Pendant du QMAKE_BUNDLE_DATA ci-dessus pour les plateformes sans bundle : les deux dossiers sont
# recopiés à côté de l'exécutable à chaque édition de liens, puisque c'est là que
# pathTool::dataDirPath() les cherche. Les scripts de distribution les remplacent ensuite par leur
# version commitée, le dossier de travail contenant aussi ce que le mainteneur a accumulé en se
# servant du programme.
#
# La destination s'écrit différemment selon l'outil de copie, et c'est la seule chose de ce .pro qui
# distingue les plateformes en dehors du bloc macx : cp veut le dossier parent et fusionne dans une
# destination déjà en place, xcopy veut le dossier cible. Il n'y a pas ici, contrairement au
# DEFINES+=HUNSPELL_STATIC de Windows, de ligne de commande qmake où mettre la différence.
!macx {
    for(nameOfData, $$list(resources themes)) {
        win32: pathOfDataDest = $$shell_path($$DESTDIR/$$nameOfData)
        else: pathOfDataDest = $$shell_path($$DESTDIR)

        QMAKE_POST_LINK += $(COPY_DIR) $$shell_path($$clean_path($$PWD/../$$nameOfData)) \
            $$pathOfDataDest $$escape_expand(\\n\\t)
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
