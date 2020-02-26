#ifndef UKMEDIAAPPLICATIONWIDGET_H
#define UKMEDIAAPPLICATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "ukmediaoutputwidget.h"
class UkmediaApplicationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaApplicationWidget(QWidget *parent = nullptr);
    ~UkmediaApplicationWidget();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QLabel *applicationLabel;
    QLabel *noAppLabel;

    QWidget *appWid;
    QStringList *app_volume_list;
    QLabel *appLabel;
    QLabel *appIconLabel;
    QPushButton *appIconBtn;
    QLabel *appVolumeLabel;
    AudioSlider *appSlider;
    QGridLayout *gridlayout;
};

#endif // UKMEDIAAPPLICATIONWIDGET_H
