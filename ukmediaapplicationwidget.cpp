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
    applicationLabel->move(0,0);
    noAppLabel->move(60,153);
    this->setMinimumSize(550,320);
    this->setMaximumSize(960,500);

    appWid->setMinimumSize(550,320);
    appWid->setMaximumSize(960,320);

    appWid->move(0,34);
   //设置样式
    applicationLabel->setStyleSheet("QLabel{background:transparent;"
                                          "border:0px;"
                                          "color:#ffffff;"
                                          "font-size:18px;}");
    noAppLabel->setStyleSheet("QLabel{width:210px;"
                              "height:14px;"
                              "font-size:14px;"
                              "color:rgba(255,255,255,0.57);"
                              "line-height:28px;}");
}

UkmediaApplicationWidget::~UkmediaApplicationWidget()
{

}
