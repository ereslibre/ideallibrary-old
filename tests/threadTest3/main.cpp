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
};

AnObject::AnObject(Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(aSignal)
{
}

void AnObject::emitIt()
{
    emit(aSignal);
}

void AnObject::slot()
{
    IDEAL_SDEBUG("*** Slot being called from thread " << pthread_self());
}

class OneClass
    : public Concurrent
{
public:
    OneClass(Object *parent);

    AnObject *object;

protected:
    void run();
};

OneClass::OneClass(Object *parent)
    : Concurrent(parent)
{
}

void OneClass::run()
{
    connect(object->aSignal, object, &AnObject::slot);
    connect(object->aSignal, object, &AnObject::slot);
    connect(object->aSignal, object, &AnObject::slot);
    connect(object->aSignal, object, &AnObject::slot);
    connect(object->aSignal, object, &AnObject::slot);
    disconnect(object->aSignal, object, &AnObject::slot);
    disconnect(object->aSignal, object, &AnObject::slot);
    disconnect(object->aSignal, object, &AnObject::slot);
    disconnect(object->aSignal, object, &AnObject::slot);
    object->emitIt();
    object->emitIt();
    object->emitIt();
    object->emitIt();
    object->emitIt();
    disconnect(object->aSignal, object, &AnObject::slot);
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    AnObject *anObject = new AnObject(&app);
    OneClass *oneClass = new OneClass(&app);
    oneClass->object = anObject;
    OneClass *otherClass = new OneClass(&app);
    otherClass->object = anObject;

    oneClass->exec();
    otherClass->exec();

#if 1
    Timer::wait(500);

    return 0;
#else
    Timer::callAfter(500, &app, &Application::quit);

    return app.exec();
#endif
}
