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
#include <core/mutex.h>
#include <core/cond_var.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    Application app(argc, argv);

    IDEAL_SDEBUG("*** Going to wait for a conditional variable that will never be signaled as maximum 1552 ms");
    IDEAL_SDEBUG("*** Obviously, there is no possibility of a so fine grain timing");
    IDEAL_SDEBUG("*** However, we are sure the number (using 'time' command) will always be a bit bigger, never smaller");

    Mutex mutex;
    CondVar condvar(mutex);
    mutex.lock();
    condvar.timedWait(1552);
    mutex.unlock();

    return 0;
}

