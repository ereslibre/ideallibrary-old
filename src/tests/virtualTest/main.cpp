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

using namespace IdealCore;
using namespace std;

class Abstract
    : public Object
{
public:
    Abstract(Object *parent)
        : Object(parent)
    {
    }

    virtual void test() const = 0;
};

class Implementation
    : public Abstract
{
public:
    Implementation(Object *parent)
        : Abstract(parent)
    {
    }

    virtual void test() const
    {
        IDEAL_SDEBUG("Test implementation was called");
    }
};

class Trigger
    : public Object
{
public:
    Trigger(Object *parent)
        : Object(parent)
        , IDEAL_SIGNAL_INIT(trigger)
    {
    }

    void emitSignal()
    {
        emit(trigger);
    }

    IDEAL_SIGNAL(trigger);
};

int main(int argc, char **argv)
{
    Application app(argc, argv);

    Abstract *object = new Implementation(&app);
    Implementation *object2 = new Implementation(&app);
    Trigger *trigger = new Trigger(&app);

    Object::connect(trigger->trigger, object, &Abstract::test);
    Object::connect(trigger->trigger, object2, &Abstract::test);

    trigger->emitSignal();

    return 0;
}
