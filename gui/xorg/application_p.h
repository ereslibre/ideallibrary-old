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

#include <map>
#include <iostream>
#include <X11/Xlib.h>
#include "fixincludes.h"

#include <core/private/event_dispatcher.h>

#include <gui/application.h>

namespace IdealGUI {

class Widget;

class Application::Private
{
public:
    Private(Application *q);
    ~Private();

    void processEvents();

    Display                  *m_dpy;
    std::map<Window, Widget*> m_widgetMap;
    Application              *q;

    class GUIEventHandler;
    GUIEventHandler          *m_guiEventHandler;

    class GUIEventDispatcher;
};

class Application::Private::GUIEventHandler
    : public IdealCore::Thread
{
public:
    GUIEventHandler(Application::Private *priv);
    ~GUIEventHandler();

protected:
    virtual void run();

public:
    Application::Private *priv;
};

class Application::Private::GUIEventDispatcher
    : public IdealCore::EventDispatcher
{
protected:
    virtual void run();
};

}
