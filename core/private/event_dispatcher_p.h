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

#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include <ideal_export.h>
#include <core/thread.h>

namespace IdealCore {

class Event;

class IDEAL_EXPORT EventDispatcher
    : public Thread
{
public:
    EventDispatcher();
    virtual ~EventDispatcher();

    void postEvent(Event *event);

protected:
    virtual void run();

protected:
    Event *m_event;
};

}

#endif //EVENT_DISPATCHER_H
