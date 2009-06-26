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

#include "widget_p.h"
#include "application_p.h"

#include <cairo-xlib.h>

namespace IdealGUI {

Widget::PrivateImpl::PrivateImpl(Widget *q)
    : Private(q)
    , m_window(0)
    , m_cs(0)
{
}

Widget::PrivateImpl::~PrivateImpl()
{
    cairo_surface_destroy(m_cs);
}

void Widget::show(int x, int y, int width, int height)
{
    IdealGUI::Application *app = static_cast<IdealGUI::Application*>(application());
    IdealGUI::Application::PrivateImpl *a_d = static_cast<IdealGUI::Application::PrivateImpl*>(app->d);
    Display *dpy = a_d->m_dpy;
    if (!D_I->m_window) {
        if (!d->m_parentWidget) {
            D_I->m_window = XCreateSimpleWindow(dpy, XDefaultRootWindow(dpy), x, y, width, height, 0,
                                                 XWhitePixel(dpy, XDefaultScreen(dpy)),
                                                 XWhitePixel(dpy, XDefaultScreen(dpy)));
        } else {
            D_I->m_window = XCreateSimpleWindow(dpy, static_cast<IdealGUI::Widget::PrivateImpl*>(d->m_parentWidget->d)->m_window,
                                                 x, y, width, height, 0,
                                                 XWhitePixel(dpy, XDefaultScreen(dpy)),
                                                 XWhitePixel(dpy, XDefaultScreen(dpy)));
        }
        a_d->m_widgetMap[D_I->m_window] = this;
        XSelectInput(dpy, D_I->m_window, ExposureMask | ButtonPressMask | ButtonReleaseMask    |
                                          EnterWindowMask | LeaveWindowMask | PointerMotionMask |
                                          FocusChangeMask | KeyPressMask | KeyReleaseMask       |
                                          StructureNotifyMask | SubstructureNotifyMask);
    }
    XMapWindow(dpy, D_I->m_window);
    D_I->m_cs = cairo_xlib_surface_create(dpy, D_I->m_window, DefaultVisual(dpy, 0), width, height);
}

void Widget::hide()
{
}

}
