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

#include "event_dispatcher.h"
#include <core/ideal_list.h>
#include <core/condvar.h>

namespace IdealCore {

class EventDispatcher::Private
{
public:
    Private()
        : m_eventListCondVar(m_eventListMutex)
    {
    }

    List<Event*> m_eventList;
    Mutex        m_eventListMutex;
    CondVar      m_eventListCondVar;
};

EventDispatcher::EventDispatcher()
    : d(new Private)
{
}

EventDispatcher::~EventDispatcher()
{
    delete d;
}

void EventDispatcher::postEvent(Event *event)
{
    ContextMutexLocker cml(d->m_eventListMutex);
    d->m_eventList.push_back(event);
}

void EventDispatcher::run()
{
}

}
