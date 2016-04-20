#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T19:29:32
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ImageTransfer
TEMPLATE = lib
CONFIG += plugin

SOURCES += imagetransfer.cpp \
    ../plugin.cpp \
    imagetransferdialog.cpp \
    imageshowdialog.cpp

HEADERS += imagetransfer.h \
    ../plugin.h \
    imagetransferdialog.h \
    imageshowdialog.h

FORMS += \
    imagetransferdialog.ui \
    imageshowdialog.ui
