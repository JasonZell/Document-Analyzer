#-------------------------------------------------
#
# Project created by QtCreator 2016-09-08T14:27:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EditorsDreamGui
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    textparser.cpp \
    worker.cpp \
    buglist.cpp

HEADERS  += mainwindow.h \
    heap.h \
    node.h \
    pQueue.h \
    pQueueNode.h \
    textparser.h \
    worker.h

FORMS    += mainwindow.ui

RESOURCES += \
    editordreamresource.qrc
