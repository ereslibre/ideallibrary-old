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

#include <cairo.h>

#include <X11/Xlib.h>
#include "fixincludes.h"

#include "application_p.h"
#include "widget_p.h"

namespace IdealGUI {

class Painter::Private
{
public:
    Private(Widget *canvas)
        : m_canvas(canvas)
    {
        IdealGUI::Application *app = static_cast<IdealGUI::Application*>(m_canvas->application());
        m_dpy = app->d->m_dpy;
        XClearWindow(m_dpy, canvas->d->m_window);
        m_cairo = cairo_create(canvas->d->m_cs);
        cairo_set_line_width(m_cairo, 1.0);
    }

    ~Private()
    {
        cairo_show_page(m_cairo);
        cairo_destroy(m_cairo);
    }

    void drawPoint(int x, int y);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawRectangle(int x, int y, int width, int height);
    void drawText(int x, int y, const IdealCore::String &text);
    void fillRectangle(int x, int y, int width, int height);

    Widget  *m_canvas;
    cairo_t *m_cairo;
    Display *m_dpy;
};

void Painter::Private::drawPoint(int x, int y)
{
}

void Painter::Private::drawLine(int x1, int y1, int x2, int y2)
{
    cairo_move_to(m_cairo, x1, y1);
    cairo_line_to(m_cairo, x2, y2);
    cairo_stroke(m_cairo);
}

void Painter::Private::drawRectangle(int x, int y, int width, int height)
{
    cairo_rectangle(m_cairo, x, y, width, height);
    cairo_stroke(m_cairo);
}

void Painter::Private::drawText(int x, int y, const IdealCore::String &text)
{
}

void Painter::Private::fillRectangle(int x, int y, int width, int height)
{
}

}
