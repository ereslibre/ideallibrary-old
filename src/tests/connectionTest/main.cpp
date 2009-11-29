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

void function()
{
    IDEAL_SDEBUG("A signal was received on a static function");
}

class MyObject
    : public IdealCore::Object
{
public:
    MyObject(IdealCore::Object *object);

    IDEAL_SIGNAL(mySignal);
    IDEAL_SIGNAL(otherSignal);

    void emitMySignal()
    {
        emit(mySignal);
    }

    void emitMySignalFromConst() const
    {
        emit(mySignal);
    }

    void emitOtherSignal()
    {
        emit(otherSignal);
    }
};

MyObject::MyObject(Object *object)
    : Object(object)
    , IDEAL_SIGNAL_INIT(mySignal)
    , IDEAL_SIGNAL_INIT(otherSignal)
{
}

class OtherObject
    : public IdealCore::Object
{
public:
    OtherObject(IdealCore::Object *object);
    virtual ~OtherObject(){}

    IDEAL_SIGNAL(otherSignal);

    void receivedSignal();
    void multiSlot(IdealCore::Object *sender);
    static void staticMethod();
    static void staticMethodMulti(IdealCore::Object *sender);
};

OtherObject::OtherObject(Object *object)
    : Object(object)
    , IDEAL_SIGNAL_INIT(otherSignal)
{
}

void OtherObject::receivedSignal()
{
    IDEAL_SDEBUG("A signal was received in object " << this);
}

void OtherObject::multiSlot(Object *sender)
{
    IDEAL_SDEBUG("A signal was received in object " << this << " and was sent by " << sender);
}

void OtherObject::staticMethod()
{
    IDEAL_SDEBUG("A signal was received on a static method");
}

void OtherObject::staticMethodMulti(Object *sender)
{
    IDEAL_SDEBUG("A signal was received on a static method and was sent by " << sender);
}

int main(int argc, char **argv)
{
    IdealCore::Application app(argc, argv);

    MyObject *myObject = new MyObject(&app);
    OtherObject *otherObject = new OtherObject(&app);

    // connect a signal to a static function
    IdealCore::Object::connectStatic(myObject->mySignal, function);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    IdealCore::Object::disconnectStatic(myObject->mySignal, function);
    myObject->emitMySignal();

    // connect a signal to another signal
    IdealCore::Object::connect(myObject->mySignal, otherObject->otherSignal);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    IdealCore::Object::disconnect(myObject->mySignal, otherObject->otherSignal);
    myObject->emitMySignal();

    // connect a signal to a slot
    IdealCore::Object::connect(myObject->mySignal, otherObject, &OtherObject::receivedSignal);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    IdealCore::Object::disconnect(myObject->mySignal, otherObject, &OtherObject::receivedSignal);
    myObject->emitMySignal();

    // connect a signal to a static slot
    IdealCore::Object::connectStatic(myObject->mySignal, OtherObject::staticMethod);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    IdealCore::Object::disconnectStatic(myObject->mySignal, OtherObject::staticMethod);
    myObject->emitMySignal();

    // connect two signals to a multislot
    IdealCore::Object::connectMulti(myObject->mySignal, otherObject, &OtherObject::multiSlot);
    IdealCore::Object::connectMulti(myObject->otherSignal, otherObject, &OtherObject::multiSlot);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    myObject->emitOtherSignal();
    IdealCore::Object::disconnectMulti(myObject->mySignal, otherObject, &OtherObject::multiSlot);
    IdealCore::Object::disconnectMulti(myObject->otherSignal, otherObject, &OtherObject::multiSlot);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    myObject->emitOtherSignal();

    // connect two signals to a static multislot
    IdealCore::Object::connectStaticMulti(myObject->mySignal, OtherObject::staticMethodMulti);
    IdealCore::Object::connectStaticMulti(myObject->otherSignal, OtherObject::staticMethodMulti);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    myObject->emitOtherSignal();
    IdealCore::Object::disconnectStaticMulti(myObject->mySignal, OtherObject::staticMethodMulti);
    IdealCore::Object::disconnectStaticMulti(myObject->otherSignal, OtherObject::staticMethodMulti);
    myObject->emitMySignal();
    myObject->emitMySignalFromConst();
    myObject->emitOtherSignal();

    delete myObject;
    delete otherObject;

    return 0;
}
