#ifndef CUSTOMWEBVIEW_HPP
#define CUSTOMWEBVIEW_HPP

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWidget>
#include <QVector>
#include <QUrl>
#include <QContextMenuEvent>

class customWebViewClass : public QWebEngineView
{
    Q_OBJECT
public:
    explicit customWebViewClass(QWidget* parent = nullptr);
    bool canGoBack();
    bool canGoForward();
public slots:
    void back();
    void forward();
protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    QWebEngineView* createWindow(QWebEnginePage::WebWindowType type) override;
private slots:
    void newUrlVisited(const QUrl& thisUrl);
private:
    QVector<QUrl> listOfVisitedUrl;
    int currentUrlID = -1;
    bool nextUrlChangeHasToBeSaved = true;
};

#endif
