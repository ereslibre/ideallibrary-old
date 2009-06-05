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

#include <signal.h>
#include <semaphore.h>
#include <sys/time.h>

#include <core/timer.h>
#include "timer_p.h"

#include <core/application.h>
#include "application_p.h"

namespace IdealCore {

Timer::PrivateImpl::PrivateImpl(Timer *q)
    : Private(q)
{
}

Timer::PrivateImpl::~PrivateImpl()
{
    if (m_state == Running) {
        stop();
    }
}

void Timer::Private::start(TimeoutType timeoutType)
{
    m_timeoutType = timeoutType;
    m_remaining = m_interval;
    m_state = Running;
    Application::Private *const app_d = q->application()->d;
    if (!listContains()) {
        std::vector<Timer*>::iterator it;
        for (it = app_d->m_runningTimerList.begin(); it != app_d->m_runningTimerList.end(); ++it) {
            Timer *currTimer = *it;
            if (currTimer->d->m_remaining < m_interval) {
                continue;
            }
            app_d->m_runningTimerList.insert(it, q);
            return;
        }
        app_d->m_runningTimerList.push_back(q);
    }
}

void Timer::Private::stop()
{
    m_state = Stopped;
    Application::Private *const app_d = q->application()->d;
    std::vector<Timer*>::iterator it = app_d->m_runningTimerList.begin();
    while (it != app_d->m_runningTimerList.end()) {
        if (*it == q) {
            app_d->m_runningTimerList.erase(it);
            break;
        }
        ++it;
    }
}

}

