/*
 * This file is part of the Ideal libraries
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
#include <core/concurrent.h>
#include <core/mutex.h>
#include <core/condvar.h>
#include <core/timer.h>

using namespace IdealCore;

Mutex output;

class AnObject
    : public Object
{
public:
    AnObject(Object *parent);

    IDEAL_SIGNAL(aSignal);

    void emitIt();
    void slot();

private:
    int i;
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
    output.lock();
    IDEAL_SDEBUG("*** Slot being called (" << i << ")");
    output.unlock();

    if (++i == 5) {
        application()->quit();
    }
}

Mutex mutex1;
Mutex mutex2;
CondVar cond1 = CondVar(&mutex1);
CondVar cond2 = CondVar(&mutex2);

class OneClass
    : public Concurrent
{
public:
    OneClass(Object *parent);

    AnObject object;

protected:
    void run();
};

OneClass::OneClass(Object *parent)
    : Concurrent(parent)
    , object(this)
{
}

void OneClass::run()
{
    output.lock();
    IDEAL_SDEBUG("*** New thread with ID " << pthread_self());
    output.unlock();

    mutex1.lock();
    cond1.signal();
    mutex1.unlock();

    output.lock();
    IDEAL_SDEBUG("*** Starting the party at thread " << pthread_self());
    output.unlock();

    Timer::callAfter(1000, &object, &AnObject::emitIt);
}

class OtherClass
    : public Concurrent
{
public:
    OtherClass(Object *parent);

    AnObject object;

protected:
    void run();
};

OtherClass::OtherClass(Object *parent)
    : Concurrent(parent)
    , object(this)
{
}

void OtherClass::run()
{
    output.lock();
    IDEAL_SDEBUG("*** New thread with ID " << pthread_self());
    output.unlock();

    mutex2.lock();
    cond2.signal();
    mutex2.unlock();

    output.lock();
    IDEAL_SDEBUG("*** Starting the party at thread " << pthread_self());
    output.unlock();

    while (true) {
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 500000000;
        nanosleep(&ts, 0);
    }
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    output.lock();
    IDEAL_SDEBUG("*** Two threads will be launched. The app will be stopped when the slot has been called 5 times");
    output.unlock();

    OneClass oneClass(&app);
    OtherClass otherClass(&app);

    mutex1.lock();
    oneClass.exec();
    cond1.wait();
    mutex1.unlock();

    mutex2.lock();
    otherClass.exec();
    cond2.wait();
    mutex2.unlock();

    output.lock();
    IDEAL_SDEBUG("*** Going to carefully connect two parties going on (BTW, I am " << pthread_self() << ")");
    output.unlock();

    Object::connect(oneClass.object.aSignal, &otherClass.object, &AnObject::slot);

    return app.exec();
}
