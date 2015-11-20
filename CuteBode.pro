#-------------------------------------------------
#
# Project created by QtCreator 2015-11-11T12:56:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CuteBode
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    trf.cpp \
    plot.cpp

HEADERS  += mainwindow.h \
    trf.h \
    plot.h

FORMS    += mainwindow.ui
