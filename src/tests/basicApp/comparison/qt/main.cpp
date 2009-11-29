/*
 * This file is part of the Ideal Library
 * Copyright (C) 2009 Rafael Fernández López <ereslibre@ereslibre.es>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QtCore/QObject>
#include <QtCore/QDebug>

class MyObject
    : public QObject
{
    Q_OBJECT

public:
    MyObject(QObject *object = 0);

    void emitSignal()
    {
        emit mySignal();
    }

    void emitSignalB(bool b)
    {
        emit pushed(b);
    }

Q_SIGNALS:
    void mySignal();
    void pushed(bool state);

public:
    int testExternalInt;
};

MyObject::MyObject(QObject *object)
    : QObject(object)
{
}

class OtherObject
    : public QObject
{
    Q_OBJECT

public:
    OtherObject(QObject *object = 0);

public Q_SLOTS:
    void receivedSignal();
    void buttonPushed(bool param);
};

OtherObject::OtherObject(QObject *object)
    : QObject(object)
{
}

void OtherObject::receivedSignal()
{
    qDebug() << "I RECEIVED A SIGNAL ! (from object:" << sender() << ")";
    MyObject *myObject = static_cast<MyObject*>(sender());
    myObject->testExternalInt = 100;
}

void OtherObject::buttonPushed(bool param)
{
    qDebug() << "BUTTON CLICKED ! (from object:" << sender() << "and param" << param << ")";
}

int main(int argc, char **argv)
{
    MyObject object;
    OtherObject otherObject;
    QObject::connect(&object, SIGNAL( mySignal () ), &otherObject, SLOT( receivedSignal () ));
    QObject::connect(&object, SIGNAL( pushed ( bool ) ), &otherObject, SLOT( buttonPushed (bool) ));

    // simulate that something happened, and see how slots become called
    for (int i = 0; i < 1000000; ++i) {
        object.emitSignal();
    }
    object.emitSignalB(true /* state */);
    object.emitSignalB(false /* state */);
    object.emitSignalB(true /* state */);
    object.emitSignalB(false /* state */);
    object.emitSignal();

    return 0;
}

#include "main.moc"
