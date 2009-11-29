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

#ifndef CONTEXT_MUTEX_LOCKER_H
#define CONTEXT_MUTEX_LOCKER_H

#include <ideal_export.h>

#include <core/mutex.h>

namespace IdealCore {

/**
  * @class ContextMutexLocker context_mutex_locker.h core/context_mutex_locker.h
  *
  * This class will lock the mutex when created, and will unlock it when destroyed.
  * 
  * Examples of usage are:
  *
  * @code
  * void Class::methodWithCriticalSection()
  * {
  *     ContextMutexLocker cml(theLock);
  *     // critical section here
  * }
  * @endcode
  *
  * what is equivalent to:
  *
  * @code
  * void Class::methodWithCriticalSection()
  * {
  *     theLock.lock();
  *     // critical section here
  *     theLock.unlock();
  * }
  * @endcode
  *
  * Having critical sections as small as possible is a good idea. If the whole method
  * is not a critical section, you can do something like this:
  *
  * @code
  * void Class::methodWithCriticalSection()
  * {
  *     // non critical section here
  *     {
  *         ContextMutexLocker cml(theLock);
  *         // critical section here
  *     }
  *     // non critical section here
  * }
  * @endcode
  *
  * what is equivalent to:
  *
  * @code
  * void Class::methodWithCriticalSection()
  * {
  *     // non critical section here
  *     theLock.lock();
  *     // critical section here
  *     theLock.unlock();
  *     // non critical section here
  * }
  * @endcode
  *
  * This will help also visually to detect really fast critical sections.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT ContextMutexLocker
{
public:
    ContextMutexLocker(Mutex &mutex);
    virtual ~ContextMutexLocker();

private:
    Mutex &m_mutex;
};

}

#endif //CONTEXT_MUTEX_LOCKER_H
