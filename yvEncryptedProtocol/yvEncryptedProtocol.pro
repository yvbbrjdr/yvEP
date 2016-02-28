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

SOURCES += yvep.cpp \
    udpsocket.cpp

HEADERS += yvep.h\
        yvencryptedprotocol_global.h \
    udpsocket.h

DESTDIR += ../lib

unix: LIBS += -lcrypto

win32: LIBS += -leay32
