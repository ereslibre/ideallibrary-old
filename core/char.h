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

#ifndef CHAR_H
#define CHAR_H

#include <ideal_export.h>

namespace IdealCore {

/**
  * @class Char
  *
  * Represents a 21 bit wide character.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Char
{
public:
    Char(char c);
    Char(unsigned short c);
    Char(unsigned int c);

    /**
      * @return The required number of octets to properly encode this character in UTF-8.
      *         The required number of octets can be 1, 2, 3 or 4.
      */
    int octetsRequired() const;

    /**
      * @return The 21 bit wide character.
      */
    unsigned int utf32Char() const;

    operator char();
    operator unsigned short();
    operator unsigned int();

    bool operator==(Char c) const;
    bool operator==(char c) const;
    bool operator==(unsigned short c) const;
    bool operator==(unsigned int c) const;
    
private:
    unsigned int c;
};

}

IDEAL_EXPORT std::ostream &operator<<(std::ostream &stream, IdealCore::Char c);

#endif //CHAR_H
