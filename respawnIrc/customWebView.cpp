#include <QMenu>
#include <QAction>
#include <QList>

#include "customWebView.hpp"

customWebViewClass::customWebViewClass(QWidget* parent) : QWebEngineView(parent)
{
    connect(this, &customWebViewClass::urlChanged, this, &customWebViewClass::newUrlVisited);
}

bool customWebViewClass::canGoBack()
{
    return (currentUrlID > 0 && currentUrlID < listOfVisitedUrl.size());
}

bool customWebViewClass::canGoForward()
{
    return (currentUrlID >= 0 && currentUrlID + 1 < listOfVisitedUrl.size());
}

void customWebViewClass::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = page()->createStandardContextMenu();
    const QList<QAction*> actions = menu->actions();
    for(QAction* thisAction : actions)
    {
        if(thisAction->toolTip() != page()->action(QWebEnginePage::Reload)->toolTip() && thisAction->toolTip() != page()->action(QWebEnginePage::CopyLinkToClipboard)->toolTip())
        {
            menu->removeAction(thisAction);
        }
    }
    menu->removeAction(page()->action(QWebEnginePage::Forward));
    connect(menu, &QMenu::aboutToHide, menu, &QMenu::deleteLater);
    menu->popup(event->globalPos());
}

QWebEngineView* customWebViewClass::createWindow(QWebEnginePage::WebWindowType type)
{
    (void)type;
    return this;
}

void customWebViewClass::back()
{
    if(canGoBack() == true)
    {
        nextUrlChangeHasToBeSaved = false;
        currentUrlID -= 1;
        setUrl(listOfVisitedUrl.at(currentUrlID));
    }
}

void customWebViewClass::forward()
{
    if(canGoForward() == true)
    {
        nextUrlChangeHasToBeSaved = false;
        currentUrlID += 1;
        setUrl(listOfVisitedUrl.at(currentUrlID));
    }
}

void customWebViewClass::newUrlVisited(const QUrl& thisUrl)
{
    if(nextUrlChangeHasToBeSaved == false)
    {
        nextUrlChangeHasToBeSaved = true;
        return;
    }

    if(currentUrlID >= 0 && currentUrlID < listOfVisitedUrl.size())
    {
        if(listOfVisitedUrl.at(currentUrlID).url() == thisUrl.url())
        {
            return;
        }
    }

    if(currentUrlID + 1 < listOfVisitedUrl.size())
    {
        listOfVisitedUrl.remove(currentUrlID + 1, listOfVisitedUrl.size() - currentUrlID - 1);
    }

    listOfVisitedUrl.push_back(thisUrl);
    currentUrlID = listOfVisitedUrl.size() - 1;
}
