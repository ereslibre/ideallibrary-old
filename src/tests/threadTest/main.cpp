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
#include <core/thread.h>
#include <core/mutex.h>
#include <core/cond_var.h>
#include <core/timer.h>

using namespace IdealCore;

class AnObject
    : public Object
{
public:
    AnObject(Object *parent);

    IDEAL_SIGNAL(aSignal);

    void emitIt();
    void slot();

private:
    iint32 i;
};

AnObject::AnObject(Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(aSignal)
    , i(0)
{
}

void AnObject::emitIt()
{
    emit(aSignal);
    Timer::callAfter(1000, this, &AnObject::emitIt);
}

void AnObject::slot()
{
    IDEAL_SDEBUG("*** Slot being called (" << i << ")");

    if (++i == 5) {
        application()->quit();
    }
}

Mutex mutex1;
Mutex mutex2;
CondVar cond1 = CondVar(mutex1);
CondVar cond2 = CondVar(mutex2);

class OneClass
    : public Object
    , public Thread
{
public:
    OneClass(Object *parent);

    AnObject *object;

protected:
    void run();
};

OneClass::OneClass(Object *parent)
    : Object(parent)
    , object(new AnObject(this))
{
}

void OneClass::run()
{
    IDEAL_SDEBUG("*** New thread with ID " << pthread_self());

    mutex1.lock();
    cond1.signal();
    mutex1.unlock();

    IDEAL_SDEBUG("*** Starting the party at thread " << pthread_self());

    Timer::callAfter(1000, object, &AnObject::emitIt);
}

class OtherClass
    : public Object
    , public Thread
{
public:
    OtherClass(Object *parent);

    AnObject *object;

protected:
    void run();
};

OtherClass::OtherClass(Object *parent)
    : Object(parent)
    , object(new AnObject(this))
{
}

void OtherClass::run()
{
    IDEAL_SDEBUG("*** New thread with ID " << pthread_self());

    mutex2.lock();
    cond2.signal();
    mutex2.unlock();

    IDEAL_SDEBUG("*** Starting the party at thread " << pthread_self());

    while (true) {
        Timer::wait(500);
    }
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    IDEAL_SDEBUG("*** Two threads will be launched. The app will be stopped when the slot has been called 5 times");

    OneClass *oneClass = new OneClass(&app);
    OtherClass *otherClass = new OtherClass(&app);

    mutex1.lock();
    oneClass->exec();
    cond1.wait();
    mutex1.unlock();

    mutex2.lock();
    otherClass->exec();
    cond2.wait();
    mutex2.unlock();

    IDEAL_SDEBUG("*** Going to carefully connect two parties going on (BTW, I am " << pthread_self() << ")");

    Object::connect(oneClass->object->aSignal, otherClass->object, &AnObject::slot);

    return app.exec();
}
