#include <QHBoxLayout>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QScrollBar>

#include "selectStickerWindow.hpp"

selectStickerWindowClass::selectStickerWindowClass(QWidget* parent) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute(Qt::WA_DeleteOnClose);

    stickerBrowser.setContextMenuPolicy(Qt::CustomContextMenu);
    stickerBrowser.setReadOnly(true);
    stickerBrowser.setOpenExternalLinks(false);
    stickerBrowser.setOpenLinks(false);
    stickerBrowser.setSearchPaths(QStringList(QCoreApplication::applicationDirPath()));
    stickerBrowser.setMinimumWidth(450);
    stickerBrowser.setMinimumHeight(430);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(&stickerBrowser);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setWindowTitle("Choisir un sticker");

    loadListOfStickers();

    connect(&stickerBrowser, &QTextBrowser::customContextMenuRequested, this, &selectStickerWindowClass::createContextMenu);
    connect(&stickerBrowser, &QTextBrowser::anchorClicked, this, &selectStickerWindowClass::linkClicked);
    connect(stickerBrowser.verticalScrollBar(), &QScrollBar::valueChanged, this, &selectStickerWindowClass::scrollBarSizeChanged);
}

void selectStickerWindowClass::loadListOfStickers()
{
    QDir stickerDir(QCoreApplication::applicationDirPath() + "/resources/stickers/");
    QStringList listOfStickers;

    if(stickerDir.exists() == true)
    {
        listOfStickers = stickerDir.entryList(QDir::Files);
    }

    for(const QString& thisSticker : listOfStickers)
    {
        stickerBrowser.insertHtml("<a href=\"sticker:" + thisSticker.left(thisSticker.size() - 4) +
                                  "\"><img src=\"resources/stickers/" + thisSticker + "\" /></a>");
    }
}

void selectStickerWindowClass::linkClicked(const QUrl &link)
{
    QString linkInString = link.toDisplayString();

    emit addThisSticker("[[sticker:p/" + linkInString.remove(0, linkInString.indexOf(':') + 1) + "]]");
}

void selectStickerWindowClass::createContextMenu(const QPoint &thisPoint)
{
    (void)thisPoint;
    close();
}

void selectStickerWindowClass::scrollBarSizeChanged()
{
    stickerBrowser.verticalScrollBar()->setValue(stickerBrowser.verticalScrollBar()->maximum() / 2);

    disconnect(stickerBrowser.verticalScrollBar(), &QScrollBar::valueChanged, this, &selectStickerWindowClass::scrollBarSizeChanged);
}
