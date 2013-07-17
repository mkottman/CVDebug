#-------------------------------------------------
#
# Project created by QtCreator 2013-04-28T20:06:23
#
#-------------------------------------------------

QT       += core gui
CONFIG   += debug

TARGET = TestClient
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += \
    mainwindow.ui

LIBS += -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lzmq \
        -lcvdebug
