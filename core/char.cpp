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

#include "char.h"

namespace IdealCore {

    Char::Char(char c)
        : c(c)
    {
    }

    Char::Char(unsigned short c)
        : c(c)
    {
    }

    Char::Char(unsigned int c)
        : c(c)
    {
    }

    Char::operator char()
    {
        if (c & 0xffffff00) {
            IDEAL_DEBUG_WARNING("the character will be corrupted on conversion");
        }
        return c;
    }

    Char::operator unsigned short()
    {
        if (c & 0xffff0000) {
            IDEAL_DEBUG_WARNING("the character will be corrupted on conversion");
        }
        return c;
    }

    Char::operator unsigned int()
    {
        return c;
    }

    bool Char::operator==(Char c) const
    {
        return c.c == this->c;
    }

    bool Char::operator==(char c) const
    {
        return (unsigned char) c == this->c;
    }

    bool Char::operator==(unsigned short c) const
    {
        return c == this->c;
    }

    bool Char::operator==(unsigned int c) const
    {
        return c == this->c;
    }

    std::ostream &Char::operator<<(std::ostream &stream)
    {
        stream << this->c;
        return stream;
    }

}
