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
#include <core/application.h>
#include <stdlib.h>

using namespace IdealCore;
using namespace std;

Timer *timer2 = 0;
Application *s_app = 0;

static void timeout()
{
    IDEAL_DEBUG("I got a timeout (timer #1)");
    timer2->stop();
}

static void timeout2()
{
    IDEAL_DEBUG("I got a timeout (timer #2)");
}

static void timeout3()
{
    IDEAL_DEBUG("I got a timeout (timer #3)");
    s_app->quit();
}

int main(int argc, char **argv)
{
    Application app(argc, argv);
    s_app = &app;

    Timer *timer = new Timer(&app);
    Object::connectStatic(timer->timeout, timeout);
    timer->setInterval(2000);
    timer->start(Timer::NoSingleShot);

    timer2 = new Timer(&app);
    Object::connectStatic(timer2->timeout, timeout2);
    timer2->setInterval(3000);
    timer2->start(Timer::NoSingleShot);

    Timer *timer3 = new Timer(&app);
    Object::connectStatic(timer3->timeout, timeout3);
    timer3->setInterval(3000);
    timer3->start(Timer::NoSingleShot);

    return app.exec();
}
