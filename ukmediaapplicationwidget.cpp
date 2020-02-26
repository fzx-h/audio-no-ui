#include "ukmediaapplicationwidget.h"
#include <QDebug>
UkmediaApplicationWidget::UkmediaApplicationWidget(QWidget *parent) : QWidget(parent)
{
    appWid = new QWidget(this);
    applicationLabel = new QLabel(tr("Application Volume"),this);
    noAppLabel = new QLabel(tr("No application is currently playing or recording audio"),appWid);
    gridlayout = new QGridLayout(appWid);
    app_volume_list = new QStringList;
    applicationLabel->setFixedSize(160,18);
    noAppLabel->setFixedSize(300,14);
    this->setMinimumSize(550,320);
    this->setMaximumSize(960,500);
    appWid->setMinimumSize(550,320);
    appWid->setMaximumSize(960,320);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(applicationLabel);
    vlayout->addWidget(appWid);
    vlayout->setSpacing(16);
    this->setLayout(vlayout);
    this->layout()->setContentsMargins(0,0,0,0);
   //设置样式
    applicationLabel->setStyleSheet("QLabel{background:transparent;"
                                          "border:0px;"
                                          "font-size:18px;}");
    noAppLabel->setStyleSheet("QLabel{width:210px;"
                              "height:14px;"
                              "font-size:14px;"
                              "line-height:28px;}");
}

UkmediaApplicationWidget::~UkmediaApplicationWidget()
{

}
