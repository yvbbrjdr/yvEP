#-------------------------------------------------
#
# Project created by QtCreator 2016-03-12T14:24:09
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = yvEncryptedChatServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    mainthread.cpp \
    serverthread.cpp

INCLUDEPATH += ../yvEncryptedProtocol

LIBS     += -L../lib -lyvEncryptedProtocol

HEADERS += \
    mainthread.h \
    serverthread.h
