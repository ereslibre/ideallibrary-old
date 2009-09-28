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

Char::Char()
    : c(0)
{
}

Char::Char(unsigned int c)
{
    if (!(c & 0x1fff80)) {
        this->c = c & 0x7f;
    } else if (!(c & 0x1ff800)) {
        this->c = ((c & 0x7c0) << 2) | 0xc000;
        this->c |= (c & 0x3f) | 0x80;
    } else if (!(c & 0x1f0000)) {
        this->c = ((c & 0xf000) << 4) | 0xe00000;
        this->c |= ((c & 0xfc0) << 2) | 0x8000;
        this->c |= (c & 0x3f) | 0x80;
    } else {
        this->c = ((c & 0x1c0000) << 6) | 0xf0000000;
        this->c |= ((c & 0x3f000) << 4) | 0x800000;
        this->c |= ((c & 0xfc0) << 2) | 0x8000;
        this->c |= (c & 0x3f) | 0x80;
    }
}

unsigned int Char::value() const
{
    return c;
}

int Char::octetsRequired() const
{
    if (!(c & 0xffffff00)) {
        return 1;
    }
    if (!(c & 0xffff0000)) {
        return 2;
    }
    if (!(c & 0xff000000)) {
        return 3;
    }
    return 4;
}

Char::operator char() const
{
    if (c & 0xffffff00) {
        IDEAL_DEBUG_WARNING("char '" << *this << "' will be corrupted on conversion");
    }
    return c;
}

Char::operator unsigned short() const
{
    if (c & 0xff000000) {
        IDEAL_DEBUG_WARNING("char '" << *this << "' will be corrupted on conversion");
    }
    unsigned short res = 0;
    if (!(c & 0xffffff00)) {
        res = c;
    } else if (!(c & 0xffff0000)) {
        res = (c & 0x1f00) >> 2;
        res |= (c & 0x3f);
    } else {
        res = (c & 0xf0000) >> 4;
        res |= (c & 0x3f00) >> 2;
        res |= (c & 0x3f);
    }
    return res;        
}

Char::operator unsigned int() const
{
    unsigned int res = 0;
    if (!(c & 0xffffff00)) {
        res = c;
    } else if (!(c & 0xffff0000)) {
        res = (c & 0x1f00) >> 2;
        res |= (c & 0x3f);
    } else if (!(c & 0xff000000)) {
        res = (c & 0xf0000) >> 4;
        res |= (c & 0x3f00) >> 2;
        res |= (c & 0x3f);
    } else {
        res = (c & 0x7000000) >> 6;
        res |= (c & 0x3f0000) >> 4;
        res |= (c & 0x3f00) >> 2;
        res |= (c & 0x3f);
    }
    return res;    
}

bool Char::operator==(Char c) const
{
    return this->c == c.c;
}

bool Char::operator==(char c) const
{
    if (this->c & 0xffffff00) {
        return false;
    }
    return this->c == (unsigned char) c;
}

bool Char::operator==(unsigned short c) const
{
    if (this->c & 0xff000000) {
        return false;
    }
    switch (octetsRequired()) {
        case 3:
            if (c & 0xffff0000) {
                return false;
            }
            if ((c & 0x3f) != (this->c & 0x3f)) {
                return false;
            }
            if ((c & 0xfc0) != ((this->c & 0x3f00) >> 2)) {
                return false;
            }
            if ((c & 0xf000) != ((this->c & 0xf0000) >> 4)) {
                return false;
            }
            break;
        case 2:
            if (c & 0xfffff800) {
                return false;
            }
            if ((c & 0x3f) != (this->c & 0x3f)) {
                return false;
            }
            if ((c & 0x7c0) != ((this->c & 0x1f00) >> 2)) {
                return false;
            }
            break;
        default:
            if (c & 0xffffff00) {
                return false;
            }
            return this->c == c;
    }
    return true;
}

bool Char::operator==(unsigned int c) const
{
    switch (octetsRequired()) {
        case 4:
            if ((c & 0x3f) != (this->c & 0x3f)) {
                return false;
            }
            if ((c & 0xfc0) != ((this->c & 0x3f00) >> 2)) {
                return false;
            }
            if ((c & 0x3f000) != ((this->c & 0x3f0000) >> 4)) {
                return false;
            }
            if ((c & 0x1c0000) != ((this->c & 0x7000000) >> 6)) {
                return false;
            }
            break;
        case 3:
            if (c & 0xffff0000) {
                return false;
            }
            if ((c & 0x3f) != (this->c & 0x3f)) {
                return false;
            }
            if ((c & 0xfc0) != ((this->c & 0x3f00) >> 2)) {
                return false;
            }
            if ((c & 0xf000) != ((this->c & 0xf0000) >> 4)) {
                return false;
            }
            break;
        case 2:
            if (c & 0xfffff800) {
                return false;
            }
            if ((c & 0x3f) != (this->c & 0x3f)) {
                return false;
            }
            if ((c & 0x7c0) != ((this->c & 0x1f00) >> 2)) {
                return false;
            }
            break;
        default:
            if (c & 0xffffff00) {
                return false;
            }
            return this->c == c;
    }
    return true;
}

}

std::ostream &operator<<(std::ostream &stream, IdealCore::Char c)
{
    const unsigned int value = c.value();
    switch (c.octetsRequired()) {
        case 4:
            stream << (char) ((value & 0xff000000) >> 24);
            stream << (char) ((value & 0xff0000) >> 16);
            stream << (char) ((value & 0xff00) >> 8);
            stream << (char) (value & 0xff);
            break;
        case 3:
            stream << (char) ((value & 0xff0000) >> 16);
            stream << (char) ((value & 0xff00) >> 8);
            stream << (char) (value & 0xff);
            break;
        case 2:
            stream << (char) ((value & 0xff00) >> 8);
            stream << (char) (value & 0xff);
            break;
        default:
            stream << (char) (value & 0xff);
            break;
    }
    return stream;
}
