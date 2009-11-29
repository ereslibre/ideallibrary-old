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
        NoRecursive = 0,    ///< This mutex is non recursive. Two lock() calls from the same thread will result in a deadlock.
        Recursive           ///< This mutex is recursive. Two or more lock() calls from the same thread will not deadlock.
    };

    Mutex(RecursionType recursionType = NoRecursive);
    Mutex(const Mutex &mutex);
    virtual ~Mutex();

    /**
      * Locks this mutex. If the mutex was already locked we wait until the mutex becomes available.
      */
    void lock();

    /**
      * Tries to lock this mutex.
      *
      * @return Whether this mutex could be locked or not.
      *
      * @note If this mutex is already locked, this method will not block calling thread.
      */
    bool tryLock();

    /**
      * Unlocks this mutex.
      */
    void unlock();

    bool operator==(const Mutex &mutex) const;
    bool operator!=(const Mutex &mutex) const;

private:
    class Private;
    class PrivateImpl;
    Private *d;
};

}

#endif //MUTEX_H
