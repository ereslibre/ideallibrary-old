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

#include "size.h"

namespace IdealGUI
{

class Size::Private
{
public:
    Private(double width, double height);

    double m_width;
    double m_height;
};

Size::Private::Private(double width, double height)
    : m_width(width)
    , m_height(height)
{
}

Size::Size(double width, double height)
    : d(new Private(width, height))
{
}

Size::Size(const Size &size)
    : d(new Private(size.d->m_width, size.d->m_height))
{
}

Size::~Size()
{
    delete d;
}

double Size::width() const
{
    return d->m_width;
}

void Size::setWidth(double width)
{
    d->m_width = width;
}

double Size::height() const
{
    return d->m_height;
}

void Size::setHeight(double height)
{
    d->m_height = height;
}

double &Size::rwidth() const
{
    return d->m_width;
}

double &Size::rheight() const
{
    return d->m_height;
}

bool Size::operator==(const Size &size)
{
    return d->m_width == size.d->m_width && d->m_height == size.d->m_height;
}

bool Size::operator!=(const Size &size)
{
    return d->m_width != size.d->m_width || d->m_height != size.d->m_height;
}

Size &Size::operator=(const Size &size)
{
    d->m_width = size.d->m_width;
    d->m_height = size.d->m_height;
    return *this;
}

Size Size::operator+(const Size &size)
{
    return Size(d->m_width + size.d->m_width, d->m_height + size.d->m_height);
}

Size Size::operator-(const Size &size)
{
    return Size(d->m_width - size.d->m_width, d->m_height - size.d->m_height);
}

Size Size::operator*(const Size &size)
{
    return Size(d->m_width * size.d->m_width, d->m_height * size.d->m_height);
}

Size Size::operator/(const Size &size)
{
    return Size(d->m_width / size.d->m_width, d->m_height / size.d->m_height);
}

Size &Size::operator+=(const Size &size)
{
    d->m_width += size.d->m_width;
    d->m_height += size.d->m_height;
    return *this;
}

Size &Size::operator-=(const Size &size)
{
    d->m_width -= size.d->m_width;
    d->m_height -= size.d->m_height;
    return *this;
}

Size &Size::operator*=(const Size &size)
{
    d->m_width *= size.d->m_width;
    d->m_height *= size.d->m_height;
    return *this;
}

Size &Size::operator/=(const Size &size)
{
    d->m_width /= size.d->m_width;
    d->m_height /= size.d->m_height;
    return *this;
}

}
