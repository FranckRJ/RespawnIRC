#include <QVBoxLayout>

#include "viewThemeInfos.hpp"
#include "styleTool.hpp"

viewThemeInfosClass::viewThemeInfosClass(QWidget* parent) : QWidget(parent)
{
    informations = new QLabel(this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(informations);
    mainLayout->addStretch(1);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
}

void viewThemeInfosClass::setThemeToShow(QString newThemeName)
{
    if(newThemeName.isEmpty() == false)
    {
        QString modelTheme = styleToolClass::getModel(newThemeName);

        bool hasButtonQuote = modelTheme.contains("<%BUTTON_QUOTE%>");
        bool hasButtonBlacklist = modelTheme.contains("<%BUTTON_BLACKLIST%>");
        bool hasButtonEdit = modelTheme.contains("<%BUTTON_EDIT%>");
        bool hasButtonDelete = modelTheme.contains("<%BUTTON_DELETE%>");
        bool hasSupportSignature = modelTheme.contains("<%SIGNATURE_MODEL%>");
        bool hasSupportAvatars = modelTheme.contains("<%AVATAR_MODEL%>");

        informations->setText("<h3>" + newThemeName + "</h3>" +
                              "Bouton citer : " + boolToYesOrNo(hasButtonQuote) + "<br>" +
                              "Bouton ignorer : " + boolToYesOrNo(hasButtonBlacklist) + "<br>" +
                              "Bouton éditer : " + boolToYesOrNo(hasButtonEdit) + "<br>" +
                              "Bouton supprimer : " + boolToYesOrNo(hasButtonDelete) + "<br>" +
                              "Support des signatures : " + boolToYesOrNo(hasSupportSignature) + "<br>" +
                              "Support des avatars : " + boolToYesOrNo(hasSupportAvatars));
    }
    else
    {
        informations->setText("<h3>Erreur</h3>"
                              "Bouton citer : <br>"
                              "Bouton ignorer : <br>"
                              "Bouton éditer : <br>"
                              "Bouton supprimer : <br>"
                              "Support des signatures : <br>"
                              "Support des avatars : ");
    }
}

QString viewThemeInfosClass::boolToYesOrNo(bool boolToCheck) const
{
    return (boolToCheck == true ? "<b>oui</b>" : "<i>non</i>");
}
