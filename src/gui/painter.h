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

#ifndef PAINTER_H
#define PAINTER_H

#include <ideal_export.h>
#include <core/ideal_string.h>
#include <gui/matrix.h>
#include <gui/point.h>
#include <gui/size.h>

namespace IdealGUI {

class Widget;

/**
  * @class Painter painter.h gui/painter.h
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Painter
{
public:
    Painter(Widget *widget);
    virtual ~Painter();

    enum Operator {
        ClearOperator = 0,
        SourceOperator,
        OverOperator,
        InOperator,
        OutOperator,
        AtopOperator,
        DestOperator,
        DestOverOperator,
        DestInOperator,
        DestOutOperator,
        DestAtopOperator,
        XorOperator,
        AddOperator,
        SaturateOperator
    };

    void setOperator(Operator op);

    enum Antialias {
        DefaultAntialias = 0,
        NoneAntialias,
        GrayAntialias,
        SubpixelAntialias
    };

    void setAntialias(Antialias antialias);

    void saveState();
    void restoreState();

    void setPenColor(ireal red, ireal green, ireal blue, ireal alpha = 1.0);
    void setLineWidth(ireal width);

    enum FillRule {
        WindingFillRule = 0,
        EvenOddFillRule
    };

    void setFillRule(FillRule fillRule);

    enum LineCap {
        ButtLineCap = 0,
        RoundLineCap,
        SquareLineCap
    };

    void setLineCap(LineCap lineCap);

    enum LineJoin {
        MiterLineJoin = 0,
        RoundLineJoin,
        BevelLineJoin
    };

    void setLineJoin(LineJoin lineJoin);

    void setDash(const ireal *dashes, iint32 numDashes, ireal offset);

    void setMiterLimit(ireal miterLimit);

    void translate(ireal tx, ireal ty);

    void scale(ireal sx, ireal sy);

    void rotate(ireal angle);

    void transform(const Matrix &matrix);

    void setMatrix(const Matrix &matrix);

    void drawPoint(const Point &point);
    void drawLine(const Point &point1, const Point &point2);
    void drawRectangle(const Point &topLeft, const Size &size);
    void drawText(const Point &bottomLeft, const IdealCore::String &text);
    void fillRectangle(const Point &topLeft, const Size &size);

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //PAINTER_H
