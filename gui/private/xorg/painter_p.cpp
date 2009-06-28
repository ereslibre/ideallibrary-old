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

#include "painter_p.h"

#include <cairo.h>

#include <X11/Xlib.h>
#include "fixincludes.h"

#include "application_p.h"
#include "widget_p.h"

namespace IdealGUI {

Painter::PrivateImpl::PrivateImpl(Widget *canvas)
    : Private(canvas)
{
    IdealGUI::Application *app = static_cast<IdealGUI::Application*>(m_canvas->application());
    m_dpy = static_cast<IdealGUI::Application::PrivateImpl*>(app->d)->m_dpy;
    Widget::PrivateImpl *const w_d = static_cast<IdealGUI::Widget::PrivateImpl*>(canvas->d);
    XClearWindow(m_dpy, w_d->m_window);
    m_cairo = cairo_create(w_d->m_cs);
    cairo_set_source_rgba(m_cairo, 0, 0, 0, 1.0);
    cairo_set_line_width(m_cairo, 2.0);
    cairo_select_font_face(m_cairo, "DejaVu Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(m_cairo, 12);
}

Painter::PrivateImpl::~PrivateImpl()
{
    cairo_destroy(m_cairo);
    XFlush(m_dpy);
}

void Painter::setPenColor(double red, double green, double blue, double alpha)
{
    cairo_set_source_rgba(D_I->m_cairo, red, green, blue, alpha);
}

void Painter::drawPoint(double x, double y)
{
    drawRectangle(x, y, 2, 2);
}

void Painter::drawLine(double x1, double y1, double x2, double y2)
{
    cairo_move_to(D_I->m_cairo, x1, y1);
    cairo_line_to(D_I->m_cairo, x2, y2);
    cairo_stroke(D_I->m_cairo);
}

void Painter::drawRectangle(double x, double y, double width, double height)
{
    cairo_rectangle(D_I->m_cairo, x, y, width, height);
    cairo_stroke(D_I->m_cairo);
}

void Painter::drawText(double x, double y, const IdealCore::String &text)
{
    cairo_move_to(D_I->m_cairo, x, y);
    cairo_show_text(D_I->m_cairo, text.data());
}

void Painter::fillRectangle(double x, double y, double width, double height)
{
}

}
