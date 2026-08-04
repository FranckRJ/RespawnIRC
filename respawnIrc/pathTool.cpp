#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPair>
#include <QStandardPaths>
#include <QVector>

#include "pathTool.hpp"

namespace
{
    /* Dossier parent d'un chemin, pour créer l'arborescence avant d'écrire. */
    QString directoryOfThisPath(const QString& thisPath)
    {
        return thisPath.left(thisPath.lastIndexOf('/'));
    }

#ifndef Q_OS_WIN
    /* Dossier standard du système, avec repli à côté de l'exécutable : QStandardPaths peut rendre
     * une chaîne vide si le nom de l'application n'a pas été défini, et rien ne serait écrit. */
    QString standardDirPath(QStandardPaths::StandardLocation typeOfLocation)
    {
        QString pathOfLocation = QStandardPaths::writableLocation(typeOfLocation);

        if(pathOfLocation.isEmpty() == true)
        {
            return pathTool::dataDirPath() + "/userdata";
        }

        return pathOfLocation;
    }
#endif

    /* Déplace un fichier ou un dossier laissé par une version antérieure, sans jamais écraser ce
     * qui serait déjà en place. */
    void moveIfStillNeeded(const QString& oldPath, const QString& newPath)
    {
        if(oldPath == newPath || QFileInfo::exists(oldPath) == false || QFileInfo::exists(newPath) == true)
        {
            return;
        }

        QDir().mkpath(directoryOfThisPath(newPath));
        QDir().rename(oldPath, newPath);
    }
}

QString pathTool::dataDirPath()
{
#ifdef Q_OS_MACOS
    /* resources/ et themes/ sont dans Contents/Resources : le QMAKE_BUNDLE_DATA du .pro les y met à
     * chaque compilation, le bundle est donc autonome et le même qu'on le distribue ou non. Le
     * binaire des tests, lui, n'est pas un bundle et retombe sur la ligne commune. */
    if(QCoreApplication::applicationDirPath().endsWith(".app/Contents/MacOS") == true)
    {
        return QDir::cleanPath(QCoreApplication::applicationDirPath() + "/../Resources");
    }
#endif

    return QCoreApplication::applicationDirPath();
}

QString pathTool::configDirPath()
{
#if defined(Q_OS_WIN)
    return dataDirPath() + "/userdata";
#elif defined(Q_OS_MACOS)
    /* macOS ne sépare pas la configuration des données, et AppConfigLocation y désigne
     * ~/Library/Preferences, réservé aux .plist gérés par cfprefsd : un config.ini n'y a pas sa
     * place, il va donc dans Application Support avec le reste. Y poser le fichier quand même ne
     * casserait rien — cfprefsd ne touche qu'aux fichiers qu'il reconnaît comme domaines — mais
     * AppConfigLocation y créerait un sous-dossier RespawnIRC dans un dossier censé être plat. Et
     * écrire un vrai plist à la place a été étudié puis écarté, voir CLAUDE.md. */
    return standardDirPath(QStandardPaths::AppDataLocation);
#else
    return standardDirPath(QStandardPaths::AppConfigLocation);
#endif
}

QString pathTool::configFilePath()
{
    QDir().mkpath(configDirPath());

    return configDirPath() + "/config.ini";
}

QString pathTool::userDataDirPath()
{
#ifdef Q_OS_WIN
    return dataDirPath() + "/userdata";
#else
    return standardDirPath(QStandardPaths::AppDataLocation);
#endif
}

QString pathTool::cacheDirPath()
{
#ifdef Q_OS_WIN
    return dataDirPath() + "/userdata";
#else
    return standardDirPath(QStandardPaths::CacheLocation);
#endif
}

QString pathTool::logDirPath()
{
    return cacheDirPath() + "/logs";
}

QStringList pathTool::dirPathsForReading()
{
    QStringList listOfDirPaths;

    /* Le cache d'abord, où atterrissent les stickers téléchargés, puis les données de
     * l'utilisateur, puis celles livrées avec le programme. Sous Windows les trois premiers
     * désignent le même dossier, d'où le dédoublonnage.
     *
     * La liste doit être nommée : parcourir directement QStringList() << ... fait boucler sur un
     * temporaire déjà détruit, l'opérateur rendant une référence et non la valeur, ce qui prive la
     * boucle de la prolongation de durée de vie. */
    const QStringList listOfDirPathsToConsider = {cacheDirPath(), userDataDirPath(), dataDirPath()};

    for(const QString& thisDirPath : listOfDirPathsToConsider)
    {
        if(listOfDirPaths.contains(thisDirPath) == false)
        {
            listOfDirPaths.append(thisDirPath);
        }
    }

    return listOfDirPaths;
}

QString pathTool::pathForReading(const QString& relativePath)
{
    QString pathInUserData = userDataDirPath() + "/" + relativePath;

    if(QFileInfo::exists(pathInUserData) == true)
    {
        return pathInUserData;
    }

    return dataDirPath() + "/" + relativePath;
}

QString pathTool::pathForWriting(const QString& relativePath)
{
    QString pathInUserData = userDataDirPath() + "/" + relativePath;

    QDir().mkpath(directoryOfThisPath(pathInUserData));

    return pathInUserData;
}

void pathTool::migrateOldUserDataIfNeeded()
{
    /* Les versions antérieures posaient tout ce que le programme écrit à côté de l'exécutable,
     * mélangé aux données livrées avec lui. On déplace ces fichiers au premier démarrage, sans quoi
     * l'utilisateur croirait avoir perdu ses comptes et ses réglages.
     *
     * Les stickers téléchargés, eux, ne sont pas déplaçables : rien dans resources/stickers/ ne les
     * distingue de ceux livrés avec le programme. Ils y restent, et la lecture à plusieurs dossiers
     * continue de les trouver.
     */
    QStringList listOfOldDirPaths;

    listOfOldDirPaths << dataDirPath();

#ifndef Q_OS_WIN
    /* Sous Windows userdata/ est la destination et non une origine. Ailleurs il a pu être créé par
     * une compilation de développement antérieure au passage aux dossiers du système. */
    listOfOldDirPaths << dataDirPath() + "/userdata";
#endif

    for(const QString& thisOldDirPath : listOfOldDirPaths)
    {
        QVector<QPair<QString, QString>> listOfPathsToMove;

        listOfPathsToMove.append(qMakePair(thisOldDirPath + "/config.ini", configFilePath()));
        listOfPathsToMove.append(qMakePair(thisOldDirPath + "/logs", logDirPath()));
        listOfPathsToMove.append(qMakePair(thisOldDirPath + "/resources/shortcut.txt",
                                           userDataDirPath() + "/resources/shortcut.txt"));

        for(const QString& thisDicFile : QDir(thisOldDirPath).entryList(QStringList("user_*.dic"), QDir::Files))
        {
            listOfPathsToMove.append(qMakePair(thisOldDirPath + "/" + thisDicFile,
                                               userDataDirPath() + "/" + thisDicFile));
        }

        for(const QPair<QString, QString>& thisPathToMove : listOfPathsToMove)
        {
            moveIfStillNeeded(thisPathToMove.first, thisPathToMove.second);
        }
    }
}
