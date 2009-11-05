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

#ifndef BYTE_STREAM_H
#define BYTE_STREAM_H

#include <ideal_export.h>
#include <core/ideal_string.h>

namespace IdealCore {

/**
  * @class ByteStream byte_stream.h core/byte_stream.h
  *
  * A stream of bytes.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT ByteStream
{
public:
    ByteStream();
    ByteStream(const ByteStream &byteStream);
    ByteStream(const char *data);
    ByteStream(const char *data, iuint64 nbytes);
    virtual ~ByteStream();

    /**
      * @return the size of the stream.
      */
    iuint64 size() const;

    /**
      * @return the contents of the stream.
      */
    const char *data() const;

    ByteStream &operator=(const char *data);
    ByteStream &operator=(const ByteStream &byteStream);

private:
    class Private;
    Private *d;
};

}

IDEAL_EXPORT std::ostream &operator<<(std::ostream &stream, const IdealCore::ByteStream &byteStream);

#endif //BYTE_STREAM_H
