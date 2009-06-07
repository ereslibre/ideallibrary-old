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

#include <algorithm>

#include <core/application.h>
#include "application_p.h"

#include <core/timer.h>
#include "timer_p.h"

namespace IdealCore {

Application::Private::Private(Application *q)
    : m_sleepTime(-1)
    , m_defaultSleepTime(500)
    , m_nextTimeout(-1)
    , q(q)
{
}

Application::Private::~Private()
{
}

void Application::Private::processEvents()
{
    int timeToSleep;
    if (m_sleepTime != -1) {
        checkTimers();
        timeToSleep = m_sleepTime;
    } else {
        m_runningTimerListMutex.lock();
        if (!m_runningTimerList.empty()) {
            m_sleepTime = m_runningTimerList.front()->d->m_interval;
            timeToSleep = m_sleepTime;
        } else {
            timeToSleep = m_defaultSleepTime;
        }
        m_runningTimerListMutex.unlock();
    }
    Timer::wait(timeToSleep);
}

void Application::Private::processDelayedDeletions()
{
    List<Object*>::iterator it;
    m_markedForDeletionMutex.lock();
    for (it = m_markedForDeletion.begin(); it != m_markedForDeletion.end(); ++it) {
        delete *it;
    }
    m_markedForDeletion.clear();
    m_markedForDeletionMutex.unlock();
}

bool Application::PrivateImpl::timerSort(const Timer *left, const Timer *right)
{
    return left->d->m_remaining < right->d->m_remaining;
}

void Application::Private::checkTimers()
{
    m_runningTimerListMutex.lock();
    if (m_runningTimerList.empty()) {
        return;
    }
    List<Timer*> expiredTimerList;
    {
        Timer *const firstExpiredTimer = m_runningTimerList.front();
        {
            // We control here the case in which the timer that was going to expire
            // (the head of the queue) was removed before actually expiring. With this
            // check we won't fire an incorrect timer when it had already time remaining.
            const int msDelta = firstExpiredTimer->d->m_remaining - m_sleepTime;
            if (msDelta) {
                std::vector<Timer*>::iterator it;
                for (it = m_runningTimerList.begin(); it != m_runningTimerList.end(); ++it) {
                    Timer *const currTimer = *it;
                    currTimer->d->m_remaining -= m_sleepTime;
                }
                m_sleepTime = std::min(msDelta, m_defaultSleepTime);
                m_runningTimerListMutex.unlock();
                return;
            }
        }
        m_nextTimeout = firstExpiredTimer->d->m_remaining;
        std::vector<Timer*>::iterator it = m_runningTimerList.begin();
        while (it != m_runningTimerList.end()) {
            Timer *const currTimer = *it;
            if (currTimer->d->m_remaining == firstExpiredTimer->d->m_remaining) {
                if (currTimer->d->m_timeoutType == Timer::SingleShot) {
                    currTimer->d->m_state = Timer::Stopped;
                    it = m_runningTimerList.erase(it);
                } else {
                    currTimer->d->m_remaining = currTimer->d->m_interval;
                    ++it;
                }
                expiredTimerList.push_back(currTimer);
            } else {
                break;
            }
        }
        if (!m_runningTimerList.empty()) {
            while (it != m_runningTimerList.end()) {
                Timer *const currTimer = *it;
                currTimer->d->m_remaining -= m_nextTimeout;
                ++it;
            }
        }
    }
    if (!m_runningTimerList.empty()) {
        std::sort(m_runningTimerList.begin(), m_runningTimerList.end(), PrivateImpl::timerSort);
        Timer *const nextTimer = m_runningTimerList.front();
        m_sleepTime = nextTimer->d->m_remaining;
    } else {
        m_sleepTime = -1;
    }
    m_runningTimerListMutex.unlock();
    List<Timer*>::iterator it;
    for (it = expiredTimerList.begin(); it != expiredTimerList.end(); ++it) {
        Timer *const currTimer = *it;
        currTimer->emit(currTimer->timeout);
    }
}

}

