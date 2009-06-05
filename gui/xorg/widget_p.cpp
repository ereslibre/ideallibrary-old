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

namespace IdealGUI {

Widget::Private::Private(Widget *q)
    : m_window(0)
    , q(q)
{
}

Widget::Private::~Private()
{
}

void Widget::Private::show(int x, int y, int width, int height)
{
    IdealGUI::Application *app = static_cast<IdealGUI::Application*>(q->application());
    Display *dpy = app->d->dpy;
    if (!m_window) {
        if (!m_parentWidget) {
            m_window = XCreateSimpleWindow(dpy, XDefaultRootWindow(dpy), x, y, width, height, 0,
                                           XWhitePixel(dpy, XDefaultScreen(dpy)),
                                           XWhitePixel(dpy, XDefaultScreen(dpy)));
        } else {
            m_window = XCreateSimpleWindow(dpy, m_parentWidget->d->m_window, x, y, width, height, 0,
                                           XWhitePixel(dpy, XDefaultScreen(dpy)),
                                           XWhitePixel(dpy, XDefaultScreen(dpy)));
        }
        app->d->widgetMap[m_window] = q;
        XSelectInput(dpy, m_window, ExposureMask | ButtonPressMask | ButtonReleaseMask |
                                    EnterWindowMask | LeaveWindowMask | PointerMotionMask |
                                    FocusChangeMask | KeyPressMask | KeyReleaseMask |
                                    StructureNotifyMask | SubstructureNotifyMask);
    }
    {
        XRenderColor color;
        color.red = 0xFFFF;
        color.green = 0;
        color.blue = 0;
        color.alpha = 0xFFFF;
        m_fillPicture = XRenderCreateSolidFill(dpy, &color);
    }
    {
        XRenderPictFormat *format = XRenderFindStandardFormat(dpy, PictStandardRGB24);
        XRenderPictureAttributes attr;
        attr.poly_edge = PolyEdgeSmooth;
        attr.poly_mode = PolyModePrecise;
        m_picture = XRenderCreatePicture(dpy, m_window, format, 0, &attr);
    }
    {
        m_maskFormat = XRenderFindStandardFormat(dpy, PictStandardA8);
    }
    XMapWindow(dpy, m_window);
    XFlush(dpy);
}

void Widget::Private::hide()
{
    IdealGUI::Application *app = static_cast<IdealGUI::Application*>(q->application());
    Display *dpy = app->d->dpy;
    XUnmapWindow(dpy, m_window);
    XFlush(dpy);
}

}
