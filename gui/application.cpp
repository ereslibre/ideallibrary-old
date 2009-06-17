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

#include "application.h"
#include "xorg/application_p.h"

#include <core/event.h>
#include <gui/widget.h>

namespace IdealGUI {

Application::Private::GUIEventHandler::GUIEventHandler(Application::Private *priv)
    : priv(priv)
{
}

Application::Private::GUIEventHandler::~GUIEventHandler()
{
}

void Application::Private::GUIEventHandler::run()
{
    while (true) {
        priv->processEvents();
    }
}

void Application::Private::GUIEventDispatcher::run()
{
    if (!m_event || !m_event->object()) {
        return;
    }
    Widget *const widget = static_cast<Widget*>(m_event->object());
    switch (m_event->type()) {
        case IdealCore::Event::CreateNotify:
        case IdealCore::Event::MapNotify:
        case IdealCore::Event::UnmapNotify:
        case IdealCore::Event::MotionNotify:
        case IdealCore::Event::EnterNotify:
        case IdealCore::Event::LeaveNotify:
        case IdealCore::Event::ButtonPress:
        case IdealCore::Event::ButtonRelease:
        case IdealCore::Event::KeyPress:
        case IdealCore::Event::KeyRelease:
        case IdealCore::Event::ConfigureNotify:
        case IdealCore::Event::Expose:
        case IdealCore::Event::FocusIn:
        case IdealCore::Event::FocusOut:
            widget->event(m_event);
            break;
        default:
            IDEAL_DEBUG_WARNING("Unexpected event type " << m_event->type());
            break;
    }
}

Application::Application(int argc, char **argv)
    : IdealCore::Application(argc, argv)
    , d(new Private(this))
{
}

Application::~Application()
{
    delete d;
}

int Application::exec()
{
    if (!d->guiEventHandler) {
        d->guiEventHandler = new Private::GUIEventHandler(d);
    }
    d->guiEventHandler->exec();
    return IdealCore::Application::exec();
}

}
