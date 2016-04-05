#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T11:40:46
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = yvEncryptedProtocol
TEMPLATE = lib

DEFINES += YVENCRYPTEDPROTOCOL_LIBRARY

SOURCES += \
    udpsocket.cpp \
    yvep.cpp

HEADERS += yvep.h\
    yvencryptedprotocol_global.h \
    udpsocket.h

DESTDIR += ../lib

LIBS += -lsodium
