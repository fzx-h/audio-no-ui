#ifndef UKMEDIASOUNDEFFECTSWIDGET_H
#define UKMEDIASOUNDEFFECTSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>

class UkmediaSoundEffectsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UkmediaSoundEffectsWidget(QWidget *parent = nullptr);
    ~UkmediaSoundEffectsWidget();
    friend class UkmediaMainWidget;
Q_SIGNALS:

public Q_SLOTS:

private:
    QWidget *themeWidget;
    QWidget *shutDownWidget;
    QWidget *lagoutWidget;

    QLabel *soundEffectLabel;
    QLabel *soundThemeLabel;
    QComboBox *soundThemeCombobox;
    QLabel *shutdownlabel;
    QComboBox *shutdownCombobox;
    QLabel *lagoutLabel;
    QComboBox *lagoutCombobox;
};

#endif // UKMEDIASOUNDEFFECTSWIDGET_H
