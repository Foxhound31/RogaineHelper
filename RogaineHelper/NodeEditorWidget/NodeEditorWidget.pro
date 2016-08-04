#-------------------------------------------------
#
# Project created by QtCreator 2015-12-08T13:42:05
#
#-------------------------------------------------


TEMPLATE = lib

CONFIG += staticlib

QT     += core gui widgets


# -----------------------------------
# Output directories and names
# -----------------------------------
# Set library name using "d" for debug configuration
Debug:  TARGET = NodeEditorWidgetd
Release:TARGET = NodeEditorWidget

# And specify where to put the target file
# $$PWD - full path to the directory where the project file (.pro )is located
DESTDIR = $$PWD/bin


# Some magic for unix
# Copy output file to target directory,
# but to actually make the file copy, you will have to execute make install
unix {
    Debug:  target.file = NodeEditorWidgetd
    Release:target.file = NodeEditorWidget

    target.path = /usr/lib
    INSTALLS += target
}





# -----------------------------------
# Headers and sources
# -----------------------------------
HEADERS += inc/NodeEditorWidget.h \
           inc/NodeEditorScene.h \
           inc/NodeItem.h \
           inc/NodeConnection.h \
           inc/NodePort.h
SOURCES += src/NodeEditorWidget.cpp \
           src/NodeEditorScene.cpp \
           src/NodeItem.cpp \
           src/NodeConnection.cpp \
           src/NodePort.cpp \
           src/ExampleNode.cpp
#RESOURCES += NodeEditorWidget.qrc

INCLUDEPATH += $$PWD/inc \
               $$PWD/../ExpanderWidget

# -----------------------------------
# Libraries
# -----------------------------------

