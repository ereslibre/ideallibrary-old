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

#ifndef CHAR_H
#define CHAR_H

#include <ideal_export.h>

namespace IdealCore {

/**
  * @class Char char.h core/char.h
  *
  * Represents a 32 bit wide character in UTF-8 encoding.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Char
{
    friend class String;
    friend class Uri;

public:
    Char();
    Char(const Char &c);
    Char(iuint32 c);
    Char(const ichar *c);

    /**
      * @return The character encoded in UTF-8.
      */
    iuint32 value() const;

    /**
      * @return The required number of octets to properly encode this character in UTF-8.
      *         The required number of octets can be 1, 2, 3 or 4.
      */
    iint32 octetsRequired() const;

    /**
      * @return This character mapped to a char type if the conversion can be done. Otherwise, 0
      *         is returned.
      */
    operator ichar() const;

    /**
      * @return This character mapped to a iuint8 type if the conversion can be done. Otherwise, 0
      *         is returned.
      */
    operator iuint8() const;

    /**
      * @return This character mapped to an iuint16 type if the conversion can be done.
      *         Otherwise, 0 is returned.
      */
    operator iuint16() const;

    /**
      * @return This character mapped to an iuint32 type if the conversion can be done.
      *         Otherwise, 0 is returned.
      */
    operator iuint32() const;

    Char &operator=(const Char &c);
    Char &operator=(ichar c);
    Char &operator=(iuint8 c);
    Char &operator=(iuint16 c);
    Char &operator=(iuint32 c);

    bool operator==(const Char &c) const;
    bool operator==(ichar c) const;
    bool operator==(iuint8 c) const;
    bool operator==(iuint16 c) const;
    bool operator==(iuint32 c) const;
    bool operator!=(const Char &c) const;
    bool operator!=(ichar c) const;
    bool operator!=(iuint8 c) const;
    bool operator!=(iuint16 c) const;
    bool operator!=(iuint32 c) const;

private:
    iuint32 c;
};

}

////////////////////////////////////////////////////////////////////////////////////////////////////

IDEAL_EXPORT std::ostream &operator<<(std::ostream &stream, IdealCore::Char c);

#endif //CHAR_H
