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

#ifndef CONCURRENT_H
#define CONCURRENT_H

#include <ideal_export.h>

namespace IdealCore {

/**
  * @class Concurrent concurrent.h core/concurrent.h
  *
  * Allows you to execute code in a different thread.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Concurrent
{
public:
    enum Type {
        Joinable = 0, ///< A joinable thread will expect join() to be called on it. This object and its resources will not be freed until this happens.
        NoJoinable,   ///< A non joinable thread will be automatically deleted when its execution finishes.
    };

    Concurrent(Type type = Joinable);
    virtual ~Concurrent();

    /**
      * Creates the new thread and executes run method in a new thread.
      */
    void exec();

    /**
      * Waits for this thread to finish.
      */
    void join();

protected:
    /**
     * When calling to exec() the code inside this method will be executed in a new thread.
     */
    virtual void run() = 0;

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //CONCURRENT_H
