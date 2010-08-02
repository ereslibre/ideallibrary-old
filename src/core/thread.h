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

#ifndef THREAD_H
#define THREAD_H

#include <core/object.h>

namespace IdealCore {

/**
  * @class Thread thread.h core/thread.h
  *
  * Allows you to execute code in a different thread.
  *
  * It is very encouraged not to inherit from this class and reimplement run() method, but despite
  * connect to the signal started which is emitted from the new thread when it is started to the
  * part of your code that will be executed in a new thread. Brief example:
  *
  * @code
  * void myNewThreadFunction()
  * {
  *     // Code in new thread here
  * }
  *
  * Thread *newThread = new Thread(parent, Thread::NoJoinable);
  * newThread->started.connectStatic(myNewThreadFunction);
  * newThread->exec();
  * @endcode
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Thread
    : public Object
{
public:
    enum Type {
        Joinable = 0, ///< A joinable thread will expect join() to be called on it. join() can be called
                      ///< after the thread has finished its execution. Additionally, you are the
                      ///< responsible of deleting this object. You can reuse this object if you wish.
                      ///< For example:
                      /// @code
                      /// MyJoinableThread *myThread = new MyJoinableThread(parent);
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
                      /// MyNoJoinableThread *myThread = new MyNoJoinableThread(parent);
                      /// myThread->resultOfHeavyMath.connect(myResult, &MyResult::printResult);
                      /// myThread->exec();
                      /// @endcode
    };

    Thread(Object *parent, Type type = Joinable);
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
      * Immediately call to join() after calling to exec()
      */
    void execAndJoin();

    /**
      * @return The type of this thread.
      */
    Type type() const;

    /**
      * Emitted when the new thread has been created.
      *
      * @note This signal is emitted from the new thread.
      */
    IDEAL_SIGNAL(started);

protected:
    /**
      * When calling to exec() the code inside this method will be executed in a new thread.
      *
      * By default, this method emits started signal. So you can connect the signal started from
      * this class to a function or method in your code, and it will be executed in a new thread.
      */
    virtual void run();

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //THREAD_H
