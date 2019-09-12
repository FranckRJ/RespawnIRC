#include "customWebPage.hpp"

customWebPageClass::customWebPageClass(QObject* newParent) : QWebEnginePage(newParent)
{
}

customWebPageClass::customWebPageClass(QWebEngineProfile* newProfile, QObject* newParent)
    : QWebEnginePage(newProfile, newParent)
{
}

void customWebPageClass::setNewPageAvailableCallback(const std::function<void(customWebPageClass*)>& newCallback)
{
    newPageAvailableCallback = newCallback;
}

QWebEnginePage* customWebPageClass::createWindow(QWebEnginePage::WebWindowType type)
{
    (void)type;
    customWebPageClass* newPage;

    if (profile() == nullptr)
    {
        newPage = new customWebPageClass(parent());
    }
    else
    {
        newPage = new customWebPageClass(profile(), parent());
    }

    if (newPageAvailableCallback)
    {
        newPageAvailableCallback(newPage);
    }

    return newPage;
}
