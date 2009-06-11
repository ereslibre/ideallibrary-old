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

#include <core/thread.h>
#include "thread_p.h"

namespace IdealCore {

Thread::PrivateImpl::PrivateImpl(Thread *q, Type type)
    : Private(q, type)
{
    pthread_attr_init(&m_attr);
    if (type == NoJoinable) {
        pthread_attr_setdetachstate(&m_attr, PTHREAD_CREATE_DETACHED);
    }
}

Thread::PrivateImpl::~PrivateImpl()
{
    pthread_attr_destroy(&m_attr);
}

void *Thread::PrivateImpl::entryPoint(void *param)
{
    Thread *thread = static_cast<Thread*>(param);
    thread->run();
    if (thread->d->m_type == NoJoinable) {
        delete thread;
    }
    return 0;
}

void Thread::Private::exec()
{
    pthread_create(&D_I->m_thread, &D_I->m_attr, PrivateImpl::entryPoint, q);
}

void Thread::Private::join()
{
    if (m_type == Joinable) {
        pthread_join(D_I->m_thread, NULL);
    } else {
        IDEAL_DEBUG_WARNING("join() has been called in a Thread object with attribute NoJoinable");
    }
}

}

