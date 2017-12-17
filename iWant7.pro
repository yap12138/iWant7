#-------------------------------------------------
#
# Project created by QtCreator 2016-11-11T23:05:11
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iWant7
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    Block.cpp \
    Game.cpp \
    Setting.cpp \
    GameArea.cpp

HEADERS  += widget.h \
    Block.h \
    Game.h \
    Setting.h \
    GameArea.h

FORMS    += widget.ui \
    Setting.ui

RC_FILE = myico.rc

RESOURCES += \
    resource.qrc
