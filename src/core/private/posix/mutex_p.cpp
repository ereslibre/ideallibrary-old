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

#include <core/mutex.h>
#include "mutex_p.h"

namespace IdealCore {

Mutex::PrivateImpl::PrivateImpl(RecursionType recursionType)
{
    pthread_mutexattr_init(&m_attr);
    if (recursionType == Recursive) {
        pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE);
    }
    pthread_mutex_init(&m_mutex, &m_attr);
}

Mutex::PrivateImpl::~PrivateImpl()
{
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock()
{
    pthread_mutex_lock(&D_I->m_mutex);
}

bool Mutex::tryLock()
{
    return !pthread_mutex_trylock(&D_I->m_mutex);
}

void Mutex::unlock()
{
    pthread_mutex_unlock(&D_I->m_mutex);
}

}
