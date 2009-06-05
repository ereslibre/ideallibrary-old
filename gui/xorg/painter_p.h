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

#include <X11/Xlib.h>
#include <math.h>
#include "application_p.h"
#include "widget_p.h"

namespace IdealGUI {

class Painter::Private
{
public:
    Private(Widget *canvas)
        : m_canvas(canvas)
    {
        IdealGUI::Application *app = static_cast<IdealGUI::Application*>(canvas->application());
        Display *dpy = app->d->dpy;
        m_gc = XCreateGC(dpy, canvas->d->m_window, 0, 0);
    }

    ~Private()
    {
        IdealGUI::Application *app = static_cast<IdealGUI::Application*>(m_canvas->application());
        Display *dpy = app->d->dpy;
        XFreeGC(dpy, m_gc);
    }

    void drawPoint(int x, int y);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawRectangle(int x, int y, int width, int height);
    void drawText(int x, int y, const IdealCore::String &text);
    void fillRectangle(int x, int y, int width, int height);

    Widget *m_canvas;
    GC      m_gc;
};

void Painter::Private::drawPoint(int x, int y)
{
    IdealGUI::Application *app = static_cast<IdealGUI::Application*>(m_canvas->application());
    Display *dpy = app->d->dpy;
    XDrawPoint(dpy, m_canvas->d->m_window, m_gc, x, y);
}

void Painter::Private::drawLine(int x1, int y1, int x2, int y2)
{
    IdealGUI::Application *app = static_cast<IdealGUI::Application*>(m_canvas->application());
    Display *dpy = app->d->dpy;

    XPointDouble poly[4];
    XDouble dx = (x2 - x1);
    XDouble dy = (y2 - y1);
    XDouble len = sqrt(dx * dx + dy * dy);
    XDouble ldx = (0.6 / 2.0) * dy / len;
    XDouble ldy = (0.6 / 2.0) * dx / len;

    // bottom right corner
    poly[0].x = x1 + ldx;
    poly[0].y = y1 - ldy;

    // top right corner
    poly[1].x = x2 + ldx;
    poly[1].y = y2 - ldy;

    // top left corner
    poly[2].x = x2 - ldx;
    poly[2].y = y2 + ldy;

    // bottom left corner
    poly[3].x = x1 - ldx;
    poly[3].y = y1 + ldy;

    XRenderCompositeDoublePoly(dpy, PictOpOver, m_canvas->d->m_fillPicture, m_canvas->d->m_picture,
                               m_canvas->d->m_maskFormat, 0, 0, 0, 0, poly, 4, EvenOddRule);
}

void Painter::Private::drawRectangle(int x, int y, int width, int height)
{
   drawLine(x, y, x + width, y);
   drawLine(x + width, y, x + width, y + height);
   drawLine(x + width, y + height, x, y + height);
   drawLine(x, y + height, x, y);
}

void Painter::Private::drawText(int x, int y, const IdealCore::String &text)
{
}

void Painter::Private::fillRectangle(int x, int y, int width, int height)
{
}

}
