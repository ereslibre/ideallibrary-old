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

class IDEAL_EXPORT Painter
{
public:
    Painter(Widget *widget);
    virtual ~Painter();

    void drawPoint(int x, int y);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawRectangle(int x, int y, int width, int height);
    void drawText(int x, int y, const IdealCore::String &text);
    void fillRectangle(int x, int y, int width, int height);

private:
    class Private;
    Private *const d;
};

}

#endif //PAINTER_H
