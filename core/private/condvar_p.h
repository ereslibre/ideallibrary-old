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

#ifndef CONDVAR_P_H
#define CONDVAR_P_H

#include <pthread.h>
#include <core/condvar.h>
#include <core/mutex.h>

namespace IdealCore {

class CondVar::Private
{
public:
    Private(Mutex *mutex, CondVar *q);
    virtual ~Private();

    void wait();
    void timedWait(int ms);
    void signal();
    void broadcast();

    Mutex   *m_mutex;   // Initialized in base class
    CondVar *q;         // Initialized in base class
};

}

#include <core/private/posix/condvar_p.h>

#endif //CONDVAR_P_H
