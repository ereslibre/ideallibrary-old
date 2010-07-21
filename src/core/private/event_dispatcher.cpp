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

#include "event_dispatcher_p.h"

#include <core/list.h>
#include <core/cond_var.h>
#include <core/timer.h>
#include <core/event.h>

namespace IdealCore {

EventDispatcher::EventDispatcher()
    : Thread(NoJoinable)
    , m_event(0)
{
}

EventDispatcher::~EventDispatcher()
{
    delete m_event;
}

void EventDispatcher::postEvent(Event *event)
{
    m_event = event;
}

void EventDispatcher::run()
{
    if (!m_event || !m_event->object()) {
        return;
    }
    switch (m_event->type()) {
        case Event::Timeout: {
            Timer *const timer = static_cast<Timer*>(m_event->object());
            timer->emit(timer->timeout);
        }
            break;
        default:
            IDEAL_DEBUG_WARNING("Unexpected event type " << m_event->type());
            break;
    }
}

}
