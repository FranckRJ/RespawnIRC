#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "testTool.hpp"
#include "payloadTool.hpp"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace
{
    int checksDone = 0;
    int checksFailed = 0;

    QTextStream& out()
    {
        static QTextStream stream(stdout);
        static bool codecIsSet = [&]() { stream.setCodec("UTF-8"); return true; }();
        Q_UNUSED(codecIsSet)
        return stream;
    }
}

int testTool::numberOfChecksDone()
{
    return checksDone;
}

int testTool::numberOfChecksFailed()
{
    return checksFailed;
}

void testTool::startGroup(const QString& nameOfGroup)
{
    out() << "\n== " << nameOfGroup << "\n";
    out().flush();
}

void testTool::reportSuccess(const QString& nameOfCheck)
{
    ++checksDone;
    out() << "  ok   " << nameOfCheck << "\n";
    out().flush();
}

void testTool::reportFailure(const QString& nameOfCheck, const QString& expected, const QString& obtained)
{
    ++checksDone;
    ++checksFailed;
    out() << "  FAIL " << nameOfCheck << "\n";
    out() << "       attendu : " << expected.left(400) << "\n";
    out() << "       obtenu  : " << obtained.left(400) << "\n";
    out().flush();
}

void testTool::checkContains(const QString& nameOfCheck, const QString& obtained, const QString& expectedPart)
{
    if(obtained.contains(expectedPart) == true)
    {
        reportSuccess(nameOfCheck);
    }
    else
    {
        reportFailure(nameOfCheck, "contient « " + expectedPart + " »", obtained);
    }
}

void testTool::checkDoesNotContain(const QString& nameOfCheck, const QString& obtained, const QString& forbiddenPart)
{
    if(obtained.contains(forbiddenPart) == false)
    {
        reportSuccess(nameOfCheck);
    }
    else
    {
        reportFailure(nameOfCheck, "ne contient pas « " + forbiddenPart + " »", obtained);
    }
}

QString testTool::loadFixture(const QString& nameOfFile)
{
    QString path = QString(FIXTURES_PATH) + "/" + nameOfFile;
    QFile file(path);

    if(file.open(QIODevice::ReadOnly) == false)
    {
        out() << "Fixture introuvable : " << path << "\n";
        out().flush();
        return "";
    }

    QByteArray content = file.readAll();

    if(nameOfFile.endsWith(".gz") == true)
    {
        content = payloadTool::gzipUncompress(content);
    }

    return QString::fromUtf8(content);
}

int testTool::finish()
{
    /* QTextStream interprète un const char* en latin-1, d'où le QString explicite. */
    out() << "\n" << checksDone << QString(" vérifications, ") << checksFailed << QString(" échec(s).\n");
    out().flush();
    return (checksFailed == 0 ? 0 : 1);
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

#ifdef Q_OS_WIN
    /* Le flux ci-dessus écrit en UTF-8, ce que la console de Windows n'est pas par défaut : sans
     * cet appel les accents des noms de vérifications s'affichent en mojibake. On remet la page de
     * codes d'origine à la sortie, parce que c'est un réglage de la console et pas du processus :
     * elle resterait sinon en UTF-8 pour tout ce qui serait tapé ensuite dans la même fenêtre.
     * Sortie redirigée vers un fichier ou un tube, ces deux appels ne font rien et n'ont rien à
     * faire : les octets UTF-8 y partent tels quels, c'est au lecteur de les décoder. */
    UINT previousConsoleCodePage = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
#endif

    runParsingTests();

    int numberOfFailures = testTool::finish();

#ifdef Q_OS_WIN
    if(previousConsoleCodePage != 0)
    {
        SetConsoleOutputCP(previousConsoleCodePage);
    }
#endif

    return numberOfFailures;
}
