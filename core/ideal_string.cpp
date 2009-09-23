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
        d->m_str = new char[rawLen];
        memcpy(d->m_str, str, rawLen);
        d->calculateSize();
    }
}

String::String(const char *str, size_t n)
    : d(new Private)
{
    if (str) {
        const unsigned int rawLen = strlen(str);
        d->m_str = new char[rawLen];
        memcpy(d->m_str, str, rawLen);
        d->calculateSize();
        if (n < rawLen) {
            // TODO
        }
    }
}

String::String(Char c)
    : d(new Private)
{
    switch (c.octetsRequired()) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        default:
            break;
    }
    d->m_size = 1;
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
}

bool String::empty() const
{
    return false;
}

size_t String::size() const
{
    return d->m_size;
}

bool String::contains(Char c) const
{
    return false;
}

size_t String::find(Char c) const
{
    return false;
}

size_t String::rfind(Char c) const
{
    return false;
}

size_t String::find(const String &str) const
{
    return false;
}

const char *String::data() const
{
    return d->m_str;
}

String String::substr(size_t pos, size_t n) const
{
    return String();
}

int String::compare(const char *s) const
{
    return 0;
}

List<String> String::split(Char separator) const
{
    return List<String>();
}

Char String::operator[](int pos) const
{
    return 'a';
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
