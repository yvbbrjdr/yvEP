#-------------------------------------------------
#
# Project created by QtCreator 2015-04-25T13:09:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yvEC
TEMPLATE = app

LIBS += -lpthread -lwsock32 -leay32

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
