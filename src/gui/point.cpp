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

#include "point.h"

namespace IdealGUI {

class Point::Private
{
public:
    Private(double x, double y);

    double m_x;
    double m_y;
};

Point::Private::Private(double x, double y)
    : m_x(x)
    , m_y(y)
{
}

Point::Point(double x, double y)
    : d(new Private(x, y))
{
}

Point::Point(const Point &point)
    : d(new Private(point.d->m_x, point.d->m_y))
{
}

Point::~Point()
{
    delete d;
}

double Point::x() const
{
    return d->m_x;
}

void Point::setX(double x)
{
    d->m_x = x;
}

double Point::y() const
{
    return d->m_y;
}

void Point::setY(double y)
{
    d->m_y = y;
}

double &Point::rx() const
{
    return d->m_x;
}

double &Point::ry() const
{
    return d->m_y;
}

bool Point::operator==(const Point &point)
{
    return d->m_x == point.d->m_x && d->m_y == point.d->m_y;
}

bool Point::operator!=(const Point &point)
{
    return d->m_x != point.d->m_x || d->m_y != point.d->m_y;
}

Point &Point::operator=(const Point &point)
{
    d->m_x = point.d->m_x;
    d->m_y = point.d->m_y;
    return *this;
}

Point Point::operator+(const Point &point)
{
    return Point(d->m_x + point.d->m_x, d->m_y + point.d->m_y);
}

Point Point::operator-(const Point &point)
{
    return Point(d->m_x - point.d->m_x, d->m_y - point.d->m_y);
}

Point Point::operator*(const Point &point)
{
    return Point(d->m_x * point.d->m_x, d->m_y * point.d->m_y);
}

Point Point::operator/(const Point &point)
{
    return Point(d->m_x / point.d->m_x, d->m_y / point.d->m_y);
}

Point &Point::operator+=(const Point &point)
{
    d->m_x += point.d->m_x;
    d->m_y += point.d->m_y;
    return *this;
}

Point &Point::operator-=(const Point &point)
{
    d->m_x -= point.d->m_x;
    d->m_y -= point.d->m_y;
    return *this;
}

Point &Point::operator*=(const Point &point)
{
    d->m_x *= point.d->m_x;
    d->m_y *= point.d->m_y;
    return *this;
}

Point &Point::operator/=(const Point &point)
{
    d->m_x /= point.d->m_x;
    d->m_y /= point.d->m_y;
    return *this;
}

}
