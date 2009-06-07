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

#include <core/timer.h>
#include "timer_p.h"

#include <core/application.h>
#include "application_p.h"

namespace IdealCore {

Timer::Private::Private(Timer *q)
    : m_timeoutType(SingleShot)
    , m_interval(1000)
    , m_remaining(0)
    , m_state(Stopped)
    , q(q)
{
}

Timer::Private::~Private()
{
}

void Timer::Private::setInterval(int msec)
{
    m_interval = msec;
}

int Timer::Private::interval() const
{
    return m_interval;
}

bool Timer::Private::listContains() const
{
    Application::Private *const app_d = q->application()->d;
    app_d->m_runningTimerListMutex.lock();
    if (app_d->m_runningTimerList.empty()) {
        app_d->m_runningTimerListMutex.unlock();
        return false;
    }
    std::vector<Timer*>::const_iterator it = app_d->m_runningTimerList.begin();
    while (it != app_d->m_runningTimerList.end()) {
        if (*it == q) {
            app_d->m_runningTimerListMutex.unlock();
            return true;
        }
        ++it;
    }
    app_d->m_runningTimerListMutex.unlock();
    return false;
}

}

