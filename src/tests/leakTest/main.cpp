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

#include <core/application.h>
#include <core/object.h>

#define CASE1 1
#define CASE2 1

using namespace IdealCore;
using namespace std;

class MyObject
    : public Object
{
public:
    MyObject(Object *object);

    IDEAL_SIGNAL(mySignal, int);

    void emitMySignal(iint32 value)
    {
        emit(mySignal, value);
    }
};

MyObject::MyObject(Object *object)
    : Object(object)
    , IDEAL_SIGNAL_INIT(mySignal, int)
{
}

class OtherObject
    : public Object
{
public:
    OtherObject(Object *object);
    virtual ~OtherObject(){}

    void receivedSignal(iint32 value);
};

OtherObject::OtherObject(Object *object)
    : Object(object)
{
}

void OtherObject::receivedSignal(iint32 value)
{
    IDEAL_SDEBUG("A signal was received in object " << this);
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

#if CASE1
    List<Object*> objectList;

    for (iint32 i = 0; i < 1000; ++i) {
        MyObject *myObject = new MyObject(&app);
        OtherObject *otherObject = new OtherObject(&app);
        Object::connect(myObject->mySignal, otherObject, &OtherObject::receivedSignal);
        objectList.push_back(myObject);
        objectList.push_back(otherObject);
    }

#if CASE2
    MyObject *object = static_cast<MyObject*>(objectList.front());
    for (iint32 i = 0; i < 1000; ++i) {
        object->emitMySignal(i);
    }
#endif

    List<Object*>::iterator it;
    for (it = objectList.begin(); it != objectList.end(); ++it) {
        delete *it;
    }
#else
    MyObject *object = new MyObject(&app);
    OtherObject *otherObject = new OtherObject(&app);
    Object::connect(object->mySignal, otherObject, &OtherObject::receivedSignal);
    for (iint32 i = 0; i < 1000; ++i) {
        object->emitMySignal(i);
    }
    delete object;
    delete otherObject;
#endif

    return 0;
}

