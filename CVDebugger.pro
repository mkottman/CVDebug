#-------------------------------------------------
#
# Project created by QtCreator 2013-04-28T19:36:04
#
#-------------------------------------------------

QT       += core gui
CONFIG   += debug

TARGET = CvDebugger
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    receiver.cpp \
    imagedisplay.cpp \
    imagelabel.cpp

HEADERS  += mainwindow.h \
    imagedisplay.h \
    imagelabel.h \
    receiver.h

!win32 {
	LIBS += -L/usr/local/lib \
		-lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_features2d \
        -lzmq
}

win32 {
	LIBPATH += C:/Work/opencv/build/x86/vc10/lib C:/Work/zmq/lib
	INCLUDEPATH += C:/Work/opencv/build/include C:/Work/zmq/include
	LIBS += opencv_core245.lib opencv_imgproc245.lib opencv_highgui245.lib opencv_features2d245.lib libzmq-v100-mt-3_2_3.lib
}

FORMS    += \
    mainwindow.ui

RESOURCES += \
    images.qrc
