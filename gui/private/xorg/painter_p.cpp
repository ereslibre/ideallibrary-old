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

void Painter::setOperator(Operator op)
{
    cairo_operator_t cairoOperator;
    switch (op) {
        case ClearOperator:
            cairoOperator = CAIRO_OPERATOR_CLEAR;
            break;
        case SourceOperator:
            cairoOperator = CAIRO_OPERATOR_SOURCE;
            break;
        case OverOperator:
            cairoOperator = CAIRO_OPERATOR_OVER;
            break;
        case InOperator:
            cairoOperator = CAIRO_OPERATOR_IN;
            break;
        case OutOperator:
            cairoOperator = CAIRO_OPERATOR_OUT;
            break;
        case AtopOperator:
            cairoOperator = CAIRO_OPERATOR_ATOP;
            break;
        case DestOperator:
            cairoOperator = CAIRO_OPERATOR_DEST;
            break;
        case DestOverOperator:
            cairoOperator = CAIRO_OPERATOR_DEST_OVER;
            break;
        case DestInOperator:
            cairoOperator = CAIRO_OPERATOR_DEST_IN;
            break;
        case DestOutOperator:
            cairoOperator = CAIRO_OPERATOR_DEST_OUT;
            break;
        case DestAtopOperator:
            cairoOperator = CAIRO_OPERATOR_DEST_ATOP;
            break;
        case XorOperator:
            cairoOperator = CAIRO_OPERATOR_XOR;
            break;
        case AddOperator:
            cairoOperator = CAIRO_OPERATOR_ADD;
            break;
        case SaturateOperator:
            cairoOperator = CAIRO_OPERATOR_SATURATE;
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown painter operator");
            cairoOperator = CAIRO_OPERATOR_OVER;
            break;
    }
    cairo_set_operator(D_I->m_cairo, cairoOperator);
}

void Painter::setAntialias(Antialias antialias)
{
    cairo_antialias_t cairoAntialias;
    switch (antialias) {
        case DefaultAntialias:
            cairoAntialias = CAIRO_ANTIALIAS_DEFAULT;
            break;
        case NoneAntialias:
            cairoAntialias = CAIRO_ANTIALIAS_NONE;
            break;
        case GrayAntialias:
            cairoAntialias = CAIRO_ANTIALIAS_GRAY;
            break;
        case SubpixelAntialias:
            cairoAntialias = CAIRO_ANTIALIAS_SUBPIXEL;
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown antialias parameter");
            cairoAntialias = CAIRO_ANTIALIAS_DEFAULT;
            break;
    }
    cairo_set_antialias(D_I->m_cairo, cairoAntialias);
}

void Painter::saveState()
{
    cairo_save(D_I->m_cairo);
}

void Painter::restoreState()
{
    cairo_restore(D_I->m_cairo);
}

void Painter::setPenColor(double red, double green, double blue, double alpha)
{
    cairo_set_source_rgba(D_I->m_cairo, red, green, blue, alpha);
}

void Painter::setLineWidth(double width)
{
    cairo_set_line_width(D_I->m_cairo, width);
}

void Painter::setFillRule(FillRule fillRule)
{
    cairo_fill_rule_t cairoFillRule;
    switch (fillRule) {
        case WindingFillRule:
            cairoFillRule = CAIRO_FILL_RULE_WINDING;
            break;
        case EvenOddFillRule:
            cairoFillRule = CAIRO_FILL_RULE_EVEN_ODD;
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown fill rule parameter");
            cairoFillRule = CAIRO_FILL_RULE_WINDING;
            break;
    }
    cairo_set_fill_rule(D_I->m_cairo, cairoFillRule);
}

void Painter::setLineCap(LineCap lineCap)
{
    cairo_line_cap_t cairoLineCap;
    switch (lineCap) {
        case ButtLineCap:
            cairoLineCap = CAIRO_LINE_CAP_BUTT;
            break;
        case RoundLineCap:
            cairoLineCap = CAIRO_LINE_CAP_ROUND;
            break;
        case SquareLineCap:
            cairoLineCap = CAIRO_LINE_CAP_SQUARE;
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown line cap parameter");
            cairoLineCap = CAIRO_LINE_CAP_BUTT;
            break;
    }
    cairo_set_line_cap(D_I->m_cairo, cairoLineCap);
}

void Painter::setLineJoin(LineJoin lineJoin)
{
    cairo_line_join_t cairoLineJoin;
    switch (lineJoin) {
        case MiterLineJoin:
            cairoLineJoin = CAIRO_LINE_JOIN_MITER;
            break;
        case RoundLineJoin:
            cairoLineJoin = CAIRO_LINE_JOIN_ROUND;
            break;
        case BevelLineJoin:
            cairoLineJoin = CAIRO_LINE_JOIN_BEVEL;
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown line join parameter");
            cairoLineJoin = CAIRO_LINE_JOIN_MITER;
            break;
    }
    cairo_set_line_join(D_I->m_cairo, cairoLineJoin);
}

void Painter::drawPoint(const Point &point)
{
    drawRectangle(point, Size(2, 2));
}

void Painter::drawLine(const Point &point1, const Point &point2)
{
    cairo_move_to(D_I->m_cairo, point1.x(), point1.y());
    cairo_line_to(D_I->m_cairo, point2.x(), point2.y());
    cairo_stroke(D_I->m_cairo);
}

void Painter::drawRectangle(const Point &topLeft, const Size &size)
{
    cairo_rectangle(D_I->m_cairo, topLeft.x(), topLeft.y(), size.width(), size.height());
    cairo_stroke(D_I->m_cairo);
}

void Painter::drawText(const Point &bottomLeft, const IdealCore::String &text)
{
    cairo_move_to(D_I->m_cairo, bottomLeft.x(), bottomLeft.y());
    cairo_show_text(D_I->m_cairo, text.data());
}

void Painter::fillRectangle(const Point &topLeft, const Size &size)
{
}

}
