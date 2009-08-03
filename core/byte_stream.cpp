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

#include "byte_stream.h"

namespace IdealCore {

class ByteStream::Private
{
public:
    Private(const char *data)
        : m_data(data)
    {
    }

    const char *m_data;
};

ByteStream::ByteStream(const char *data)
    : d(new Private(data))
{
}

ByteStream::~ByteStream()
{
    delete d;
}

const char *ByteStream::data() const
{
    return d->m_data;
}

}

std::ostream &operator<<(std::ostream &stream, const IdealCore::ByteStream &byteStream)
{
    stream << byteStream.data();
    return stream;
}
