#include <QRegularExpression>
#include <QString>
#include <QStringList>

#include "testTool.hpp"
#include "configDependentVar.hpp"

/* Le découpage en mots du correcteur, avec le motif et l'option qu'il emploie vraiment. */
void runSpellCheckTests()
{
    testTool::startGroup("Frontière de mot du correcteur");

    const QRegularExpression expForSeparators(configDependentVar::expForWordSeparatorPattern + "+",
                                              configDependentVar::expForWordSeparatorOptions);
    const QRegularExpression expForWholeWord(R"rgx(\b)rgx" + QRegularExpression::escape("café") + R"rgx(\b)rgx",
                                             configDependentVar::expForWordSeparatorOptions);

    testTool::checkEquals("mots accentués", QString("café très tôt").split(expForSeparators).join('|'),
                          QString("café|très|tôt"));
    testTool::checkEquals("apostrophes et tirets", QString("aujourd'hui c'est l'arc-en-ciel").split(expForSeparators).join('|'),
                          QString("aujourd'hui|c'est|l'arc-en-ciel"));
    testTool::checkTrue("mot accentué retrouvé entier", QString("un café serré").contains(expForWholeWord));
    testTool::checkTrue("pas au milieu d'un mot", QString("cafétéria").contains(expForWholeWord) == false);
}
