#include <QCoreApplication>
#include <QFile>
#include <QTextStream>

#include "testTool.hpp"
#include "payloadTool.hpp"

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

    runParsingTests();

    return testTool::finish();
}
