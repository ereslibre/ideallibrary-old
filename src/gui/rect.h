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

#ifndef RECT_H
#define RECT_H

#include <ideal_export.h>

#include <gui/size.h>
#include <gui/point.h>

namespace IdealGUI {

class IDEAL_EXPORT Rect
{
public:
    Rect(const Point &topLeft, const Size &size);
    Rect(const Rect &rect);
    virtual ~Rect();

    Point topLeft() const;
    void setTopLeft(const Point &topLeft);

    Size size() const;
    void setSize(const Size &size);

    Point topRight() const;
    Point bottomLeft() const;
    Point bottomRight() const;

    bool operator==(const Rect &rect);
    bool operator!=(const Rect &rect);
    Rect &operator=(const Rect &rect);

private:
    class Private;
    Private *const d;
};

}

#endif //RECT_H
