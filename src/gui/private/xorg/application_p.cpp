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

#include <core/event.h>
#include "application_p.h"

#include <gui/widget.h>

namespace IdealGUI {

Application::PrivateImpl::PrivateImpl(Application *q)
    : Private(q)
{
    XInitThreads();
    m_dpy = XOpenDisplay("");
}

Application::PrivateImpl::~PrivateImpl()
{
    XCloseDisplay(m_dpy);
}

void Application::Private::processEvents()
{
    IdealGUI::Application::PrivateImpl *const d = static_cast<IdealGUI::Application::PrivateImpl*>(this);
    XEvent xe;
    XNextEvent(d->m_dpy, &xe);
    IdealCore::Event *event = 0;
    Widget *const widget = d->m_widgetMap[xe.xany.window];
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
            if (xe.xexpose.count >= 1) {
                return;
            }
            event = new IdealCore::Event(widget, IdealCore::Event::Expose);
            break;
        case FocusIn_:
            event = new IdealCore::Event(widget, IdealCore::Event::FocusIn);
            break;
        case FocusOut_:
            event = new IdealCore::Event(widget, IdealCore::Event::FocusOut);
            break;
        default:
            IDEAL_DEBUG("unknown event received: " << xe.type);
            return;
    }
    GUIEventDispatcher *guiEventDispatcher = new GUIEventDispatcher;
    guiEventDispatcher->postEvent(event);
    guiEventDispatcher->exec();
}

}
