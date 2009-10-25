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
    Private(const Point &topLeft, const Size &size);

    Point m_topLeft;
    Size  m_size;
};

Rect::Private::Private(const Point &topLeft, const Size &size)
    : m_topLeft(topLeft)
    , m_size(size)
{
}

Rect::Rect(const Point &topLeft, const Size &size)
    : d(new Private(topLeft, size))
{
}

Rect::Rect(const Rect &rect)
    : d(new Private(rect.d->m_topLeft, rect.d->m_size))
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

Size Rect::size() const
{
    return d->m_size;
}

void Rect::setSize(const Size &size)
{
    d->m_size = size;
}

Point Rect::topRight() const
{
    return Point(d->m_topLeft.x() + d->m_size.width(), d->m_topLeft.y());
}

Point Rect::bottomLeft() const
{
    return Point(d->m_topLeft.x(), d->m_topLeft.y() + d->m_size.height());
}

Point Rect::bottomRight() const
{
    return Point(d->m_topLeft.x() + d->m_size.width(), d->m_topLeft.y() + d->m_size.height());
}

bool Rect::operator==(const Rect &rect)
{
    return d->m_topLeft == rect.d->m_topLeft && d->m_size == rect.d->m_size;
}

bool Rect::operator!=(const Rect &rect)
{
    return d->m_topLeft != rect.d->m_topLeft || d->m_size != rect.d->m_size;
}

Rect &Rect::operator=(const Rect &rect)
{
    d->m_topLeft = rect.d->m_topLeft;
    d->m_size = rect.d->m_size;
    return *this;
}

}
