#ifndef SELECTSTICKERWINDOW_HPP
#define SELECTSTICKERWINDOW_HPP

#include <QDialog>
#include <QTextBrowser>

class selectStickerWindowClass : public QDialog
{
    Q_OBJECT
public:
    selectStickerWindowClass(QWidget* parent);
    void loadListOfStickers();
public slots:
    void linkClicked(const QUrl &link);
    void createContextMenu(const QPoint& thisPoint);
    void scrollBarSizeChanged();
signals:
    void addThisSticker(QString stringSticker);
private:
    QTextBrowser stickerBrowser;
};

#endif
