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

#ifndef THREAD_P_H_POSIX
#define THREAD_P_H_POSIX

#include <pthread.h>
#include <core/private/thread_p.h>

namespace IdealCore {

class Thread::PrivateImpl
    : public Thread::Private
{
public:
    PrivateImpl(Type type);
    virtual ~PrivateImpl();

    static void *entryPoint(void *param);

    pthread_t          m_thread;
    pthread_attr_t     m_attr;
    struct sched_param m_schedParam;
};

}

#endif //THREAD_P_H_POSIX

