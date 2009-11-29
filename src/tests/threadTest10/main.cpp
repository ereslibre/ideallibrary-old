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
#include <core/timer.h>

using namespace IdealCore;

class MyObject
    : public Object
{
public:
    MyObject(Object *parent)
        : Object(parent)
        , m_i(0)
    {
    }

    void doSomething()
    {
        ++m_i;
        if (m_i == 2) {
            application()->quit();
        }
    }

private:
    iint32 m_i;
};

int main(int argc, char **argv)
{
    Application app(argc, argv);

    MyObject *myObject = new MyObject(&app);
    Mutex mutex;

    Timer t(&app);
    t.setInterval(100);
    Object::connectSynchronized(t.timeout, myObject, &MyObject::doSomething, mutex);

    Timer t2(&app);
    t2.setInterval(100);
    Object::connectSynchronized(t2.timeout, myObject, &MyObject::doSomething, mutex);

    t.start();
    t2.start();

    return app.exec();
}
