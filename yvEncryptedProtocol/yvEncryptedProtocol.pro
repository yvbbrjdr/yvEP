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
    yvep.cpp \
    yvepsocket.cpp \
    socketsender.cpp \
    socketreceiver.cpp

HEADERS += yvep.h\
    yvencryptedprotocol_global.h \
    udpsocket.h \
    yvepsocket.h \
    socketbuffer.h \
    socketsender.h \
    socketreceiver.h \
    statusflag.h

DESTDIR += ../lib

LIBS += -lsodium
