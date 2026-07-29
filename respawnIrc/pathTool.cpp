#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#include "pathTool.hpp"

namespace
{
    /* Dossier parent d'un chemin, pour créer l'arborescence avant d'écrire. */
    QString directoryOfThisPath(const QString& thisPath)
    {
        return thisPath.left(thisPath.lastIndexOf('/'));
    }
}

QString pathTool::dataDirPath()
{
#ifdef Q_OS_MACOS
    QDir dirOfApp(QCoreApplication::applicationDirPath());

    if(dirOfApp.absolutePath().endsWith(".app/Contents/MacOS") == true)
    {
        dirOfApp.cdUp(); /* Contents */
        dirOfApp.cdUp(); /* RespawnIRC.app */
        dirOfApp.cdUp(); /* dossier contenant le bundle */
        return dirOfApp.absolutePath();
    }
#endif

    return QCoreApplication::applicationDirPath();
}

QString pathTool::userDataDirPath()
{
    return dataDirPath() + "/userdata";
}

QStringList pathTool::dirPathsForReading()
{
    QStringList listOfDirPaths;

    listOfDirPaths << userDataDirPath() << dataDirPath();

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
    /* Avant l'introduction de userdata/, tout ce que le programme écrivait était posé à côté de
     * l'exécutable, mélangé aux données livrées avec lui. On déplace ces fichiers au premier
     * démarrage, sans quoi l'utilisateur croirait avoir perdu ses comptes et ses réglages.
     *
     * Les stickers téléchargés, eux, ne sont pas déplaçables : rien dans resources/stickers/ ne
     * les distingue de ceux livrés avec le programme. Ils y restent, et la lecture à deux dossiers
     * continue de les trouver.
     */
    QStringList listOfRelativePathsToMove;

    listOfRelativePathsToMove << "config.ini" << "logs" << "resources/shortcut.txt";
    listOfRelativePathsToMove << QDir(dataDirPath()).entryList(QStringList("user_*.dic"), QDir::Files);

    for(const QString& thisRelativePath : listOfRelativePathsToMove)
    {
        QString oldPath = dataDirPath() + "/" + thisRelativePath;
        QString newPath = userDataDirPath() + "/" + thisRelativePath;

        if(QFileInfo::exists(oldPath) == true && QFileInfo::exists(newPath) == false)
        {
            QDir().mkpath(directoryOfThisPath(newPath));
            QDir().rename(oldPath, newPath);
        }
    }
}
