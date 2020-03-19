#-------------------------------------------------
#
# Project created by QtCreator 2020-02-24T09:53:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = audio
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 \
          no_keywords link_pkgconfig

PKGCONFIG += libmatemixer \
             Qt5Xdg \
             gio-2.0 \
             libxml-2.0 \
             Qt5Multimedia

SOURCES += \
        main.cpp \
    ukmedia_application_widget.cpp \
    ukmedia_input_widget.cpp \
    ukmedia_main_widget.cpp \
    ukmedia_output_widget.cpp \
    ukmedia_sound_effects_widget.cpp

HEADERS += \
    ukmedia_application_widget.h \
    ukmedia_input_widget.h \
    ukmedia_main_widget.h \
    ukmedia_output_widget.h \
    ukmedia_sound_effects_widget.h

TRANSLATIONS += translations/audio-zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
