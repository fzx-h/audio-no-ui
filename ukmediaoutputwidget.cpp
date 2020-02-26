#include "ukmediaoutputwidget.h"
#include <QHBoxLayout>
AudioSlider::AudioSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

AudioSlider::~AudioSlider()
{

}

UkmediaOutputWidget::UkmediaOutputWidget(QWidget *parent) : QWidget(parent)
{
    outputWidget = new QWidget(this);
    outputDeviceWidget = new QWidget(outputWidget);
    masterVolumeWidget = new QWidget(outputWidget);
    channelBalanceWidget = new QWidget(outputWidget);

    //设置大小
    outputDeviceWidget->setMinimumSize(550,50);
    outputDeviceWidget->setMaximumSize(960,50);
    masterVolumeWidget->setMinimumSize(550,50);
    masterVolumeWidget->setMaximumSize(960,50);
    channelBalanceWidget->setMinimumSize(550,50);
    channelBalanceWidget->setMaximumSize(960,50);

    outputLabel = new QLabel(tr("Output"),this);
    outputDeviceLabel = new QLabel(tr("Select output device"),outputWidget);
    outputDeviceCombobox = new QComboBox(outputDeviceWidget);
    opVolumeLabel = new QLabel(tr("Master volume"),masterVolumeWidget);
    outputIconBtn = new QPushButton(masterVolumeWidget);
    opVolumeSlider = new AudioSlider(masterVolumeWidget);
    opVolumePercentLabel = new QLabel(masterVolumeWidget);
    opBalanceLabel = new QLabel(tr("Channel balance"),channelBalanceWidget);
    leftBalanceLabel = new QLabel(tr("Left"),channelBalanceWidget);
    opBalanceSlider = new AudioSlider(channelBalanceWidget);
    rightBalanceLabel = new QLabel(tr("right"),channelBalanceWidget);

    opVolumeSlider->setOrientation(Qt::Horizontal);
    opBalanceSlider->setOrientation(Qt::Horizontal);
    opVolumeSlider->setRange(0,100);
    outputIconBtn->setFocusPolicy(Qt::NoFocus);
    //输出设备添加布局
    QHBoxLayout *outputDeviceLayout = new QHBoxLayout(outputDeviceWidget);
    outputLabel->setFixedSize(83,24);
    outputDeviceCombobox->setMinimumSize(50,32);
    outputDeviceCombobox->setMaximumSize(900,32);
    QSpacerItem *item1 = new QSpacerItem(16,20,QSizePolicy::Fixed);
    QSpacerItem *item2 = new QSpacerItem(48,20,QSizePolicy::Fixed);
    outputDeviceLabel->setFixedSize(83,24);
    outputDeviceLayout->addItem(item1);
    outputDeviceLayout->addWidget(outputDeviceLabel);
    outputDeviceLayout->addItem(item2);
    outputDeviceLayout->addWidget(outputDeviceCombobox);
    outputDeviceLayout->addItem(item1);
    outputDeviceLayout->setSpacing(0);
    outputDeviceWidget->setLayout(outputDeviceLayout);
    outputDeviceLayout->layout()->setContentsMargins(0,0,0,0);
    //主音量添加布局
    QHBoxLayout *masterLayout = new QHBoxLayout(masterVolumeWidget);
    opVolumeLabel->setFixedSize(83,24);
    outputIconBtn->setFixedSize(24,24);
    opVolumeSlider->setFixedHeight(22);
    opVolumePercentLabel->setFixedSize(36,24);
    masterLayout->addItem(item1);
    masterLayout->addWidget(opVolumeLabel);
    masterLayout->addItem(item2);
    masterLayout->addWidget(outputIconBtn);
    masterLayout->addItem(item1);
    masterLayout->addWidget(opVolumeSlider);
    masterLayout->addItem(item1);
    masterLayout->addWidget(opVolumePercentLabel);
    masterLayout->addItem(item1);
    masterLayout->setSpacing(0);
    masterVolumeWidget->setLayout(masterLayout);
    masterVolumeWidget->layout()->setContentsMargins(0,0,0,0);
    //声道平衡添加布局
    QHBoxLayout *soundLayout = new QHBoxLayout(channelBalanceWidget);
    opBalanceLabel->setFixedSize(83,24);
    leftBalanceLabel->setFixedSize(24,24);
    opBalanceSlider->setFixedHeight(22);
    rightBalanceLabel->setFixedSize(36,24);
    soundLayout->addItem(item1);
    soundLayout->addWidget(opBalanceLabel);
    soundLayout->addItem(item2);
    soundLayout->addWidget(leftBalanceLabel);
    soundLayout->addItem(item1);
    soundLayout->addWidget(opBalanceSlider);
    soundLayout->addItem(item1);
    soundLayout->addWidget(rightBalanceLabel);
    soundLayout->addItem(item1);
    soundLayout->setSpacing(0);
    channelBalanceWidget->setLayout(soundLayout);
    channelBalanceWidget->layout()->setContentsMargins(0,0,0,0);
    //进行整体布局
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->addWidget(outputLabel);
    vLayout->addItem(item1);
    vLayout->addWidget(outputDeviceWidget);
    vLayout->addWidget(masterVolumeWidget);
    vLayout->addWidget(channelBalanceWidget);
    this->setLayout(vLayout);
    vLayout->setSpacing(0);
    this->layout()->setContentsMargins(0,0,0,0);

    outputDeviceWidget->setObjectName("outputDeviceWidget");
    masterVolumeWidget->setObjectName("masterVolumeWidget");
    //设置样式
    opVolumeSlider->setStyleSheet("QSlider::groove:horizontal {"
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
    opBalanceSlider->setStyleSheet("QSlider::groove:horizontal {"
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
    outputDeviceCombobox->setStyleSheet("QComboBox {width:140px;height:30px;background:rgba(248,248,248,1);"
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
    outputIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");
    this->setStyleSheet("QWidget{width: 552px;height: 150px;"
                        "background: rgba(244,244,244,1);"
                        "border-radius: 4px;}");
    outputDeviceWidget->setStyleSheet("QWidget#outputDeviceWidget{border-bottom:1px solid white;"
                                      "border-radius:0px;}");
    masterVolumeWidget->setStyleSheet("QWidget#masterVolumeWidget{border-bottom:1px solid white;"
                                      "border-radius:0px;}");
}

UkmediaOutputWidget::~UkmediaOutputWidget()
{

}
