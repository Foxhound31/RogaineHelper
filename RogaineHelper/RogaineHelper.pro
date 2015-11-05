#-------------------------------------------------
#
# Project created by QtCreator 2015-11-05T12:59:12
#
#-------------------------------------------------
# Path to opencv must be added to the OPENCV_DIR variable.

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RogaineHelper
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp \
    MapRecognaizer.cpp

HEADERS  += MainWindow.h \
    MapRecognaizer.h

FORMS    += MainWindow.ui

INCLUDEPATH += $$(OPENCV_DIR)/build/include

LIBS += -L$$(OPENCV_DIR)/build/x64/vc12/lib

CONFIG(debug, debug|release) {
    DESTDIR = ../bin_debug
    LIBS += opencv_ts300d.lib \
        opencv_world300d.lib
} else {
    DESTDIR = ../bin_release
    LIBS += opencv_ts300.lib \
    opencv_world300.lib
}
