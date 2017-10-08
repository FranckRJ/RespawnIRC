#ifndef SELECTSTICKERWINDOW_HPP
#define SELECTSTICKERWINDOW_HPP

#include <QWidget>
#include <QDialog>
#include <QTextBrowser>
#include <QUrl>
#include <QPoint>
#include <QString>
#include <QVector>
#include <QScrollArea>

#include <clickableLabel.hpp>

class selectStickerWindowClass : public QDialog
{
    Q_OBJECT
public:
    explicit selectStickerWindowClass(QWidget* parent);
protected:
    void showEvent(QShowEvent* event) override;
private:
    clickableLabelClass* createQLabelForStickerTypeWithThesesInfos(QString imageName, QWidget* parent);
    void loadAndUseListOfStickers(int stickerType);
    void generateAndInsertStickerCode(QString stickerName, QString& whereToInsert);
private slots:
    void linkClicked(const QUrl& link);
    void createContextMenu(const QPoint& thisPoint);
    void labelClicked(Qt::MouseButton buttonClicked, int labelID);
    void saveLastStickerTypeUsedValueChanged(bool newVal);
signals:
    void addThisSticker(QString stringSticker);
private:
    QTextBrowser* stickerBrowser;
    QScrollArea* stickerTypeListscrollArea;
    QVector<clickableLabelClass*> listOfLabels;
    int oldLabelSelected = 1;
    bool firstTimeShowed = true;
};

#endif
