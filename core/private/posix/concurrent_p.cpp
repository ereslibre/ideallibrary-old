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

#include <pthread.h>

#include <core/concurrent.h>
#include "concurrent_p.h"

namespace IdealCore {

Concurrent::PrivateImpl::PrivateImpl(Concurrent *q)
    : Private(q)
{
}

Concurrent::PrivateImpl::~PrivateImpl()
{
}

void *Concurrent::PrivateImpl::entryPoint(void *param)
{
    Concurrent *concurrent = static_cast<Concurrent*>(param);
    concurrent->run();
    return 0;
}

void Concurrent::Private::exec()
{
    pthread_create(&D_I->m_thread, 0, PrivateImpl::entryPoint, q);
}

void Concurrent::Private::join()
{
    pthread_join(D_I->m_thread, NULL);
}

}

