#include "ukmediainputwidget.h"

UkmediaInputWidget::UkmediaInputWidget(QWidget *parent) : QWidget(parent)
{
    inputWidget = new QWidget(this);
    inputDeviceWidget = new QWidget(inputWidget);
    volumeWidget = new QWidget(inputWidget);
    inputLevelWidget = new QWidget(inputWidget);

    //设置大小
    inputDeviceWidget->setMinimumSize(550,50);
    inputDeviceWidget->setMaximumSize(960,50);
    volumeWidget->setMinimumSize(550,50);
    volumeWidget->setMaximumSize(960,50);
    inputLevelWidget->setMinimumSize(550,50);
    inputLevelWidget->setMaximumSize(960,50);

    inputLabel = new QLabel(tr("Input"),this);
    inputDeviceLabel = new QLabel(tr("Select input device"),inputWidget);
    inputDeviceCombobox = new QComboBox(inputDeviceWidget);
    ipVolumeLabel = new QLabel(tr("volume"),volumeWidget);
    inputIconBtn = new QPushButton(volumeWidget);
    ipVolumeSlider = new AudioSlider(volumeWidget);
    ipVolumePercentLabel = new QLabel(volumeWidget);
    inputLevelLabel = new QLabel(tr("Input level"),inputLevelWidget);
    lowLevelLabel = new QLabel(tr("low"),inputLevelWidget);
    inputLevelSlider = new AudioSlider(inputLevelWidget);
    highLevelLabel = new QLabel(tr("high"),inputLevelWidget);

    ipVolumeSlider->setOrientation(Qt::Horizontal);
    inputLevelSlider->setOrientation(Qt::Horizontal);
    ipVolumeSlider->setRange(0,100);
    inputIconBtn->setFocusPolicy(Qt::NoFocus);
    //输出设备添加布局
    QHBoxLayout *inputDeviceLayout = new QHBoxLayout(inputDeviceWidget);
    inputLabel->setFixedSize(83,24);
    inputDeviceCombobox->setMinimumSize(50,32);
    inputDeviceCombobox->setMaximumSize(900,32);
    QSpacerItem *item1 = new QSpacerItem(16,20,QSizePolicy::Fixed);
    QSpacerItem *item2 = new QSpacerItem(48,20,QSizePolicy::Fixed);
    inputDeviceLabel->setFixedSize(83,24);
    inputDeviceLayout->addItem(item1);
    inputDeviceLayout->addWidget(inputDeviceLabel);
    inputDeviceLayout->addItem(item2);
    inputDeviceLayout->addWidget(inputDeviceCombobox);
    inputDeviceLayout->addItem(item1);
    inputDeviceLayout->setSpacing(0);
    inputDeviceWidget->setLayout(inputDeviceLayout);
    inputDeviceLayout->layout()->setContentsMargins(0,0,0,0);
    //主音量添加布局
    QHBoxLayout *masterLayout = new QHBoxLayout(volumeWidget);
    ipVolumeLabel->setFixedSize(83,24);
    inputIconBtn->setFixedSize(24,24);
    ipVolumeSlider->setFixedHeight(22);
    ipVolumePercentLabel->setFixedSize(36,24);
    masterLayout->addItem(item1);
    masterLayout->addWidget(ipVolumeLabel);
    masterLayout->addItem(item2);
    masterLayout->addWidget(inputIconBtn);
    masterLayout->addItem(item1);
    masterLayout->addWidget(ipVolumeSlider);
    masterLayout->addItem(item1);
    masterLayout->addWidget(ipVolumePercentLabel);
    masterLayout->addItem(item1);
    masterLayout->setSpacing(0);
    volumeWidget->setLayout(masterLayout);
    volumeWidget->layout()->setContentsMargins(0,0,0,0);
    //声道平衡添加布局
    QHBoxLayout *soundLayout = new QHBoxLayout(inputLevelWidget);
    inputLevelLabel->setFixedSize(83,24);
    lowLevelLabel->setFixedSize(24,24);
    inputLevelSlider->setFixedHeight(22);
    highLevelLabel->setFixedSize(36,24);
    soundLayout->addItem(item1);
    soundLayout->addWidget(inputLevelLabel);
    soundLayout->addItem(item2);
    soundLayout->addWidget(lowLevelLabel);
    soundLayout->addItem(item1);
    soundLayout->addWidget(inputLevelSlider);
    soundLayout->addItem(item1);
    soundLayout->addWidget(highLevelLabel);
    soundLayout->addItem(item1);
    soundLayout->setSpacing(0);
    inputLevelWidget->setLayout(soundLayout);
    inputLevelWidget->layout()->setContentsMargins(0,0,0,0);
    //进行整体布局
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(inputLabel);
    vLayout->addItem(item1);
    vLayout->addWidget(inputDeviceWidget);
    vLayout->addWidget(volumeWidget);
    vLayout->addWidget(inputLevelWidget);
    this->setLayout(vLayout);
    vLayout->setSpacing(0);
    this->layout()->setContentsMargins(0,0,0,0);

    //设置样式
    ipVolumeSlider->setStyleSheet("QSlider::groove:horizontal {"
                                  "border: 0px solid #bbb; }"
                                  "QSlider::sub-page:horizontal {"
                                  "background: #2e8ad2;border-radius: 2px;"
                                  "margin-top:8px;margin-bottom:9px;}"
                                  "QSlider::add-page:horizontal {"
                                  "background:  rgba(52,70,80,90%);"
                                  "border: 0px solid #777;"
                                  "border-radius: 2px;"
                                  "margin-top:8px;"
                                  "margin-bottom:9px;}"
                                  "QSlider::handle:horizontal {"
                                  "width: 20px;"
                                  "height: 20px;"
                                  "background: rgb(61,107,229);"
                                  "border-radius:10px;}");
    inputLevelSlider->setStyleSheet("QSlider::groove:horizontal {"
                                   "border: 0px solid #bbb; }"
                                   "QSlider::sub-page:horizontal {"
                                   "background: #2e8ad2;border-radius: 2px;"
                                   "margin-top:8px;margin-bottom:9px;}"
                                   "QSlider::add-page:horizontal {"
                                   "background:  rgba(52,70,80,90%);"
                                   "border: 0px solid #777;"
                                   "border-radius: 2px;"
                                   "margin-top:8px;"
                                   "margin-bottom:9px;}"
                                   "QSlider::handle:horizontal {"
                                   "width: 20px;"
                                   "height: 20px;"
                                   "background: rgb(61,107,229);"
                                   "border-radius:10px;}");
    inputDeviceCombobox->setStyleSheet("QComboBox{"
                                       "background: rgba(255, 255, 255);"
                                       "border: 1px solid rgb(255, 255, 255);"
                                       "border-radius: 6px;"
                                       "font-size:12px;"
                                       "color: black;"
                                       "min-height: 40px;}"

                                       "QComboBox QAbstractItemView::item {"
                                       "min-height: 40px;"
                                       "border-radius: 2px ;"
                                       "color:black;"
                                       "background-color: rgba(255, 255, 255);}"
                                       "QComboBox::down-arrow{"
                                       "image:url(/usr/share/ukui-media/img/dowm.png);}"
                                       "QComboBox::drop-down{"
                                       "width: 30px;"
                                       "border: none;}"
                                       "QComboBox QListView::item:selected{"
                                       "background: #3D6BE5;}");
//    inputDeviceCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
//                                       "border:2px solid rgba(218, 227, 250, 1);border-radius:4px;}"
//                                       "QComboBox QAbstractItemView{"
//                                       "font-size: 14px;height: 80px;width: 140px;}"
//                                       "QComboBox QAbstractItemView::item {"
//                                       "height: 30px;}"
//                                       "QComboBox QAbstractItemView::item:hover {"
//                                       "background: rgba(218,227,250,0.9);"
//                                       "border-radius: 2px;"
//                                       "font-color: rgb(225, 0, 0);}"
//                                       "QComboBox::drop-down{background-color:rgba(248,248,248,1);}"
//                                       "QComboBox:on{background:linear-gradient(0deg,rgba(248,248,248,1) 0%,rgba(248,248,248,0.9) 100%);"
//                                       "border-radius:4px;}"
//                                       "QComboBox::down-arrow:hover{"
//                                       "width:184px;height:30px;"
//                                       "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                       "border-radius:2px;}"
//                                       "QComboBox::down-arrow {"
//                                       "image: url(/usr/share/ukui-media/img/dowm.png);"
//                                       "height:6px;width:11px;}"
//                                       "QComboBox::down-arrow:hover{"
//                                       "width:184px;height:30px;"
//                                       "background:linear-gradient(180deg,rgba(218,227,250,0.9),rgba(218,227,250,1));"
//                                       "border-radius:2px;}");

    inputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");
    this->setStyleSheet("QWidget{width: 552px;height: 150px;"
                        "background: rgba(244,244,244,1);"
                        "border-radius: 4px;}");
    inputDeviceWidget->setObjectName("inputDeviceWidget");
    volumeWidget->setObjectName("volumeWidget");
    inputDeviceWidget->setStyleSheet("QWidget#inputDeviceWidget{border-bottom:1px solid white;"
                                      "border-radius:0px;}");
    volumeWidget->setStyleSheet("QWidget#volumeWidget{border-bottom:1px solid white;"
                                      "border-radius:0px;}");
}

UkmediaInputWidget::~UkmediaInputWidget()
{

}
