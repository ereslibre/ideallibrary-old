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

#ifndef PATH_H
#define PATH_H

#include <ideal_export.h>

namespace IdealGUI {

/**
  * @class Path path.h gui/path.h
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Path
{
public:
    Path();
    ~Path();

    enum Type {
        Absolute = 0,
        Relative
    };

    void moveTo(double x, double y, Type type = Absolute);
    void lineTo(double x, double y, Type type = Absolute);
    void curveTo(double x1, double y1, double x2, double y2, double x3, double y3, Type type = Absolute);
    void arc(double xc, double yc, double radius, double angle1, double angle2);
    void arcNegative(double xc, double yc, double radius, double angle1, double angle2);
    void rectangle(double x, double y, double width, double height);

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //PATH_H
