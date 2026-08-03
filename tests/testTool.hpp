#ifndef TESTTOOL_HPP
#define TESTTOOL_HPP

#include <QString>
#include <QTextStream>
#include <QVariant>

/* Petit harnais de test maison : le programme n'utilise pas QtTest, et le but est de
 * pouvoir vérifier le parsing sur de vraies pages sauvegardées sans lancer l'interface. */
namespace testTool
{
    int numberOfChecksDone();
    int numberOfChecksFailed();

    void startGroup(const QString& nameOfGroup);
    void reportSuccess(const QString& nameOfCheck);
    void reportFailure(const QString& nameOfCheck, const QString& expected, const QString& obtained);

    /* Charge une fixture depuis tests/fixtures. Les .html.gz sont décompressés à la
     * volée, pour ne pas garder des centaines de kilo-octets de HTML dans le dépôt. */
    QString loadFixture(const QString& nameOfFile);

    int finish();

    /* QVariant ne sait pas construire depuis un `long`, d'où le passage explicite par
     * qlonglong pour les types entiers. */
    inline QString valueToString(const QString& value) { return value; }
    inline QString valueToString(bool value) { return (value ? "vrai" : "faux"); }
    inline QString valueToString(qlonglong value) { return QString::number(value); }
    inline QString valueToString(int value) { return QString::number(value); }
    inline QString valueToString(long value) { return QString::number(static_cast<qlonglong>(value)); }

    template<typename T, typename U>
    void checkEquals(const QString& nameOfCheck, const T& obtained, const U& expected)
    {
        if(obtained == expected)
        {
            reportSuccess(nameOfCheck);
        }
        else
        {
            reportFailure(nameOfCheck, valueToString(expected), valueToString(obtained));
        }
    }

    inline void checkTrue(const QString& nameOfCheck, bool obtained)
    {
        checkEquals(nameOfCheck, obtained, true);
    }

    void checkContains(const QString& nameOfCheck, const QString& obtained, const QString& expectedPart);
    void checkDoesNotContain(const QString& nameOfCheck, const QString& obtained, const QString& forbiddenPart);
}

void runParsingTests();

#endif
