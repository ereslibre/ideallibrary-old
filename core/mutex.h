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

#ifndef MUTEX_H
#define MUTEX_H

#include <ideal_export.h>

namespace IdealCore {

/**
  * @class Mutex mutex.h core/mutex.h
  *
  * Allows you to protect critical sections when executing several threads accessing/modifying a
  * critical section.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Mutex
{
    friend class CondVar;

public:
    enum RecursionType {
        NoRecursive = 0,
        Recursive
    };

    Mutex(RecursionType recursionType = NoRecursive);
    virtual ~Mutex();

    /**
      * Locks this mutex. If the mutex was already locked we wait until the mutex becomes available.
      */
    void lock();
    
    /**
      * Unlocks this mutex.
      */
    void unlock();

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};


/**
  * @class ContextMutexLocker mutex.h core/mutex.h
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
    ~ContextMutexLocker();

private:
    Mutex &mutex;
};

}

#endif //MUTEX_H
