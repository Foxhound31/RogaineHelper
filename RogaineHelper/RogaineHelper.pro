#-------------------------------------------------
#
# Project created by QtCreator 2015-11-05T12:59:12
#
#-------------------------------------------------
# Path to opencv must be added to the OPENCV_DIR variable.

TEMPLATE = app

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets



# -----------------------------------
# Output directories and names
# -----------------------------------
TARGET = RogaineHelper

#Debug:  DESTDIR = ../bin_debug
#Release:DESTDIR = ../bin_release


# -----------------------------------
# Headers and sources
# -----------------------------------

SOURCES += \
            main.cpp \
            MainWindow.cpp \
            MapRecognizer.cpp \
            OpencvQtTools.cpp
HEADERS += \
            MainWindow.h \
            MapRecognizer.h \
            OpencvQtTools.h
FORMS += \
            MainWindow.ui


# -----------------------------------
# Libraries
# -----------------------------------
INCLUDEPATH += $$(OPENCV_DIR)/build/include
LIBS += -L$$(OPENCV_DIR)/build/x86/vc12/lib


Debug:  LIBS += opencv_ts300d.lib \
                opencv_world300d.lib

Release:LIBS += opencv_ts300.lib \
                opencv_world300.lib





