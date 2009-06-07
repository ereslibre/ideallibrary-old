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

#include "timer.h"
#include "private/timer_p.h"

namespace IdealCore {

Timer::Timer(Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(timeout)
    , d(new PrivateImpl(this))
{
}

Timer::~Timer()
{
    delete d;
}

void Timer::start(TimeoutType timeoutType)
{
    d->start(timeoutType);
}

void Timer::stop()
{
    d->stop();
}

Timer::State Timer::state() const
{
    return d->m_state;
}

void Timer::setInterval(int msec)
{
    d->setInterval(msec);
}

int Timer::interval() const
{
    return d->interval();
}

void Timer::wait(int ms)
{
    Timer t;
    t.timedWait(ms);
}

Timer::Timer()
    : IDEAL_SIGNAL_INIT(timeout)
    , d(new PrivateImpl(this))
{
}

void Timer::timedWait(int ms) const
{
    return d->timedWait(ms);
}

}
