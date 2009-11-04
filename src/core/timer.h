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

#ifndef TIMER_H
#define TIMER_H

#include <ideal_export.h>
#include <core/object.h>

namespace IdealCore {

/**
  * @class Timer timer.h core/timer.h
  *
  * This class lets you set you an interval, that when is caught a timeout signal is emitted.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Timer
    : public Object
{
    friend class Application;

public:
    Timer(Object *parent);
    virtual ~Timer();

    enum State {
        Stopped = 0,    ///< This timer is stopped
        Running         ///< This timer is running
    };

    enum TimeoutType {
        SingleShot = 0, ///< This timer will not restart after a timeout
        NoSingleShot    ///< This timer will restart after a timeout until manually stopped
    };

    /**
      * Starts or restarts the timer.
      *
      * @note if this timer is already started, it will be restarted.
      *
      * @param timeoutOnce If true, this timer will expire only one time. Otherwise it will
      *                    expire continuously every interval() msec until stopped.
      */
    void start(TimeoutType timeoutType = SingleShot);

    /**
      * Stops the timer.
      */
    void stop();

    /**
      * @return the state of the timer
      */
    State state() const;

    /**
      * Sets the interval of timeout of this timer
      */
    void setInterval(iint32 msec);

    /**
      * @return the interval of timeout of this timer
      */
    iint32 interval() const;

    /**
      * Will pause the current thread for @p ms milliseconds
      */
    static void wait(iint32 ms);

    /**
      * Calls @p member in @p receiver after @p ms milliseconds
      */
    template <typename Receiver, typename Member>
    static void callAfter(iint32 ms, Receiver *receiver, Member member);

    /**
      * Calls the static member @p member after @p ms milliseconds
      */
    template <typename Member>
    static void callStaticAfter(iint32 ms, Member member);

public:
    /**
      * This timer has finished its interval.
      */
    IDEAL_SIGNAL(timeout);

private:
    Timer();
    void timedWait(iint32 ms) const;

    class Private;
    class PrivateImpl;
    Private *const d;
};

template <typename Receiver, typename Member>
void Timer::callAfter(iint32 ms, Receiver *receiver, Member member)
{
    Timer *timer = new Timer(receiver);
    connect(timer->timeout, receiver, member);
    connect(timer->timeout, timer, &Object::deleteLater);
    timer->setInterval(ms);
    timer->start();
}

template <typename Member>
void Timer::callStaticAfter(iint32 ms, Member member)
{
    Timer *timer = new Timer;
    connectStatic(timer->timeout, member);
    connect(timer->timeout, timer, &Object::deleteLater);
    timer->setInterval(ms);
    timer->start();
}

}

#endif //TIMER_H
