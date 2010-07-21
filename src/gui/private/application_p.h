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

#ifndef APPLICATION_P_H
#define APPLICATION_P_H

#include <core/thread.h>
#include <core/private/event_dispatcher_p.h>

namespace IdealGUI {

class Widget;

class Application::Private
{
public:
    Private(Application *q);
    virtual ~Private();

    void processEvents();

    Style           *m_style;

    class GUIEventHandler;
    GUIEventHandler *m_guiEventHandler;

    class GUIEventDispatcher;

    Application     *q;
};

class Application::Private::GUIEventHandler
    : public IdealCore::Thread
{
public:
    GUIEventHandler(Object *parent, Application::Private *priv);
    virtual ~GUIEventHandler();

protected:
    virtual void run();

public:
    Application::Private *priv;
};

class Application::Private::GUIEventDispatcher
    : public IdealCore::EventDispatcher
{
public:
    GUIEventDispatcher(Object *parent);

protected:
    virtual void run();
};

}

#include <gui/private/xorg/application_p.h>

#endif //APPLICATION_P_H
