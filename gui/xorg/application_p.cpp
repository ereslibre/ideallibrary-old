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
    switch (xe.type) {
    case CreateNotify_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::CreateNotify);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case MapNotify_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::MapNotify);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case UnmapNotify_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::UnmapNotify);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case MotionNotify_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::MotionNotify);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case EnterNotify_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::EnterNotify);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case LeaveNotify_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::LeaveNotify);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case ButtonPress_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::ButtonPress);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case ButtonRelease_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::ButtonRelease);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case KeyPress_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::KeyPress);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case KeyRelease_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::KeyRelease);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case ConfigureNotify_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::ConfigureNotify);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case Expose_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::Expose);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case FocusIn_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::FocusIn);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    case FocusOut_: {
        Widget *const widget = widgetMap[xe.xany.window];
        IdealCore::Event *event = new IdealCore::Event(widget, IdealCore::Event::FocusOut);
        GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
        guiEventDispatcher->postEvent(event);
        guiEventDispatcher->exec();
    }
        break;
    default:
        IDEAL_DEBUG("unknown event received: " << xe.type);
        break;
    }
}

}
