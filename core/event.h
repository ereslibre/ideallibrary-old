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

#ifndef EVENT_H
#define EVENT_H

#include <ideal_export.h>

namespace IdealCore {

class Object;

/**
  * @class Event event.h core/event.h
  *
  * Base class for all events.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Event
{
public:
    enum Type {
        CreateNotify,
        MapNotify,
        UnmapNotify,
        MotionNotify,
        EnterNotify,
        LeaveNotify,
        ButtonPress,
        ButtonRelease,
        KeyPress,
        KeyRelease,
        ConfigureNotify,
        Expose,
        FocusIn,
        FocusOut
    };

    Event(Object *object, Type type);
    virtual ~Event();

    /**
      * @return the object of the event
      */
    Object *object() const;

    /**
      * @return the type of the event
      */
    Type type() const;

private:
    class Private;
    Private *const d;
};

}

#endif //EVENT_H
