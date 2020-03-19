#include "ukmedia_main_widget.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //加载qm翻译文件o
    QString locale = QLocale::system().name();
    QTranslator translator;
    if (locale == "zh_CN") {
        if (translator.load("/usr/share/ukui-media/translations/audio-zh_CN.qm")) {
            app.installTranslator(&translator);
        }
        else {
            qDebug() << "Load translations file" << locale << "failed!";
        }
    }
    UkmediaMainWidget w;
    w.show();
    return app.exec();
}
