#ifndef SELECTSTICKERWINDOW_HPP
#define SELECTSTICKERWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QTextBrowser>
#include <QUrl>
#include <QPoint>
#include <QString>

class selectStickerWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit selectStickerWindowClass(QWidget* parent);
private:
    void loadListOfStickers();
private slots:
    void linkClicked(const QUrl& link);
    void createContextMenu(const QPoint& thisPoint);
    void scrollBarSizeChanged();
signals:
    void addThisSticker(QString stringSticker);
private:
    QTextBrowser* stickerBrowser;
};

#endif
