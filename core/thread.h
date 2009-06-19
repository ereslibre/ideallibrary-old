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

#ifndef THREAD_H
#define THREAD_H

#include <ideal_export.h>

namespace IdealCore {

/**
  * @class Thread thread.h core/thread.h
  *
  * Allows you to execute code in a different thread.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Thread
{
public:
    enum Type {
        Joinable = 0, ///< A joinable thread will expect join() to be called on it. join() can be called
                      ///< after the thread has finished its execution. Additionally, you are the
                      ///< responsible of deleting this object. You can reuse this object if you wish.
                      ///< For example:
                      /// @code
                      /// MyJoinableThread *myThread = new MyJoinableThread;
                      /// myThread->exec();
                      /// myThread->join();
                      /// myThread->exec();
                      /// myThread->join();
                      /// delete myThread;
                      /// @endcode
        NoJoinable,   ///< A non joinable thread will be automatically freed when its execution finishes.
                      ///< This means that you are not the responsible of deleting it. It is very important
                      ///< to understand that NoJoinable Thread objects cannot be synchronized in any way.
                      ///< They start they life, and they dissappear. The interesting advantage is that they
                      ///< obviously can emit signals. So, when using them, you have to think as a naturally
                      ///< asynchronous relationship. For example:
                      /// @code
                      /// MyNoJoinableThread *myThread = new MyNoJoinableThread;
                      /// connect(myThread->resultOfHeavyMath, myResult, &MyResult::printResult);
                      /// myThread->exec();
                      /// @endcode
    };

    enum Priority {
        LowestPriority = 0, ///< This thread will run with lowest priority
        MediumPriority,     ///< This thread will run with medium priority
        HighestPriority     ///< This thread will run with highest priority
    };

    Thread(Type type = Joinable, Priority priority = LowestPriority);
    virtual ~Thread();

    /**
      * Creates the new thread and executes run method in a new thread.
      */
    void exec();

    /**
      * Waits for this thread to finish.
      */
    void join();

    /**
      * @return The type of this thread.
      */
    Type type() const;

    /**
      * @return The priority of this thread.
      */
    Priority priority() const;

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

#endif //THREAD_H
