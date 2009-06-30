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

#ifndef SIZE_H
#define SIZE_H

#include <ideal_export.h>

namespace IdealGUI {

class IDEAL_EXPORT Size
{
public:
    Size(double width, double height);
    Size(const Size &size);
    ~Size();

    double width() const;
    void setWidth(double width);

    double height() const;
    void setHeight(double height);

    double &rwidth() const;
    double &rheight() const;

    bool operator==(const Size &size);
    bool operator!=(const Size &size);
    Size &operator=(const Size &size);
    Size operator+(const Size &size);
    Size operator-(const Size &size);
    Size operator*(const Size &size);
    Size operator/(const Size &size);
    Size &operator+=(const Size &size);
    Size &operator-=(const Size &size);
    Size &operator*=(const Size &size);
    Size &operator/=(const Size &size);

private:
    class Private;
    Private *const d;
};

}

#endif //SIZE_H
