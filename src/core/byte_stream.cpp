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

#include "byte_stream.h"

#include <string.h>

namespace IdealCore {

class ByteStream::Private
{
public:
    Private();
    virtual ~Private();

    void newAndDetach(ByteStream *byteStream);
    void clearContents();

    void ref();
    void deref();

    static Private *empty();

    void init(const ichar *data, size_t nbytes = 0);

    ichar  *m_data;
    size_t  m_size;
    size_t  m_refs;

    static Private *m_privateEmpty;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

ByteStream::Private::Private()
    : m_data(0)
    , m_size(0)
    , m_refs(1)
{
}

ByteStream::Private::~Private()
{
    free(m_data);
}

void ByteStream::Private::newAndDetach(ByteStream *byteStream)
{
    if (m_refs > 1) {
        byteStream->d = new Private;
        deref();
    } else if (this == m_privateEmpty) {
        m_privateEmpty = 0;
    } else {
        clearContents();
    }
}

void ByteStream::Private::clearContents()
{
    free(m_data);
    m_data = 0;
    m_size = 0;
}

void ByteStream::Private::ref()
{
    ++m_refs;
}

void ByteStream::Private::deref()
{
    --m_refs;
    if (!m_refs) {
        if (this == m_privateEmpty) {
            m_privateEmpty = 0;
        }
        delete this;
    }
}

ByteStream::Private *ByteStream::Private::empty()
{
    if (!m_privateEmpty) {
        m_privateEmpty = new Private;
    } else {
        m_privateEmpty->ref();
    }
    return m_privateEmpty;
}

void ByteStream::Private::init(const ichar *data, size_t nbytes)
{
    if (data) {
        if (nbytes) {
            m_size = nbytes;
        } else {
            m_size = strlen(data);
        }
        m_data = (ichar*) calloc(m_size + 1, sizeof(ichar));
        memcpy(m_data, data, m_size);
    }
}

ByteStream::Private *ByteStream::Private::m_privateEmpty = 0;

////////////////////////////////////////////////////////////////////////////////////////////////////

ByteStream::ByteStream()
    : d(Private::empty())
{
}

ByteStream::ByteStream(const ByteStream &byteStream)
{
    byteStream.d->ref();
    d = byteStream.d;
}

ByteStream::ByteStream(const ichar *data)
{
    if (data) {
        d = new Private;
        d->init(data);
    } else {
        d = Private::empty();
    }
}

ByteStream::ByteStream(const ichar *data, size_t nbytes)
{
    if (data && nbytes) {
        d = new Private;
        d->init(data, nbytes);
    } else {
        d = Private::empty();
    }
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
    d->newAndDetach(this);
    d->init(data);
    return *this;
}

ByteStream &ByteStream::operator=(const ByteStream &byteStream)
{
    if (d == byteStream.d) {
        return *this;
    }
    d->deref();
    byteStream.d->ref();
    d = byteStream.d;
    return *this;
}

}

////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream &stream, const IdealCore::ByteStream &byteStream)
{
    stream << '\"' << byteStream.data() << '\"';
    return stream;
}
