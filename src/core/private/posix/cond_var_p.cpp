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

#include <sys/time.h>

#include <core/cond_var.h>
#include "cond_var_p.h"
#include "mutex_p.h"

namespace IdealCore {

CondVar::PrivateImpl::PrivateImpl(Mutex &mutex)
    : Private(mutex)
{
    pthread_cond_init(&m_cond, 0);
}

CondVar::PrivateImpl::~PrivateImpl()
{
    pthread_cond_destroy(&m_cond);
}

void CondVar::wait()
{
    pthread_cond_wait(&D_I->m_cond, &static_cast<Mutex::PrivateImpl*>(d->m_mutex.d)->m_mutex);
}

void CondVar::timedWait(iint32 ms)
{
    struct timeval curr;
    gettimeofday(&curr, 0);
    struct timespec timeout;
    timeout.tv_sec = curr.tv_sec + ms / 1000;
    timeout.tv_nsec = curr.tv_usec * 1000 + (ms % 1000) * 1000000;
    if (timeout.tv_nsec >= 1000000000) {
        ++timeout.tv_sec;
        timeout.tv_nsec -= 1000000000;
    }
    pthread_cond_timedwait(&D_I->m_cond, &static_cast<Mutex::PrivateImpl*>(d->m_mutex.d)->m_mutex, &timeout);
}

void CondVar::signal()
{
    pthread_cond_signal(&D_I->m_cond);
}

void CondVar::broadcast()
{
    pthread_cond_broadcast(&D_I->m_cond);
}

}

