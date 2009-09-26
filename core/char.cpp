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

unsigned int Char::value() const
{
    return c;
}

Char::operator char()
{
    return c;
}

Char::operator unsigned short()
{
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
    return false;
}

bool Char::operator==(unsigned short c) const
{
    return false;
}

bool Char::operator==(unsigned int c) const
{
    return c == this->c;
}

Char Char::operator&=(Char c)
{
    this->c &= c.c;
    return *this;
}

Char Char::operator&=(unsigned int c)
{
    this->c &= c;
    return *this;
}

Char Char::operator|=(Char c)
{
    this->c |= c.c;
    return *this;
}

Char Char::operator|=(unsigned int c)
{
    this->c |= c;
    return *this;
}

Char Char::operator<<(int b) const
{
    return c << b;
}

}

std::ostream &operator<<(std::ostream &stream, IdealCore::Char c)
{
    const unsigned int value = c.value();
    if (value & 0xf0000000) {
        stream << (char) ((value & 0xff000000) >> 24);
        stream << (char) ((value & 0xff0000) >> 16);
        stream << (char) ((value & 0xff00) >> 8);
        stream << (char) (value & 0xff);
    } else if (value & 0xf00000) {
        stream << (char) ((value & 0xff0000) >> 16);
        stream << (char) ((value & 0xff00) >> 8);
        stream << (char) (value & 0xff);
    } else if (value & 0xf000) {
        stream << (char) ((value & 0xff00) >> 8);
        stream << (char) (value & 0xff);
    } else {
        stream << (char) (value & 0xff);
    }
    return stream;
}
