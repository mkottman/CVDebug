#-------------------------------------------------
#
# Project created by QtCreator 2013-04-28T20:11:15
#
#-------------------------------------------------

QT       -= gui
CONFIG   += debug

TARGET = cvdebug
TEMPLATE = lib

DEFINES += CVDEBUG_LIBRARY

SOURCES += cvdebug.cpp
HEADERS += cvdebug.h

!win32 {
	LIBS += -lopencv_core -lzmq
	headers.path = /usr/local/include
	headers.files = $$HEADERS
	target.path = /usr/local/lib
	INSTALLS += target headers
}

win32:LIBPATH += C:/Work/opencv/build/x86/vc10/lib C:/work/zmq/lib
win32:INCLUDEPATH += C:/Work/opencv/build/include C:/work/zmq/include
win32:LIBS += opencv_core245.lib libzmq-v100-mt-3_2_3.lib

win32:QMAKE_CXXFLAGS_RELEASE += /Zi
win32:QMAKE_LFLAGS_RELEASE += /DEBUG

