#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QScrollBar>
#include <QImage>
#include <QPixmap>
#include <QPalette>
#include <QLabel>
#include <QCheckBox>

#include "selectStickerWindow.hpp"
#include "settingTool.hpp"

namespace
{
    QVector<QString> listOfStickerTypeContent;
}

selectStickerWindowClass::selectStickerWindowClass(QWidget* parent) : baseDialogClass(parent)
{
    stickerBrowser = new QTextBrowser(this);

    stickerBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    stickerBrowser->setReadOnly(true);
    stickerBrowser->setOpenExternalLinks(false);
    stickerBrowser->setOpenLinks(false);
    stickerBrowser->setSearchPaths(QStringList(QCoreApplication::applicationDirPath()));
    stickerBrowser->setMinimumWidth(450);
    stickerBrowser->setMinimumHeight(480);

    stickerTypeListscrollArea = new QScrollArea(this);
    QWidget* mainWidgetOfStickerTypeList = new QWidget(stickerTypeListscrollArea);
    QVBoxLayout* layoutOfStickerTypeList = new QVBoxLayout(mainWidgetOfStickerTypeList);

    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("sticker.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/zuc-fr.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1jnd.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1kgu.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1kkh.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1kkr.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1kl6.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1lgg.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1ljl.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1lm9.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1nub.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1ltd.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1n1p-fr.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1n2c.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1o33.png", mainWidgetOfStickerTypeList));
    layoutOfStickerTypeList->addWidget(createQLabelForStickerTypeWithThesesInfos("stickers/1ptd.png", mainWidgetOfStickerTypeList));
    listOfStickerTypeContent.resize(listOfLabels.size());

    layoutOfStickerTypeList->setMargin(1);
    layoutOfStickerTypeList->setSpacing(0);
    mainWidgetOfStickerTypeList->setObjectName("stickerTypeList");
    mainWidgetOfStickerTypeList->setLayout(layoutOfStickerTypeList);
    stickerTypeListscrollArea->setFixedWidth(80);
    stickerTypeListscrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    stickerTypeListscrollArea->setWidget(mainWidgetOfStickerTypeList);

    QHBoxLayout* stickerLayout = new QHBoxLayout();
    stickerLayout->addWidget(stickerBrowser, 1);
    stickerLayout->addWidget(stickerTypeListscrollArea);
    stickerLayout->setMargin(0);
    stickerLayout->setSpacing(0);

    QCheckBox* saveLastStickerTypeUsedCheckbox = new QCheckBox("Sauvegarder le dernier type de sticker utilisé", this);
    QLabel* stickerInfoLabel = new QLabel("(clique droit pour fermer la fenêtre)", this);

    saveLastStickerTypeUsedCheckbox->setChecked(settingTool::getThisBoolOption("saveLastStickerTypeUsed"));

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(saveLastStickerTypeUsedCheckbox);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(stickerInfoLabel);
    bottomLayout->setMargin(5);

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(stickerLayout, 1);
    mainLayout->addLayout(bottomLayout);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    if(saveLastStickerTypeUsedCheckbox->isChecked() == true)
    {
        oldLabelSelected = settingTool::getThisIntOption("lastTypeOfStickerUsed").value;

        if(oldLabelSelected < 0)
        {
            oldLabelSelected = 0;
        }
        else if(oldLabelSelected >= listOfLabels.size())
        {
            oldLabelSelected = listOfLabels.size() - 1;
        }
    }

    setLayout(mainLayout);
    setWindowTitle("Choisir un sticker");

    connect(stickerBrowser, &QTextBrowser::customContextMenuRequested, this, &selectStickerWindowClass::createContextMenu);
    connect(stickerBrowser, &QTextBrowser::anchorClicked, this, &selectStickerWindowClass::linkClicked);
    connect(saveLastStickerTypeUsedCheckbox, &QCheckBox::toggled, this, &selectStickerWindowClass::saveLastStickerTypeUsedValueChanged);
}

void selectStickerWindowClass::showEvent(QShowEvent* event)
{
    baseDialogClass::showEvent(event);

    if(firstTimeShowed == true)
    {
        labelClicked(Qt::LeftButton, oldLabelSelected);
        firstTimeShowed = false;
    }
}

clickableLabelClass* selectStickerWindowClass::createQLabelForStickerTypeWithThesesInfos(QString imageName, QWidget* parent)
{
    QImage image;
    QPalette palette;
    clickableLabelClass* imageView = new clickableLabelClass(listOfLabels.size(), parent);

    image.load(QCoreApplication::applicationDirPath() + "/resources/" + imageName);
    image = image.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    palette.setColor(QPalette::Background, Qt::transparent);
    imageView->setPalette(palette);
    imageView->setAutoFillBackground(true);
    imageView->setMargin(3);
    imageView->setPixmap(QPixmap::fromImage(image));
    listOfLabels.append(imageView);

    connect(imageView, &clickableLabelClass::clicked, this, &selectStickerWindowClass::labelClicked);

    return imageView;
}

void selectStickerWindowClass::loadAndUseListOfStickers(int stickerType)
{
    if(listOfStickerTypeContent.at(stickerType).isEmpty() == true)
    {
        switch(stickerType)
        {
            case 0:
            {
                QDir stickerDir(QCoreApplication::applicationDirPath() + "/resources/stickers/");
                QStringList listOfStickers;

                if(stickerDir.exists() == true)
                {
                    listOfStickers = stickerDir.entryList(QDir::Files);
                }

                for(const QString& thisSticker : listOfStickers)
                {
                    generateAndInsertStickerCode(thisSticker, listOfStickerTypeContent[stickerType]);
                }
                break;
            }
            case 1:
            {
                //ours
                generateAndInsertStickerCode("1f88.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1f89.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1f8a.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1f8b.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1f8c.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1f8d.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1f8e.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1f8f.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zu2.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zu6.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zu7.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zu8.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zu9.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zua.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zub.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zuc-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zuc-en.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("zuc-es.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 2:
            {
                //bourge
                generateAndInsertStickerCode("1jnc.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1jnd.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1jne.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1jnf.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1jng.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1jnh.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1jni.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1jnj.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 3:
            {
                //lama
                generateAndInsertStickerCode("1kgu.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kgv.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kgw.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kgx.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kgy.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kgz.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kh0.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kh1.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 4:
            {
                //hap
                generateAndInsertStickerCode("1kkg.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkh.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kki.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkj.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkk.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkl.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkm.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkn.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 5:
            {
                //noel
                generateAndInsertStickerCode("1kko.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkp.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkq.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kks.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkt.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kku.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkv.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 6:
            {
                //chat
                generateAndInsertStickerCode("1kky.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kkz.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl0.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl1.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl2.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl3.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl4.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl5.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl6.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl7.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl8.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kl9.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1kla.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1klb.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 7:
            {
                //orc
                generateAndInsertStickerCode("1lga.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lgb.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lgc.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lgd.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lge.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lgf.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lgg.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lgh.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 8:
            {
                //dom
                generateAndInsertStickerCode("1ljj.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ljl.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ljm.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ljn.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ljo.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ljp.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ljq.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ljr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rzs.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rzt.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rzu.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rzv.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rzw.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 9:
            {
                //aventurier
                generateAndInsertStickerCode("1lm9.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lma.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmb.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmc.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmd.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lme.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmf.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmg.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 10:
            {
                //saumon
                generateAndInsertStickerCode("1lmh.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmi.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmj.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmk.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lml.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmm.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmn.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmo.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lmp.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1mqv.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1mqw.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1mqx.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1mqy.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1mqz.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1mr0.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1mr1.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1nu6.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1nu7.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1nu8.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1nu9.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1nua.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1nub.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 11:
            {
                //bureau
                generateAndInsertStickerCode("1lt7.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lt8.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lt9.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lta.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ltb.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ltc.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ltd.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1lte.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 12:
            {
                //foot
                generateAndInsertStickerCode("1n1m-de.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1m-es.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1m-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1m-it.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1n-de.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1n-es.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1n-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1n-it.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1o-de.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1o-es.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1o-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1o-it.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1p-de.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1p-es.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1p-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1p-it.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1q-de.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1q-es.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1q-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1q-it.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1r-de.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1r-es.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1r-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1r-it.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1s.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1t-de.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1t-es.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1t-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n1t-it.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 13:
            {
                //store
                generateAndInsertStickerCode("1n2c.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2d.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2g.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2h.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2i.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2j.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2k.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2l.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2m.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2n.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1n2o.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 14:
            {
                //pixel
                generateAndInsertStickerCode("1o2k.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1o33.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1o3f.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1o3g.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1o3i.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1o3k.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1o66.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1o67.png", listOfStickerTypeContent[stickerType]);
                break;
            }
            case 15:
            {
                //gym
                generateAndInsertStickerCode("1ptd.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rob.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1ron.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpa.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpp.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpp-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpt.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpw.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpw-fr.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpy.png", listOfStickerTypeContent[stickerType]);
                generateAndInsertStickerCode("1rpy-fr.png", listOfStickerTypeContent[stickerType]);
                break;
            }
        }
    }

    stickerBrowser->setHtml(listOfStickerTypeContent.at(stickerType));
    stickerBrowser->verticalScrollBar()->setValue(stickerBrowser->verticalScrollBar()->maximum() / 2);
}

void selectStickerWindowClass::generateAndInsertStickerCode(QString stickerName, QString& whereToInsert)
{
    whereToInsert.append("<a href=\"sticker:" + stickerName.left(stickerName.size() - 4) +
                         "\"><img src=\"resources/stickers/" + stickerName + "\" /></a>");
}

void selectStickerWindowClass::linkClicked(const QUrl& link)
{
    QString linkInString = link.toDisplayString();

    emit addThisSticker("[[sticker:p/" + linkInString.remove(0, linkInString.indexOf(':') + 1) + "]]");
}

void selectStickerWindowClass::createContextMenu(const QPoint& thisPoint)
{
    (void)thisPoint;
    close();
}

void selectStickerWindowClass::labelClicked(Qt::MouseButton buttonClicked, int labelID)
{
    if(buttonClicked == Qt::LeftButton && labelID >= 0 && labelID < listOfLabels.size())
    {
        QPalette tmpPalette;

        tmpPalette.setColor(QPalette::Background, Qt::transparent);
        listOfLabels.at(oldLabelSelected)->setPalette(tmpPalette);

        tmpPalette.setColor(QPalette::Background, Qt::darkGray);
        listOfLabels.at(labelID)->setPalette(tmpPalette);
        stickerTypeListscrollArea->ensureWidgetVisible(listOfLabels.at(labelID), 0, 0);
        loadAndUseListOfStickers(labelID);
        oldLabelSelected = labelID;
        settingTool::saveThisOption("lastTypeOfStickerUsed", oldLabelSelected);
    }
    else if(buttonClicked == Qt::RightButton)
    {
        close();
    }
}

void selectStickerWindowClass::saveLastStickerTypeUsedValueChanged(bool newVal)
{
    settingTool::saveThisOption("saveLastStickerTypeUsed", newVal);
}
