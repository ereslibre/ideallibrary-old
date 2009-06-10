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
#include <core/object.h>

namespace IdealCore {

/**
  * @class Concurrent concurrent.h core/concurrent.h
  *
  * Allows you to execute code in a different thread.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Concurrent
    : public Object
{
public:
    Concurrent(Object *parent);
    ~Concurrent();

    enum State {
        NotStarted = 0,     ///< this thread has not started yet (not called to exec() yet)
        Running,            ///< this thread is running
        Finished            ///< this thread has finished its execution
    };

    /**
      * Creates the new thread and executes run method in a new thread.
      */
    void exec();

    /**
      * Waits for this thread to finish.
      */
    void join();

    /**
      * @return The current state of this thread.
      */
    State state() const;

public:
    IDEAL_SIGNAL(finished);

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
