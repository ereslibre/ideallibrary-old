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
    Private(ireal width, ireal height);

    ireal m_width;
    ireal m_height;
};

Size::Private::Private(ireal width, ireal height)
    : m_width(width)
    , m_height(height)
{
}

Size::Size(ireal width, ireal height)
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

ireal Size::width() const
{
    return d->m_width;
}

void Size::setWidth(ireal width)
{
    d->m_width = width;
}

ireal Size::height() const
{
    return d->m_height;
}

void Size::setHeight(ireal height)
{
    d->m_height = height;
}

ireal &Size::rwidth() const
{
    return d->m_width;
}

ireal &Size::rheight() const
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
