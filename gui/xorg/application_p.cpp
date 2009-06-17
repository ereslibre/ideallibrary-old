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

#include <core/event.h>
#include "application_p.h"
#include <gui/widget.h>

namespace IdealGUI {

Application::Private::Private(Application *q)
    : q(q)
    , guiEventHandler(0)
{
    dpy = XOpenDisplay("");
}

Application::Private::~Private()
{
    XCloseDisplay(dpy);
    delete guiEventHandler;
}

void Application::Private::processEvents()
{
    XEvent xe;
    XNextEvent(dpy, &xe);
    Widget *const widget = widgetMap[xe.xany.window];
    GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
    IdealCore::Event *event = 0;
    switch (xe.type) {
        case CreateNotify_:
            event = new IdealCore::Event(widget, IdealCore::Event::CreateNotify);
            break;
        case MapNotify_:
            event = new IdealCore::Event(widget, IdealCore::Event::MapNotify);
            break;
        case UnmapNotify_:
            event = new IdealCore::Event(widget, IdealCore::Event::UnmapNotify);
            break;
        case MotionNotify_:
            event = new IdealCore::Event(widget, IdealCore::Event::MotionNotify);
            break;
        case EnterNotify_:
            event = new IdealCore::Event(widget, IdealCore::Event::EnterNotify);
            break;
        case LeaveNotify_:
            event = new IdealCore::Event(widget, IdealCore::Event::LeaveNotify);
            break;
        case ButtonPress_:
            event = new IdealCore::Event(widget, IdealCore::Event::ButtonPress);
            break;
        case ButtonRelease_:
            event = new IdealCore::Event(widget, IdealCore::Event::ButtonRelease);
            break;
        case KeyPress_:
            event = new IdealCore::Event(widget, IdealCore::Event::KeyPress);
            break;
        case KeyRelease_:
            event = new IdealCore::Event(widget, IdealCore::Event::KeyRelease);
            break;
        case ConfigureNotify_:
            event = new IdealCore::Event(widget, IdealCore::Event::ConfigureNotify);
            break;
        case Expose_:
            event = new IdealCore::Event(widget, IdealCore::Event::Expose);
            break;
        case FocusIn_:
            event = new IdealCore::Event(widget, IdealCore::Event::FocusIn);
            break;
        case FocusOut_:
            event = new IdealCore::Event(widget, IdealCore::Event::FocusOut);
            break;
        default:
            delete guiEventDispatcher;
            IDEAL_DEBUG("unknown event received: " << xe.type);
            return;
    }
    guiEventDispatcher->postEvent(event);
    guiEventDispatcher->exec();
}

}
