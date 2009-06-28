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

#ifndef PAINTER_H
#define PAINTER_H

#include <ideal_export.h>
#include <core/ideal_string.h>

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

    void setPenColor(double red, double green, double blue, double alpha = 1.0);

    void drawPoint(double x, double y);
    void drawLine(double x1, double y1, double x2, double y2);
    void drawRectangle(double x, double y, double width, double height);
    void drawText(double x, double y, const IdealCore::String &text);
    void fillRectangle(double x, double y, double width, double height);

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //PAINTER_H
