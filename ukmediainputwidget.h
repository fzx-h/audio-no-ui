#ifndef UKMEDIAINPUTWIDGET_H
#define UKMEDIAINPUTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include "ukmediaoutputwidget.h"

class UkmediaInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaInputWidget(QWidget *parent = nullptr);
    ~UkmediaInputWidget();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QWidget *inputWidget;
    QWidget *inputDeviceWidget;
    QWidget *volumeWidget;
    QWidget *inputLevelWidget;
    QLabel *inputLabel;
    QLabel *inputDeviceLabel;
    QLabel *ipVolumeLabel;
    QLabel *inputLevelLabel;
    QLabel *lowLevelLabel;
    QLabel *highLevelLabel;
    QLabel *ipVolumePercentLabel;
    QPushButton *inputIconBtn;
    AudioSlider *ipVolumeSlider;
    AudioSlider *inputLevelSlider;
    QComboBox *inputDeviceCombobox;
};

#endif // UKMEDIAINPUTWIDGET_H
