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
    Private(const ichar *data, size_t nbytes = 0)
        : m_data(0)
        , m_size(0)
        , m_refs(1)
    {
        if (data) {
            if (nbytes) {
                m_size = nbytes;
            } else {
                m_size = strlen(data);
            }
            m_data = new ichar[m_size + 1];
            memcpy(m_data, data, m_size);
            m_data[m_size] = '\0';
        }
    }

    ~Private()
    {
        delete[] m_data;
    }

    void ref()
    {
        ++m_refs;
    }

    void deref()
    {
        --m_refs;
        if (!m_refs) {
            delete this;
        }
    }

    size_t refCount()
    {
        return m_refs;
    }

    ichar  *m_data;
    size_t  m_size;
    size_t  m_refs;
};

ByteStream::ByteStream()
    : d(new Private(0))
{
}

ByteStream::ByteStream(const ByteStream &byteStream)
{
    byteStream.d->ref();
    d = byteStream.d;
}

ByteStream::ByteStream(const ichar *data)
    : d(new Private(data))
{
}

ByteStream::ByteStream(const ichar *data, size_t nbytes)
    : d(new Private(data, nbytes))
{
}

ByteStream::~ByteStream()
{
    d->deref();
}

size_t ByteStream::size() const
{
    return d->m_size;
}

const ichar *ByteStream::data() const
{
    return d->m_data;
}

ByteStream &ByteStream::operator=(const ichar *data)
{
    d->deref();
    d = new Private(data);
    return *this;
}

ByteStream &ByteStream::operator=(const ByteStream &byteStream)
{
    if (this == &byteStream || d == byteStream.d) {
        return *this;
    }
    d->deref();
    byteStream.d->ref();
    d = byteStream.d;
    return *this;
}

}

std::ostream &operator<<(std::ostream &stream, const IdealCore::ByteStream &byteStream)
{
    stream << '\"' << byteStream.data() << '\"';
    return stream;
}
