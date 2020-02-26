#ifndef UKMEDIAOUTPUTWIDGET_H
#define UKMEDIAOUTPUTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QCoreApplication>
#include <QVBoxLayout>

class AudioSlider : public QSlider
{
    Q_OBJECT
public:
    AudioSlider(QWidget *parent = nullptr);
    ~AudioSlider();
    friend class UkmediaInputWidget;

protected:
    void mousePressEvent(QMouseEvent *ev)
    {
        //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
        QSlider::mousePressEvent(ev);
        //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
        double pos = ev->pos().x() / (double)width();
        setValue(pos *(maximum() - minimum()) + minimum());
        //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
        QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
        QCoreApplication::sendEvent(parentWidget(), &evEvent);
    }
};

class UkmediaOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaOutputWidget(QWidget *parent = nullptr);
    ~UkmediaOutputWidget();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QWidget *outputWidget;
    QWidget *outputDeviceWidget;
    QWidget *masterVolumeWidget;
    QWidget *channelBalanceWidget;
    QLabel *outputLabel;
    QLabel *outputDeviceLabel;
    QComboBox *outputDeviceCombobox;
    QLabel *opVolumeLabel;
    QPushButton *outputIconBtn;
    AudioSlider *opVolumeSlider;
    QLabel *opVolumePercentLabel;
    QLabel *opBalanceLabel;
    QLabel *leftBalanceLabel;
    AudioSlider *opBalanceSlider;
    QLabel *rightBalanceLabel;
};

#endif // UKMEDIAOUTPUTWIDGET_H
