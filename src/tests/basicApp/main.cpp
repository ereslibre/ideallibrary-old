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

#include <core/object.h>
#include <core/timer.h>
#include <core/file.h>
#include <core/application.h>
#include <core/genious_pointer.h>

using namespace IdealCore;

class MyObject
    : public Object
{
public:
    MyObject(Object *object);
    virtual ~MyObject()
    {
        IDEAL_SDEBUG("deleting " << this);
    }

    IDEAL_SIGNAL(mySignal);
    IDEAL_SIGNAL(pushed, bool /* state */);
    IDEAL_SIGNAL(testWithMoreParams, bool, String, int);

    void emitMySignal()
    {
        emit(mySignal);
    }

    void emitPushed(bool state)
    {
        emit(pushed, true);
    }
};

MyObject::MyObject(Object *object)
    : Object(object)
    , IDEAL_SIGNAL_INIT(mySignal)
    , IDEAL_SIGNAL_INIT(pushed, bool)
    , IDEAL_SIGNAL_INIT(testWithMoreParams, bool, String, int)
{
}

class OtherObject
    : public Object
{
public:
    OtherObject(Object *object);
    virtual ~OtherObject()
    {
        IDEAL_SDEBUG("deleting " << this);
    }

    void receivedSignal();
    void buttonPushed(bool param);
    void destroyedSlot();
};

OtherObject::OtherObject(Object *object)
    : Object(object)
{
}

void OtherObject::receivedSignal()
{
    IDEAL_SDEBUG("I (" << this << ") RECEIVED A SIGNAL !");
}

void OtherObject::buttonPushed(bool param)
{
    IDEAL_SDEBUG("BUTTON CLICKED ! (param " << param << ")");
}

void OtherObject::destroyedSlot()
{
    IDEAL_SDEBUG("deleting object");
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    MyObject *myObject = new MyObject(&app);
    OtherObject *otherObject = new OtherObject(&app);

    Object::connect(myObject->mySignal, otherObject, &OtherObject::receivedSignal);

    delete myObject;
    delete otherObject;


    // Genious pointers usage
    MyObject *geniousPointerObj = new MyObject(&app);
    GeniousPointer<MyObject> myGeniousPointer(geniousPointerObj);
    otherObject = new OtherObject(&app);

    Object::connect(myGeniousPointer->mySignal, otherObject, &OtherObject::receivedSignal);

    myGeniousPointer->emitMySignal(); // normal pointer usage

    IDEAL_DEBUG("My genious pointer points to " << myGeniousPointer.content() << " before deleting anything. Content Destroyed ? " << (myGeniousPointer.isContentDestroyed() ? "Yes" : "No"));
    delete geniousPointerObj;
    IDEAL_DEBUG("My genious pointer points to " << myGeniousPointer.content() << " after deleting the object. Content Destroyed ? " << (myGeniousPointer.isContentDestroyed() ? "Yes" : "No"));

    delete otherObject;

    return 0;
}

