#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T12:06:05
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = yvEncryptedChat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logindialog.cpp \
    plugin.cpp \
    pluginmanager.cpp \
    configmanager.cpp

HEADERS  += mainwindow.h \
    logindialog.h \
    plugin.h \
    pluginmanager.h \
    configmanager.h

FORMS    += mainwindow.ui \
    logindialog.ui

INCLUDEPATH += ../yvEncryptedProtocol

LIBS     += -L../lib -lyvEncryptedProtocol
