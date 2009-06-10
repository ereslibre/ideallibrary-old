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

//### WARNING:
//    This test only exists because of an strange warning got by helgrind. Since I think the code
//    was just OK, I have filled a bug with a small test case of the problem on the valgrind bug
//    tracker. So this is hopefully a helgrind bug.
//
//    Bug report: https://bugs.kde.org/show_bug.cgi?id=195845
//    Test case:  https://bugs.kde.org/attachment.cgi?id=34397

#include <core/application.h>
#include <core/concurrent.h>
#include <core/timer.h>

using namespace IdealCore;

class OneClass
    : public Concurrent
{
public:
    OneClass(Object *parent);

protected:
    void run();
};

OneClass::OneClass(Object *parent)
    : Concurrent(parent)
{
}

void OneClass::run()
{
}

int main(int argc, char **argv)
{
    // Please note that you should _never_ write your code like this:

    Application *app = new Application(argc, argv);

    OneClass *oneClass = new OneClass(app);
    oneClass->exec();

    Timer::wait(500);

    delete app;

    return 0;

    // This should have written like _this_ instead:
#if 0
    Application app(argc, argv);

    OneClass *oneClass = new OneClass(&app);
    oneClass->exec();

    Timer::wait(500);

    return app.quit();
#endif
}
