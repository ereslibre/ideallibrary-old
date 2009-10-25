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

#ifndef COND_VAR_H
#define COND_VAR_H

#include <ideal_export.h>
#include <core/mutex.h>

namespace IdealCore {

/**
  * @class CondVar condvar.h core/condvar.h
  *
  * Condition variable.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT CondVar
{
public:
    CondVar(Mutex &mutex);
    virtual ~CondVar();

    /**
      * Locks the current thread waiting for this condition variable to be signaled.
      */
    void wait();

    /**
      * Locks the current thread waiting for this condition variable to be signaled
      * for a maximum interval of @p ms milliseconds.
      */
    void timedWait(int ms);

    /**
      * Signal this condition variable.
      */
    void signal();

    /**
      * Broadcast this condition variable.
      */
    void broadcast();

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //COND_VAR_H
