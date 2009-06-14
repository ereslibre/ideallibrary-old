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

#include "application.h"
#include "private/application_p.h"

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
    if (m_state == Running) {
        q->stop();
    }
}

bool Timer::Private::listContains() const
{
    Application::Private *const app_d = q->application()->d;
    ContextMutexLocker cml(app_d->m_runningTimerListMutex);
    if (app_d->m_runningTimerList.empty()) {
        return false;
    }
    std::vector<Timer*>::const_iterator it = app_d->m_runningTimerList.begin();
    while (it != app_d->m_runningTimerList.end()) {
        if (*it == q) {
            return true;
        }
        ++it;
    }
    return false;
}

Timer::Timer(Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(timeout)
    , d(new PrivateImpl(this))
{
}

Timer::Timer()
    : IDEAL_SIGNAL_INIT(timeout)
    , d(new PrivateImpl(this))
{
}

Timer::~Timer()
{
    delete d;
}

void Timer::start(TimeoutType timeoutType)
{
    d->m_timeoutType = timeoutType;
    d->m_remaining = d->m_interval;
    d->m_state = Running;
    Application::Private *const app_d = application()->d;
    if (!d->listContains()) {
        std::vector<Timer*>::iterator it;
        ContextMutexLocker cml(app_d->m_runningTimerListMutex);
        for (it = app_d->m_runningTimerList.begin(); it != app_d->m_runningTimerList.end(); ++it) {
            Timer *currTimer = *it;
            if (currTimer->d->m_remaining < d->m_interval) {
                continue;
            }
            app_d->m_runningTimerList.insert(it, this);
            return;
        }
        app_d->m_runningTimerList.push_back(this);
    }
}

void Timer::stop()
{
    d->m_state = Stopped;
    Application::Private *const app_d = application()->d;
    std::vector<Timer*>::iterator it = app_d->m_runningTimerList.begin();
    while (it != app_d->m_runningTimerList.end()) {
        if (*it == this) {
            app_d->m_runningTimerList.erase(it);
            break;
        }
        ++it;
    }
}

Timer::State Timer::state() const
{
    return d->m_state;
}

void Timer::setInterval(int msec)
{
    d->m_interval = msec;
}

int Timer::interval() const
{
    return d->m_interval;
}

void Timer::wait(int ms)
{
    Timer t;
    t.timedWait(ms);
}

}
