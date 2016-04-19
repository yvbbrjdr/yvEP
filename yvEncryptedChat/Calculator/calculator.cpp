/*
This file is part of yvEncryptedChat
yvEncryptedChat is an IM program that provides secure chats. 
Copyright (C) 2016  yvbbrjdr

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "calculator.h"

void Calculator::Init(PluginManager *manager) {
    connect(manager,SIGNAL(RecvMsg(QString,QString)),this,SLOT(RecvMsg(QString,QString)));
    connect(this,SIGNAL(SendMsg(QString,QString)),manager,SIGNAL(SendMsg(QString,QString)));
}

void Calculator::RecvMsg(const QString &Nickname,const QString &Content) {
    if (Nickname=="Broadcast")
        return;
    for (int i=0;i<Content.length();++i) {
        if (Content[i].isLetter()) {
            emit SendMsg(Nickname,"Letters are forbidden in the expression");
            return;
        }
    }
    emit SendMsg(Nickname,qse.evaluate(Content).toString());
}
