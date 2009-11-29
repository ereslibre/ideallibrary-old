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

#include <QObject>
#include <iostream>

#define CASE1 1
#define CASE2 1

using namespace std;

class MyObject
    : public QObject
{
    Q_OBJECT

public:
    MyObject(QObject *object = 0);

    void emitSignal(int value)
    {
        emit mySignal(value);
    }

Q_SIGNALS:
    void mySignal(int value);
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
    void receivedSignal(int value);
};

OtherObject::OtherObject(QObject *object)
    : QObject(object)
{
}

void OtherObject::receivedSignal(int value)
{
    IDEAL_SDEBUG("A signal was received in object " << this << " from " << sender());
}

int main(int argc, char **argv)
{
#if CASE1
    List<QObject*> objectList;

    for (int i = 0; i < 1000; ++i) {
        MyObject *object = new MyObject;
        OtherObject *otherObject = new OtherObject;
        QObject::connect(object, SIGNAL(mySignal(int)), otherObject, SLOT(receivedSignal(int)));
        objectList.push_back(object);
        objectList.push_back(otherObject);
    }

#if CASE2
    MyObject *object = static_cast<MyObject*>(objectList.front());
    for (int i = 0; i < 1000; ++i) {
        object->emitSignal(i);
    }
#endif

    List<QObject*>::iterator it;
    for (it = objectList.begin(); it != objectList.end(); ++it) {
        delete *it;
    }
#else
    MyObject *object = new MyObject;
    OtherObject *otherObject = new OtherObject;
    QObject::connect(object, SIGNAL(mySignal(int)), otherObject, SLOT(receivedSignal(int)));
    for (int i = 0; i < 1000; ++i) {
        object->emitSignal(i);
    }
    delete object;
    delete otherObject;
#endif

    return 0;
}

#include "main.moc"
