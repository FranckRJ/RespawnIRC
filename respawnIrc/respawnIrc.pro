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

# Un numéro de version arrive par la ligne de commande du compilateur, que make ne voit pas : rien ne
# rattachait les objets déjà compilés à version.pri, et un numéro changé ne leur parvenait donc pas.
# Constaté avant ce correctif, et pas seulement possible : version.pri passé à 3.1.42, un
# ./build-unix.sh donnait un bundle dont Info.plist annonçait 3.1.42 et dont le programme annonçait
# toujours v3.1.17. L'effacement du bundle par le script ne s'en gardait pas — il force l'édition de
# liens, pas la compilation, et le respawnIrc.o resté en place portait l'ancien numéro.
#
# $(OBJECTS) est la liste que qmake a lui-même écrite dans le Makefile : les noms des cibles y sont
# donc forcément ceux de ses propres règles, sans rien à deviner. Tout recompiler pour un changement
# de version coûte moins cher que de tenir à jour la liste des sources qui lisent le DEFINES —
# aujourd'hui respawnIrc.cpp seul, et rien ne le dit ici.
dependsOfVersion.target = $(OBJECTS)
dependsOfVersion.depends = $$clean_path($$PWD/../version.pri)
QMAKE_EXTRA_TARGETS += dependsOfVersion

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

    # Deux des règles que qmake engendre pour le bundle n'ont pas les dépendances qu'il faudrait, et
    # make les saute donc dès qu'un bundle est en place : celle d'Info.plist n'en a aucune, et celles
    # de resources/ et themes/ n'ont que le dossier source, dont la date ne bouge pas quand un
    # fichier change dedans. On leur ajoute ici les prérequis manquants, et rien d'autre : en make,
    # une même cible peut apparaître dans plusieurs règles tant qu'une seule porte des commandes, les
    # prérequis s'additionnant. C'est ce qui rend le bundle réparable sans l'effacer.
    #
    # Le nom de la cible doit être écrit **exactement** comme qmake l'écrit dans le Makefile : make
    # compare des chaînes et ne sait pas que deux chemins désignent le même fichier, donc une écriture
    # différente donnerait une seconde cible, dont personne ne dépend et qui ne servirait à rien sans
    # que rien ne le signale. D'où $(DESTDIR), la variable du Makefile : c'est de là que viennent les
    # règles du bundle, la reprendre évite de deviner comment qmake a écrit ce chemin — relatif au
    # dossier de compilation, et pas de la façon la plus courte.
    pathOfBundleForMake = $(DESTDIR)$${QMAKE_BUNDLE}.app

    dependsOfInfoPlist.target = $$pathOfBundleForMake/Contents/Info.plist
    dependsOfInfoPlist.depends = $$PWD/Info.plist $$clean_path($$PWD/../version.pri)
    QMAKE_EXTRA_TARGETS += dependsOfInfoPlist

    # Les fichiers des deux dossiers, listés récursivement. Cette liste-là est figée au moment du
    # qmake, mais elle n'a pas à connaître les fichiers ajoutés depuis : c'est le dossier, que qmake
    # garde comme prérequis, qui rattrape l'ajout et la suppression — sa date bouge dans ces deux
    # cas, et pas quand un fichier change. Les deux prérequis sont complémentaires.
    for(nameOfData, $$list(resources themes)) {
        eval(dependsOf$${nameOfData}.target = $$pathOfBundleForMake/Contents/Resources/$$nameOfData)
        eval(dependsOf$${nameOfData}.depends = $$files($$clean_path($$PWD/../$$nameOfData)/*, true))
        QMAKE_EXTRA_TARGETS += dependsOf$$nameOfData
    }
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

        # La copie se faisant à l'édition de liens, un thème modifié sans qu'un .cpp bouge ne
        # parvenait pas au programme : make ne trouvait rien à relier, donc rien à copier. Les
        # fichiers des deux dossiers deviennent donc des prérequis de l'exécutable, ce qui suffit à
        # rendre la main à ce QMAKE_POST_LINK. Le prix est une édition de liens pour un thème
        # modifié ; c'est le pendant de ce que le bloc macx obtient en ajoutant les mêmes fichiers
        # aux règles du bundle, qui elles ne passent pas par l'exécutable.
        PRE_TARGETDEPS += $$files($$clean_path($$PWD/../$$nameOfData)/*, true)
    }
}

CONFIG += c++14
CONFIG += strict_c++

QMAKE_CXXFLAGS_RELEASE += -O2

# Hunspell est attendu dans un dossier `hunspell` à la racine du dépôt (voir le wiki). Sous macOS
# on se rabat sur celui de Homebrew, dont la bibliothèque porte un nom versionné contrairement à
# celle de Debian ; ce nom peut être précisé : qmake HUNSPELL_LIB_NAME=hunspell-1.8
#
# Le dossier se désigne de la même façon, et c'est ce qui rend la compilation possible sur un Mac
# Apple Silicon : le programme y est en x86_64 comme ailleurs — le Qt 5.15.2 officiel n'existe pas
# autrement, et ses mkspecs mettent -arch x86_64 dans le Makefile — alors que le Homebrew de
# /opt/homebrew ne fournit que de l'arm64, qui ne se lie pas avec. Il faut donc un Hunspell x86_64 :
# celui du Homebrew de /usr/local, ou une compilation à soi posée dans le dossier `hunspell` du
# dépôt, que la ligne ci-dessous prend sans qu'on ait rien à désigner.
#
# Un dossier désigné n'est jamais remplacé en douce par celui de Homebrew, même s'il ne convient
# pas : c'est le principe qu'unix-common.sh applique déjà au Qt donné en argument.
hunspellDirWasGiven = $$HUNSPELL_DIR
isEmpty(HUNSPELL_DIR): HUNSPELL_DIR = $$PWD/../hunspell

macx:isEmpty(hunspellDirWasGiven):!exists($$HUNSPELL_DIR/include/hunspell/hunspell.hxx) {
    HUNSPELL_DIR = $$system(brew --prefix hunspell)
    isEmpty(HUNSPELL_LIB_NAME): HUNSPELL_LIB_NAME = hunspell-1.7

    # brew ne rend rien quand Hunspell n'est pas installé, et pas davantage quand c'est le Homebrew
    # de /opt/homebrew qui est appelé depuis un processus traduit par Rosetta 2 — ce que qmake est
    # forcément sur un Mac Apple Silicon, ses binaires étant en x86_64. Sans ce test, LIBS gardait
    # un -L/lib/ et l'échec n'apparaissait qu'à l'édition de liens, en « library not found for
    # -lhunspell-1.7 » qui ne dit pas d'où ce chemin venait ni pourquoi il est vide.
    isEmpty(HUNSPELL_DIR): \
        error("Hunspell introuvable : brew --prefix hunspell n'a rien rendu. Installez-le, ou désignez-le : qmake ... HUNSPELL_DIR=/usr/local/opt/hunspell HUNSPELL_LIB_NAME=hunspell-1.7")
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
