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

void Painter::save()
{
    cairo_save(D_I->m_cairo);
}

void Painter::restore()
{
    cairo_restore(D_I->m_cairo);
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

Painter::Operator Painter::getOperator() const
{
    switch (cairo_get_operator(D_I->m_cairo)) {
        case CAIRO_OPERATOR_CLEAR:
            return ClearOperator;
        case CAIRO_OPERATOR_SOURCE:
            return SourceOperator;
        case CAIRO_OPERATOR_OVER:
            return OverOperator;
        case CAIRO_OPERATOR_IN:
            return InOperator;
        case CAIRO_OPERATOR_OUT:
            return OutOperator;
        case CAIRO_OPERATOR_ATOP:
            return AtopOperator;
        case CAIRO_OPERATOR_DEST:
            return DestOperator;
        case CAIRO_OPERATOR_DEST_OVER:
            return DestOverOperator;
        case CAIRO_OPERATOR_DEST_IN:
            return DestInOperator;
        case CAIRO_OPERATOR_DEST_OUT:
            return DestOutOperator;
        case CAIRO_OPERATOR_DEST_ATOP:
            return DestAtopOperator;
        case CAIRO_OPERATOR_XOR:
            return XorOperator;
        case CAIRO_OPERATOR_ADD:
            return AddOperator;
        case CAIRO_OPERATOR_SATURATE:
            return SaturateOperator;
        default:
            IDEAL_DEBUG_WARNING("unknown painter operator");
            break;
    }
    return OverOperator;
}

#if 0
    // TODO: write Pattern class
void Painter::setSource(const Pattern &pattern)
{
}

const Pattern &Painter::source() const
{
}
#endif

void Painter::setSourceRGB(ireal red, ireal green, ireal blue)
{
    cairo_set_source_rgb(D_I->m_cairo, red, green, blue);
}

void Painter::setSourceRGBA(ireal red, ireal green, ireal blue, ireal alpha)
{
    cairo_set_source_rgba(D_I->m_cairo, red, green, blue, alpha);
}

void Painter::setTolerance(ireal tolerance)
{
    cairo_set_tolerance(D_I->m_cairo, tolerance);
}

ireal Painter::tolerance() const
{
    return cairo_get_tolerance(D_I->m_cairo);
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

Painter::Antialias Painter::antialias() const
{
    switch (cairo_get_antialias(D_I->m_cairo)) {
        case CAIRO_ANTIALIAS_DEFAULT:
            return DefaultAntialias;
        case CAIRO_ANTIALIAS_NONE:
            return NoneAntialias;
        case CAIRO_ANTIALIAS_GRAY:
            return GrayAntialias;
        case CAIRO_ANTIALIAS_SUBPIXEL:
            return SubpixelAntialias;
        default:
            IDEAL_DEBUG_WARNING("unknown antialias parameter");
            break;
    }
    return DefaultAntialias;
}

bool Painter::hasCurrentPoint() const
{
    return cairo_has_current_point(D_I->m_cairo);
}

void Painter::currentPoint(ireal &x, ireal &y) const
{
    cairo_get_current_point(D_I->m_cairo, &x, &y);
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

Painter::FillRule Painter::fillRule() const
{
    switch (cairo_get_fill_rule(D_I->m_cairo)) {
        case CAIRO_FILL_RULE_WINDING:
            return WindingFillRule;
        case CAIRO_FILL_RULE_EVEN_ODD:
            return EvenOddFillRule;
        default:
            IDEAL_DEBUG_WARNING("unknown fill rule parameter");
            break;
    }
    return WindingFillRule;
}

void Painter::setLineWidth(ireal width)
{
    cairo_set_line_width(D_I->m_cairo, width);
}

ireal Painter::lineWidth() const
{
    return cairo_get_line_width(D_I->m_cairo);
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

Painter::LineCap Painter::lineCap() const
{
    switch (cairo_get_line_cap(D_I->m_cairo)) {
        case CAIRO_LINE_CAP_BUTT:
            return ButtLineCap;
        case CAIRO_LINE_CAP_ROUND:
            return RoundLineCap;
        case CAIRO_LINE_CAP_SQUARE:
            return SquareLineCap;
        default:
            IDEAL_DEBUG_WARNING("unknown line cap parameter");
            break;
    }
    return ButtLineCap;
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

Painter::LineJoin Painter::lineJoin() const
{
    switch (cairo_get_line_join(D_I->m_cairo)) {
        case MiterLineJoin:
            return MiterLineJoin;
        case CAIRO_LINE_JOIN_ROUND:
            return RoundLineJoin;
        case CAIRO_LINE_JOIN_BEVEL:
            return BevelLineJoin;
        default:
            IDEAL_DEBUG_WARNING("unknown line join parameter");
            break;
    }
    return MiterLineJoin;
}

void Painter::setDash(const ireal *dashes, iint32 numDashes, ireal offset)
{
    cairo_set_dash(D_I->m_cairo, dashes, numDashes, offset);
}

iint32 Painter::dashCount() const
{
    return cairo_get_dash_count(D_I->m_cairo);
}

void Painter::dash(ireal &dashes, ireal &offset)
{
    cairo_get_dash(D_I->m_cairo, &dashes, &offset);
}

void Painter::setMiterLimit(ireal miterLimit)
{
    cairo_set_miter_limit(D_I->m_cairo, miterLimit);
}

ireal Painter::miterLimit() const
{
    return cairo_get_miter_limit(D_I->m_cairo);
}

void Painter::translate(ireal tx, ireal ty)
{
    cairo_translate(D_I->m_cairo, tx, ty);
}

void Painter::scale(ireal sx, ireal sy)
{
    cairo_scale(D_I->m_cairo, sx, sy);
}

void Painter::rotate(ireal angle)
{
    cairo_rotate(D_I->m_cairo, angle);
}

void Painter::showText(const IdealCore::String &text)
{
    cairo_show_text(D_I->m_cairo, text.data());
}

#if 0
// TODO: write Matrix class
void Painter::transform(const Matrix &matrix)
{
}

void Painter::setMatrix(const Matrix &matrix)
{
}

void Painter::matrix(Matrix &matrix) const
{
}

void Painter::identityMatrix()
{
}
#endif

void Painter::newPath()
{
    cairo_new_path(D_I->m_cairo);
}

void Painter::moveTo(ireal x, ireal y)
{
    cairo_move_to(D_I->m_cairo, x, y);
}

void Painter::newSubPath()
{
    cairo_new_sub_path(D_I->m_cairo);
}

void Painter::lineTo(ireal x, ireal y)
{
    cairo_line_to(D_I->m_cairo, x, y);
}

void Painter::curveTo(ireal x1, ireal y1, ireal x2, ireal y2, ireal x3, ireal y3)
{
    cairo_curve_to(D_I->m_cairo, x1, y1, x2, y2, x3, y3);
}

void Painter::arc(ireal xc, ireal yc, ireal radius, ireal angle1, ireal angle2)
{
    cairo_arc(D_I->m_cairo, xc, yc, radius, angle1, angle2);
}

void Painter::arcNegative(ireal xc, ireal yc, ireal radius, ireal angle1, ireal angle2)
{
    cairo_arc_negative(D_I->m_cairo, xc, yc, radius, angle1, angle2);
}

void Painter::relMoveTo(ireal dx, ireal dy)
{
    cairo_rel_move_to(D_I->m_cairo, dx, dy);
}

void Painter::relLineTo(ireal dx, ireal dy)
{
    cairo_rel_line_to(D_I->m_cairo, dx, dy);
}

void Painter::relCurveTo(ireal dx1, ireal dy1, ireal dx2, ireal dy2, ireal dx3, ireal dy3)
{
    cairo_rel_curve_to(D_I->m_cairo, dx1, dy1, dx2, dy2, dx3, dy3);
}

void Painter::rectangle(ireal x, ireal y, ireal width, ireal height)
{
    cairo_rectangle(D_I->m_cairo, x, y, width, height);
}

void Painter::closePath()
{
    cairo_close_path(D_I->m_cairo);
}

void Painter::pathExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2)
{
    cairo_path_extents(D_I->m_cairo, &x1, &y1, &x2, &y2);
}

void Painter::paint()
{
    cairo_paint(D_I->m_cairo);
}

void Painter::paintWithAlpha(ireal alpha)
{
    cairo_paint_with_alpha(D_I->m_cairo, alpha);
}

#if 0
// TODO: write Pattern class
void Painter::mask(const Pattern &pattern)
{
}
#endif

void Painter::stroke()
{
    cairo_stroke(D_I->m_cairo);
}

void Painter::strokePreserve()
{
    cairo_stroke_preserve(D_I->m_cairo);
}

void Painter::fill()
{
    cairo_fill(D_I->m_cairo);
}

void Painter::fillPreserve()
{
    cairo_fill_preserve(D_I->m_cairo);
}

void Painter::copyPage()
{
    cairo_copy_page(D_I->m_cairo);
}

void Painter::showPage()
{
    cairo_show_page(D_I->m_cairo);
}

void Painter::strokeExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2)
{
    cairo_stroke_extents(D_I->m_cairo, &x1, &y1, &x2, &y2);
}

void Painter::fillExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2)
{
    cairo_fill_extents(D_I->m_cairo, &x1, &y1, &x2, &y2);
}

void Painter::resetClip()
{
    cairo_reset_clip(D_I->m_cairo);
}

void Painter::clip()
{
    cairo_clip(D_I->m_cairo);
}

void Painter::clipPreserve()
{
    cairo_clip_preserve(D_I->m_cairo);
}

void Painter::clipExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2)
{
    cairo_clip_extents(D_I->m_cairo, &x1, &y1, &x2, &y2);
}

}
