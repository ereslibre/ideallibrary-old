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

using namespace IdealCore;

pthread_mutex_t output = PTHREAD_MUTEX_INITIALIZER;

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
}

void AnObject::slot()
{
    pthread_mutex_lock(&output);
    IDEAL_SDEBUG("*** Slot being called (" << i << ")");
    pthread_mutex_unlock(&output);

    if (++i == 5) {
        application()->quit();
    }
}

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

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
    pthread_mutex_lock(&output);
    IDEAL_SDEBUG("*** New thread with ID " << pthread_self());
    pthread_mutex_unlock(&output);

    pthread_mutex_lock(&mutex1);
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&mutex1);

    pthread_mutex_lock(&output);
    IDEAL_SDEBUG("*** Starting the party at thread " << pthread_self());
    pthread_mutex_unlock(&output);

    while (true) {
        object.emitIt();
        struct timespec ts;
        ts.tv_sec = 1;
        ts.tv_nsec = 0;
        nanosleep(&ts, 0);
    }
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
    pthread_mutex_lock(&output);
    IDEAL_SDEBUG("*** New thread with ID " << pthread_self());
    pthread_mutex_unlock(&output);

    pthread_mutex_lock(&mutex2);
    pthread_cond_signal(&cond2);
    pthread_mutex_unlock(&mutex2);

    pthread_mutex_lock(&output);
    IDEAL_SDEBUG("*** Starting the party at thread " << pthread_self());
    pthread_mutex_unlock(&output);

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

    pthread_mutex_lock(&output);
    IDEAL_SDEBUG("*** Two threads will be launched. The app will be stopped when the slot has been called 5 times");
    pthread_mutex_unlock(&output);

    OneClass oneClass(&app);
    OtherClass otherClass(&app);

    pthread_mutex_lock(&mutex1);
    oneClass.exec();
    pthread_cond_wait(&cond1, &mutex1);
    pthread_mutex_unlock(&mutex1);

    pthread_mutex_lock(&mutex2);
    otherClass.exec();
    pthread_cond_wait(&cond2, &mutex2);
    pthread_mutex_unlock(&mutex2);

    pthread_mutex_lock(&output);
    IDEAL_SDEBUG("*** Going to carefully connect two parties going on (BTW, I am " << pthread_self() << ")");
    pthread_mutex_unlock(&output);

    Object::connect(oneClass.object.aSignal, &otherClass.object, &AnObject::slot);

    return app.exec();
}
