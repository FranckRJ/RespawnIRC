#ifndef CUSTOMWEBPAGE_HPP
#define CUSTOMWEBPAGE_HPP

#include <QWebEnginePage>

#include <functional>

class customWebPageClass : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit customWebPageClass(QObject* newParent = nullptr);
    explicit customWebPageClass(QWebEngineProfile* newProfile, QObject* newParent = nullptr);

    void setNewPageAvailableCallback(const std::function<void(customWebPageClass*)>& newCallback);

protected:
    QWebEnginePage* createWindow(QWebEnginePage::WebWindowType type) override;

private:
    std::function<void(customWebPageClass*)> newPageAvailableCallback;
};

#endif
