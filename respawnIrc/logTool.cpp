#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QTextStream>
#include <QtGlobal>

#include "logTool.hpp"
#include "pathTool.hpp"

Q_LOGGING_CATEGORY(logNetwork, "respawnirc.network")
Q_LOGGING_CATEGORY(logParsing, "respawnirc.parsing")
Q_LOGGING_CATEGORY(logTopic, "respawnirc.topic")
Q_LOGGING_CATEGORY(logForum, "respawnirc.forum")

namespace
{
    QtMessageHandler previousMessageHandler = nullptr;
    QMutex mutexForLogFile;
    bool logToFileEnabled = false;
    int numberOfPagesDumped = 0;
    const int maxNumberOfPagesDumped = 20;

    QString directoryForLogs()
    {
        return pathTool::dataDirPath() + "/logs";
    }

    QString levelToString(QtMsgType type)
    {
        switch(type)
        {
            case QtDebugMsg: return "DEBUG";
            case QtInfoMsg: return "INFO ";
            case QtWarningMsg: return "WARN ";
            case QtCriticalMsg: return "ERROR";
            case QtFatalMsg: return "FATAL";
        }
        return "?????";
    }

    void writeLineToLogFile(const QString& line)
    {
        QMutexLocker locker(&mutexForLogFile);

        QFile file(logTool::logFilePath());
        if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text) == false)
        {
            return;
        }

        QTextStream stream(&file);
        stream << line << "\n";
    }

    void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
    {
        if(previousMessageHandler != nullptr)
        {
            previousMessageHandler(type, context, message);
        }

        if(logToFileEnabled == true)
        {
            QString category = (context.category != nullptr ? QString(context.category) : QString("default"));
            writeLineToLogFile(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") + " [" +
                               levelToString(type) + "] " + category + ": " + message);
        }
    }
}

void logTool::initialize()
{
    if(qEnvironmentVariableIsSet("RESPAWNIRC_DEBUG") == true && qgetenv("RESPAWNIRC_DEBUG") != "0")
    {
        logToFileEnabled = true;
        /* Les règles fournies par l'utilisateur passent avant, pour pouvoir n'activer
         * qu'une catégorie tout en gardant les logs fichier. */
        QLoggingCategory::setFilterRules("respawnirc.*.debug=true\n" + QString(qEnvironmentVariable("QT_LOGGING_RULES")));
    }

    previousMessageHandler = qInstallMessageHandler(messageHandler);

    if(logToFileEnabled == true)
    {
        QDir().mkpath(directoryForLogs());
        qInfo(logNetwork) << "RespawnIRC démarré, logs écrits dans" << logFilePath();
    }
}

bool logTool::logToFileIsEnabled()
{
    return logToFileEnabled;
}

void logTool::setLogToFileIsEnabled(bool newValue)
{
    if(newValue == true)
    {
        QDir().mkpath(directoryForLogs());
    }

    logToFileEnabled = newValue;
}

QString logTool::logFilePath()
{
    return directoryForLogs() + "/respawnirc.log";
}

bool logTool::pageDumpIsEnabled()
{
    return logToFileEnabled;
}

QString logTool::dumpFailedPage(const QString& nameHint, const QString& pageSource)
{
    if(pageDumpIsEnabled() == false || numberOfPagesDumped >= maxNumberOfPagesDumped)
    {
        return "";
    }

    QString cleanedNameHint = nameHint;
    cleanedNameHint.replace(QRegularExpression(R"rgx([^a-zA-Z0-9_-])rgx"), "_");
    cleanedNameHint.truncate(60);

    QString path = directoryForLogs() + "/page-" + QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss-zzz") +
                   "-" + cleanedNameHint + ".html";

    QDir().mkpath(directoryForLogs());

    QFile file(path);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text) == false)
    {
        return "";
    }

    QTextStream stream(&file);
    stream << pageSource;
    ++numberOfPagesDumped;

    return path;
}
