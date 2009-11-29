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
    virtual ~Path();

    enum Type {
        Absolute = 0,
        Relative
    };

    void moveTo(ireal x, ireal y, Type type = Absolute);
    void lineTo(ireal x, ireal y, Type type = Absolute);
    void curveTo(ireal x1, ireal y1, ireal x2, ireal y2, ireal x3, ireal y3, Type type = Absolute);
    void arc(ireal xc, ireal yc, ireal radius, ireal angle1, ireal angle2);
    void arcNegative(ireal xc, ireal yc, ireal radius, ireal angle1, ireal angle2);
    void rectangle(ireal x, ireal y, ireal width, ireal height);

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //PATH_H
