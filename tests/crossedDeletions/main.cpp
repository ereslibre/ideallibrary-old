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
#include <core/object.h>

using namespace IdealCore;

class A
    : public Object
{
public:
    IDEAL_SIGNAL(signal);

    A(Object *parent)
        : Object(parent)
        , IDEAL_SIGNAL_INIT(signal)
    {
    }

    void emitMySignal()
    {
        emit(signal);
    }
};

int main(int argc, char **argv)
{
    Application app(argc, argv);

    A *a = new A(&app);
    Object *b = new Object(&app);
    IDEAL_SDEBUG("a is " << a << " while b is " << b << ". app is " << &app);
    Object::connect(a->signal, b, &Object::deleteNow);
    Object::connect(b->destroyed, a, &Object::deleteNow);
    a->emitMySignal();

    return 0;
}
