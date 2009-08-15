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
    ByteStream(const char *data);
    virtual ~ByteStream();

    int size() const;

    const char *data() const;

private:
    class Private;
    Private *const d;
};

}

IDEAL_EXPORT std::ostream &operator<<(std::ostream &stream, const IdealCore::ByteStream &byteStream);

#endif //BYTE_STREAM_H