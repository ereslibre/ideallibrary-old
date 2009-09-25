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

#include "ideal_string.h"

#include <stdlib.h>
#include <string.h>

namespace IdealCore {

class String::Private
{
public:
    Private()
        : m_str(0)
        , m_charMap(0)
        , m_size(0)
        , m_refs(1)
    {
    }

    ~Private()
    {
        delete[] m_str;
        delete[] m_charMap;
    }

    Private *copy()
    {
        const unsigned int rawLen = strlen(m_str);
        Private *privateCopy = new Private;
        privateCopy->m_str = new char[rawLen];
        memcpy(privateCopy->m_str, m_str, rawLen);
        privateCopy->m_charMap = new unsigned int[m_size];
        memcpy(privateCopy->m_charMap, m_charMap, m_size * sizeof(unsigned int));
        privateCopy->m_size = m_size;
        return privateCopy;
    }

    void calculateSize()
    {
        const unsigned int rawLen = strlen(m_str);
        m_charMap = new unsigned int[rawLen];
        bzero(m_charMap, rawLen);
        size_t i = 0;
        m_size = 0;
        while (true) {
            const char c = m_str[i];
            if (c == '\0') {
                break;
            }
            if (!(c & (1 << 7))) {
                m_charMap[m_size] = i;
                ++m_size;
            } else if ((c & (1 << 7)) && !(c & (1 << 6))) {
                IDEAL_DEBUG_WARNING("unexpected result when reading utf8");
                return;
            } else if (((c & (1 << 7)) && (c & (1 << 6)) && !(c & (1 << 5)))) {
                m_charMap[m_size] = i;
                ++i;
                ++m_size;
            } else if ((c & (1 << 7)) && (c & (1 << 6)) && (c & (1 << 5)) && !(c & (1 << 4))) {
                m_charMap[m_size] = i;
                i += 2;
                ++m_size;
            } else if ((c & (1 << 7)) && (c & (1 << 6)) && (c & (1 << 5)) && (c & (1 << 4))) {
                m_charMap[m_size] = i;
                i += 3;
                ++m_size;
            } else {
                IDEAL_DEBUG_WARNING("unexpected result when reading utf8");
                return;
            }
            ++i;
        }
        m_charMap = (unsigned int*) realloc(m_charMap, m_size);
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

    int refCount()
    {
        return m_refs;
    }

    char         *m_str;
    unsigned int *m_charMap;
    unsigned int  m_size;
    unsigned int  m_refs;
};

String::String()
    : d(new Private)
{
}

String::String(const String &str)
{
    str.d->ref();
    d = str.d;
}

String::String(const char *str)
    : d(new Private)
{
    if (str) {
        const unsigned int rawLen = strlen(str);
        d->m_str = new char[rawLen + 1];
        memcpy(d->m_str, str, rawLen);
        d->m_str[rawLen] = '\0';
        d->calculateSize();
    }
}

String::String(const char *str, size_t n)
    : d(new Private)
{
    if (str) {
        const unsigned int rawLen = strlen(str);
        d->m_str = new char[rawLen];
        bzero(d->m_str, rawLen);
        memcpy(d->m_str, str, rawLen);
#if 0
        d->m_str[rawLen] = '\0';
        d->calculateSize();
#endif
        if (n < rawLen) {
            // TODO
        }
    }
}

String::String(Char c)
    : d(new Private)
{
    const int octetsRequired = c.octetsRequired();
    d->m_str = new char[octetsRequired];
    bzero(d->m_str, octetsRequired);
    d->m_size = 1;
    unsigned int utf32Char = c.utf32Char();
    switch (octetsRequired) {
        case 1:
            d->m_str[0] = utf32Char & 0x0000007f;
            return;
        case 2:
            d->m_str[0] = (utf32Char & 0x0000001f) | 0x000000c0;
            utf32Char = utf32Char << 5;
            break;
        case 3:
            d->m_str[0] = (utf32Char & 0x0000000f) | 0x000000e0;
            utf32Char = utf32Char << 4;
            break;
        case 4:
            d->m_str[0] = (utf32Char & 0x00000007) | 0x000000f0;
            utf32Char = utf32Char << 3;
            break;
    }
    for (int i = 1; i < octetsRequired; ++i) {
        d->m_str[i] = (utf32Char & 0x0000003f) | 0x00000080;
        utf32Char = utf32Char << 6;
    }
}

String::~String()
{
    d->deref();
}

void String::clear()
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    delete[] d->m_str;
    d->m_str = 0;
    delete[] d->m_charMap;
    d->m_charMap = 0;
    d->m_size = 0;
}

bool String::empty() const
{
    return d->m_size == 0;
}

size_t String::size() const
{
    return d->m_size;
}

bool String::contains(Char c) const
{
    // TODO
    return false;
}

size_t String::find(Char c) const
{
    // TODO
    return false;
}

size_t String::rfind(Char c) const
{
    // TODO
    return false;
}

size_t String::find(const String &str) const
{
    // TODO
    return false;
}

const char *String::data() const
{
    return d->m_str;
}

String String::substr(size_t pos, size_t n) const
{
    // TODO
    return String();
}

int String::compare(const char *s) const
{
    return strcoll(d->m_str, s);
}

List<String> String::split(Char separator) const
{
    // TODO
    return List<String>();
}

Char String::operator[](unsigned int pos) const
{
    if (pos >= d->m_size) {
        IDEAL_DEBUG_WARNING("requested char is out of bounds");
        return Char();
    }
    int cpos = d->m_charMap[pos];
    const char requestedChar = d->m_str[cpos];
    if (!(requestedChar & 0x80)) {
        return requestedChar;
    }
    Char res;
    int octets;
    if (!(requestedChar & 0x20)) {
        octets = 2;
        res |= (requestedChar & 0x0000001f);
        res = res << 5;
    } else if (!(requestedChar & 0x10)) {
        octets = 3;
        res |= (requestedChar & 0x0000000f);
        res = res << 4;
    } else {
        octets = 4;
        res |= (requestedChar & 0x00000007);
        res = res << 3;
    }
    for (int i = 1; i < octets; ++i) {
        const char c = d->m_str[++cpos];
        res |= (c & 0x0000003f);
        res = res << 6;
    }
    return res;
}

String &String::operator=(const String &str)
{
    if (this == &str) {
        return *this;
    }
    d->deref();
    str.d->ref();
    d = str.d;
    return *this;
}

String &String::operator=(const char *str)
{
    if (!str) {
        return *this;
    }
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    d->calculateSize();
    return *this;
}

String &String::operator=(Char c)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    d->m_size = 1;
    return *this;
}

String &String::operator+=(const String &str)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    d->m_size += str.d->m_size;
    return *this;
}

String &String::operator+=(const char *str)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    d->calculateSize();
    return *this;
}

String &String::operator+=(Char c)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    ++d->m_size;
    return *this;
}

String String::operator+(const String &str) const
{
    String res;
    res.d->m_size = d->m_size + str.d->m_size;
    return res;
}

String String::operator+(const char *str) const
{
    String res;
    res.d->calculateSize();
    return res;
}

String String::operator+(Char c) const
{
    String res;
    res.d->m_size = d->m_size + 1;
    return res;
}

bool String::operator==(const String &str) const
{
    if (this == &str || d == str.d) {
        return true;
    }
    return false;
}

bool String::operator!=(const String &str) const
{
    return !(*this == str);
}

bool String::operator<(const String &str) const
{
    if (this == &str) {
        return false;
    }
    return false;
}

bool String::operator>(const String &str) const
{
    return !(*this < str) && !(*this == str);
}

bool String::operator<=(const String &str) const
{
    return !(*this > str);
}

bool String::operator>=(const String &str) const
{
    return !(*this < str);
}

}

std::ostream &operator<<(std::ostream &stream, const IdealCore::String &str)
{
    stream << str.data();
    return stream;
}
