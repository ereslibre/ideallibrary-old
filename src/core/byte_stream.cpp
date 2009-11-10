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

#include <string.h>

namespace IdealCore {

class ByteStream::Private
{
public:
    Private(const char *data, iuint64 nbytes = 0)
        : m_data(0)
        , m_size(0)
    {
        if (data) {
            if (nbytes) {
                m_size = nbytes;
            } else {
                m_size = strlen(data);
            }
            m_data = new char[m_size + 1];
            memcpy(m_data, data, m_size);
            m_data[m_size] = '\0';
        }
    }

    ~Private()
    {
        delete[] m_data;
    }

    char   *m_data;
    iuint64 m_size;
};

ByteStream::ByteStream()
    : d(new Private(0))
{
}

ByteStream::ByteStream(const ByteStream &byteStream)
    : d(new Private(byteStream.d->m_data, byteStream.d->m_size))
{
}

ByteStream::ByteStream(const char *data)
    : d(new Private(data))
{
}

ByteStream::ByteStream(const char *data, iuint64 nbytes)
    : d(new Private(data, nbytes))
{
}

ByteStream::~ByteStream()
{
    delete d;
}

iuint64 ByteStream::size() const
{
    return d->m_size;
}

const char *ByteStream::data() const
{
    return d->m_data;
}

ByteStream &ByteStream::operator=(const char *data)
{
    delete d;
    d = new Private(data);
    return *this;
}

ByteStream &ByteStream::operator=(const ByteStream &byteStream)
{
    delete d;
    d = new Private(byteStream.d->m_data, byteStream.d->m_size);
    return *this;
}

}

std::ostream &operator<<(std::ostream &stream, const IdealCore::ByteStream &byteStream)
{
    stream << '\"' << byteStream.data() << '\"';
    return stream;
}
