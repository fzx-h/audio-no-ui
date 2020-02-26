#include "ukmediasoundeffectswidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
UkmediaSoundEffectsWidget::UkmediaSoundEffectsWidget(QWidget *parent) : QWidget(parent)
{
    themeWidget = new QWidget(this);
    shutDownWidget = new QWidget(this);
    lagoutWidget = new QWidget(this);

    //设置大小
    themeWidget->setMinimumSize(550,50);
    themeWidget->setMaximumSize(960,50);
    shutDownWidget->setMinimumSize(550,50);
    shutDownWidget->setMaximumSize(960,50);
    lagoutWidget->setMinimumSize(550,50);
    lagoutWidget->setMaximumSize(960,50);

    soundEffectLabel = new QLabel(tr("System sound"),this);
    soundThemeLabel = new QLabel(tr("System sound theme"),themeWidget);
    soundThemeCombobox = new QComboBox(themeWidget);
    shutdownlabel = new QLabel(tr("Shutdown"),shutDownWidget);
    shutdownCombobox = new QComboBox(shutDownWidget);
    lagoutLabel = new QLabel(tr("Lagout"),lagoutWidget);
    lagoutCombobox = new QComboBox(lagoutWidget);

    soundEffectLabel->setFixedSize(83,24);
    soundThemeLabel->setFixedSize(83,24);
    shutdownlabel->setFixedSize(83,24);
    lagoutLabel->setFixedSize(83,24);

    soundThemeCombobox->setMinimumSize(50,32);
    soundThemeCombobox->setMaximumSize(900,32);

    shutdownCombobox->setMinimumSize(50,32);
    shutdownCombobox->setMaximumSize(900,32);
    lagoutCombobox->setMinimumSize(50,32);
    lagoutCombobox->setMaximumSize(900,32);
    QSpacerItem *item1 = new QSpacerItem(16,20,QSizePolicy::Fixed);
    QSpacerItem *item2 = new QSpacerItem(48,20,QSizePolicy::Fixed);
    //添加布局
    QHBoxLayout *themeLayout = new QHBoxLayout(themeWidget);
    themeLayout->addItem(item1);
    themeLayout->addWidget(soundThemeLabel);
    themeLayout->addItem(item2);
    themeLayout->addWidget(soundThemeCombobox);
    themeLayout->addItem(item1);
    themeLayout->setSpacing(0);
    themeWidget->setLayout(themeLayout);
    themeWidget->layout()->setContentsMargins(0,0,0,0);

    QHBoxLayout *shutdownLayout = new QHBoxLayout(shutDownWidget);
    shutdownLayout->addItem(item1);
    shutdownLayout->addWidget(shutdownlabel);
    shutdownLayout->addItem(item2);
    shutdownLayout->addWidget(shutdownCombobox);
    shutdownLayout->addItem(item1);
    shutdownLayout->setSpacing(0);
    shutDownWidget->setLayout(shutdownLayout);
    shutDownWidget->layout()->setContentsMargins(0,0,0,0);

    QHBoxLayout *lagoutLayout = new QHBoxLayout(lagoutWidget);
    lagoutLayout->addItem(item1);
    lagoutLayout->addWidget(lagoutLabel);
    lagoutLayout->addItem(item2);
    lagoutLayout->addWidget(lagoutCombobox);
    lagoutLayout->addItem(item1);
    lagoutLayout->setSpacing(0);
    lagoutWidget->setLayout(lagoutLayout);
    lagoutWidget->layout()->setContentsMargins(0,0,0,0);

    //进行整体布局
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(soundEffectLabel);
    vLayout->addItem(item1);
    vLayout->addWidget(themeWidget);
    vLayout->addItem(item1);
    vLayout->addWidget(shutDownWidget);
    vLayout->addWidget(lagoutWidget);
    this->setLayout(vLayout);
    vLayout->setSpacing(0);
    this->layout()->setContentsMargins(0,0,0,0);

    shutDownWidget->setObjectName("shutDownWidget");
    soundThemeCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
                                      "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
                                      "QComboBox QAbstractItemView{"
                                      "font-size: 14px;height: 80px;width: 140px;}"
                                      "QComboBox QAbstractItemView::item {"
                                      "height: 30px;}"
                                      "QComboBox QAbstractItemView::item:hover {"
                                      "background: rgba(218,227,250,0.9);"
                                      "border-radius: 2px;"
                                      "font-color: rgb(225, 0, 0);}"
                                      "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
                                      "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
                                      "border-radius:4px;}"
                                      "QComboBox::down-arrow:hover{"
                                      "width:184px;height:30px;"
                                      "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                      "border-radius:2px;}"
                                      "QComboBox::down-arrow {"
                                      "image: url(/usr/share/ukui-media/img/dowm.png);"
                                      "height:6px;width:11px;}"
                                      "QComboBox::down-arrow:hover{"
                                      "width:184px;height:30px;"
                                      "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                      "border-radius:2px;}");
    shutdownCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
                                    "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
                                    "QComboBox QAbstractItemView{"
                                    "font-size: 14px;height: 80px;width: 140px;}"
                                    "QComboBox QAbstractItemView::item {"
                                    "height: 30px;}"
                                    "QComboBox QAbstractItemView::item:hover {"
                                    "background: rgba(218,227,250,0.9);"
                                    "border-radius: 2px;"
                                    "font-color: rgb(225, 0, 0);}"
                                    "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
                                    "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
                                    "border-radius:4px;}"
                                    "QComboBox::down-arrow:hover{"
                                    "width:184px;height:30px;"
                                    "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                    "border-radius:2px;}"
                                    "QComboBox::down-arrow {"
                                    "image: url(/usr/share/ukui-media/img/dowm.png);"
                                    "height:6px;width:11px;}"
                                    "QComboBox::down-arrow:hover{"
                                    "width:184px;height:30px;"
                                    "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                    "border-radius:2px;}");

    lagoutCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
                                  "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
                                  "QComboBox QAbstractItemView{"
                                  "font-size: 14px;height: 80px;width: 140px;}"
                                  "QComboBox QAbstractItemView::item {"
                                  "height: 30px;}"
                                  "QComboBox QAbstractItemView::item:hover {"
                                  "background: rgba(218,227,250,0.9);"
                                  "border-radius: 2px;"
                                  "font-color: rgb(225, 0, 0);}"
                                  "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
                                  "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
                                  "border-radius:4px;}"
                                  "QComboBox::down-arrow:hover{"
                                  "width:184px;height:30px;"
                                  "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                  "border-radius:2px;}"
                                  "QComboBox::down-arrow {"
                                  "image: url(/usr/share/ukui-media/img/dowm.png);"
                                  "height:6px;width:11px;}"
                                  "QComboBox::down-arrow:hover{"
                                  "width:184px;height:30px;"
                                  "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
                                  "border-radius:2px;}");;
    this->setStyleSheet("QWidget{width: 552px;height: 150px;"
                        "background: rgba(244,244,244,1);"
                        "border-radius: 4px;}");
    shutDownWidget->setStyleSheet("QWidget#shutDownWidget{border-bottom:1px solid white;"
                                      "border-radius:0px;}");


}

UkmediaSoundEffectsWidget::~UkmediaSoundEffectsWidget()
{

}
