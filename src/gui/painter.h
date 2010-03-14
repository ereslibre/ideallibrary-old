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

    enum Antialias {
        DefaultAntialias = 0,
        NoneAntialias,
        GrayAntialias,
        SubpixelAntialias
    };

    enum FillRule {
        WindingFillRule = 0,
        EvenOddFillRule
    };

    enum LineCap {
        ButtLineCap = 0,
        RoundLineCap,
        SquareLineCap
    };

    enum LineJoin {
        MiterLineJoin = 0,
        RoundLineJoin,
        BevelLineJoin
    };

    void save();
    void restore();

    void setOperator(Operator op);
    Operator getOperator() const;

#if 0
    // TODO: write Pattern class
    void setSource(const Pattern &pattern);
    const Pattern &source() const;
#endif

    void setSourceRGB(ireal red, ireal green, ireal blue);
    void setSourceRGBA(ireal red, ireal green, ireal blue, ireal alpha);

    void setTolerance(ireal tolerance);
    ireal tolerance() const;

    void setAntialias(Antialias antialias);
    Antialias antialias() const;

    bool hasCurrentPoint() const;
    void currentPoint(ireal &x, ireal &y) const;

    void setFillRule(FillRule fillRule);
    FillRule fillRule() const;

    void setLineWidth(ireal width);
    ireal lineWidth() const;

    void setLineCap(LineCap lineCap);
    LineCap lineCap() const;

    void setLineJoin(LineJoin lineJoin);
    LineJoin lineJoin() const;

    void setDash(const ireal *dashes, iint32 numDashes, ireal offset);
    iint32 dashCount() const;
    void dash(ireal &dashes, ireal &offset);

    void setMiterLimit(ireal miterLimit);
    ireal miterLimit() const;

    void translate(ireal tx, ireal ty);

    void scale(ireal sx, ireal sy);

    void rotate(ireal angle);

    void showText(const IdealCore::String &text);

#if 0
    // TODO: write Matrix class
    void transform(const Matrix &matrix);

    void setMatrix(const Matrix &matrix);
    void matrix(Matrix &matrix) const;

    void identityMatrix();
#endif

    void newPath();

    void moveTo(ireal x, ireal y);

    void newSubPath();

    void lineTo(ireal x, ireal y);

    void curveTo(ireal x1, ireal y1, ireal x2, ireal y2, ireal x3, ireal y3);

    void arc(ireal xc, ireal yc, ireal radius, ireal angle1, ireal angle2);

    void arcNegative(ireal xc, ireal yc, ireal radius, ireal angle1, ireal angle2);

    void relMoveTo(ireal dx, ireal dy);

    void relLineTo(ireal dx, ireal dy);

    void relCurveTo(ireal dx1, ireal dy1, ireal dx2, ireal dy2, ireal dx3, ireal dy3);

    void rectangle(ireal x, ireal y, ireal width, ireal height);

    void closePath();

    void pathExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2);

    void paint();

    void paintWithAlpha(ireal alpha);

#if 0
    // TODO: write Pattern class
    void mask(const Pattern &pattern);
#endif

    void stroke();

    void strokePreserve();

    void fill();

    void fillPreserve();

    void copyPage();

    void showPage();

    void strokeExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2);

    void fillExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2);

    void resetClip();

    void clip();

    void preseve();

    void clipExtents(ireal &x1, ireal &y1, ireal &x2, ireal &y2);


private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //PAINTER_H
