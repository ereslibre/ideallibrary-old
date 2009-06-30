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

#include "rect.h"

namespace IdealGUI {

class Rect::Private
{
public:
    Private(const Point &topLeft, double width, double height);

    Point  m_topLeft;
    double m_width;
    double m_height;
};

Rect::Private::Private(const Point &topLeft, double width, double height)
    : m_topLeft(topLeft)
    , m_width(width)
    , m_height(height)
{
}

Rect::Rect(const Point &topLeft, double width, double height)
    : d(new Private(topLeft, width, height))
{
}

Rect::Rect(const Rect &rect)
    : d(new Private(rect.d->m_topLeft, rect.d->m_width, rect.d->m_height))
{
}

Rect::~Rect()
{
    delete d;
}

Point Rect::topLeft() const
{
    return d->m_topLeft;
}

void Rect::setTopLeft(const Point &topLeft)
{
    d->m_topLeft = topLeft;
}

double Rect::width() const
{
    return d->m_width;
}

void Rect::setWidth(double width)
{
    d->m_width = width;
}

double Rect::height() const
{
    return d->m_height;
}

void Rect::setHeight(double height)
{
    d->m_height = height;
}

Point Rect::topRight() const
{
    return Point(d->m_topLeft.x() + d->m_width, d->m_topLeft.y());
}

Point Rect::bottomLeft() const
{
    return Point(d->m_topLeft.x(), d->m_topLeft.y() + d->m_height);
}

Point Rect::bottomRight() const
{
    return Point(d->m_topLeft.x() + d->m_width, d->m_topLeft.y() + d->m_height);
}

bool Rect::operator==(const Rect &rect)
{
    return d->m_topLeft == rect.d->m_topLeft && d->m_width == rect.d->m_width &&
           d->m_height == rect.d->m_height;
}

bool Rect::operator!=(const Rect &rect)
{
    return d->m_topLeft != rect.d->m_topLeft || d->m_width != rect.d->m_width ||
           d->m_height != rect.d->m_height;
}

Rect &Rect::operator=(const Rect &rect)
{
    d->m_topLeft = rect.d->m_topLeft;
    d->m_width = rect.d->m_width;
    d->m_height = rect.d->m_height;
    return *this;
}

}
