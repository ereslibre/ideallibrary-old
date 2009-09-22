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
        : m_size(0)
        , m_refs(1)
    {
    }

    ~Private()
    {
    }

    Private *copy()
    {
        Private *privateCopy = new Private;
        privateCopy->m_str = m_str;
        privateCopy->m_size = m_size;
        return privateCopy;
    }

    void calculateSize()
    {
        size_t i = 0;
        while (true) {
            const char c = m_str[i];
            if (c == '\0') {
                break;
            }
            if (!(c & (1 << 7))) {
                ++m_size;
            } else if ((c & (1 << 7)) && !(c & (1 << 6))) {
                IDEAL_DEBUG_WARNING("unexpected result when reading utf8");
                return;
            } else if (((c & (1 << 7)) && (c & (1 << 6)) && !(c & (1 << 5)))) {
                ++i;
                ++m_size;
            } else if ((c & (1 << 7)) && (c & (1 << 6)) && (c & (1 << 5)) && !(c & (1 << 4))) {
                i += 2;
                ++m_size;
            } else if ((c & (1 << 7)) && (c & (1 << 6)) && (c & (1 << 5)) && (c & (1 << 4))) {
                i += 3;
                ++m_size;
            } else {
                IDEAL_DEBUG_WARNING("unexpected result when reading utf8");
                return;
            }
            ++i;
        }
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

    std::string m_str;
    int         m_size;
    int         m_refs;
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

String::String(const std::string &str)
    : d(new Private)
{
    d->m_str = str;
    d->calculateSize();
}

String::String(const char *str)
    : d(new Private)
{
    if (str) {
        d->m_str = str;
        d->calculateSize();
    }
}

String::String(const char *str, size_t n)
    : d(new Private)
{
    if (str) {
        d->m_str = std::string(str, n);
        d->calculateSize();
    }
}

String::String(Char c)
    : d(new Private)
{
    d->m_str = c;
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
    d->m_str.clear();
}

bool String::empty() const
{
    return d->m_str.empty();
}

size_t String::size() const
{
    return d->m_size;
}

bool String::contains(Char c) const
{
    return d->m_str.find(c) != std::string::npos;
}

size_t String::find(Char c) const
{
    return d->m_str.find(c);
}

size_t String::rfind(Char c) const
{
    return d->m_str.rfind(c);
}

size_t String::find(const String &str) const
{
    return d->m_str.find(str.d->m_str);
}

const char *String::data() const
{
    return d->m_str.data();
}

String String::substr(size_t pos, size_t n) const
{
    String res(d->m_str.substr(pos, n));
    return res;
}

int String::compare(const char *s) const
{
    return strcoll(d->m_str.data(), s);
}

List<String> String::split(Char separator) const
{
    List<String> res;
    size_t pos = 0;
    size_t oldPos = 0;
    while ((pos = d->m_str.find(separator, pos)) != String::npos) {
        if (pos > 0 && pos <= d->m_str.size() - 1) {
            res.push_back(d->m_str.substr(oldPos, pos - oldPos));
        }
        ++pos;
        oldPos = pos;
    }
    if (oldPos <= d->m_str.size() - 1) {
        res.push_back(d->m_str.substr(oldPos));
    }
    return res;
}

Char String::operator[](int pos) const
{
    return d->m_str[pos];
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
    d->m_str = str;
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
    d->m_str = c;
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
    d->m_str += str.d->m_str;
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
    d->m_str += str;
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
    d->m_str += c;
    ++d->m_size;
    return *this;
}

String String::operator+(const String &str) const
{
    String res;
    res.d->m_str = d->m_str + str.d->m_str;
    res.d->m_size = d->m_size + str.d->m_size;
    return res;
}

String String::operator+(const char *str) const
{
    String res;
    res.d->m_str = d->m_str + str;
    res.d->calculateSize();
    return res;
}

String String::operator+(Char c) const
{
    String res;
    res.d->m_str = d->m_str + (char) c; // FIXME
    res.d->m_size = d->m_size + 1;
    return res;
}

bool String::operator==(const String &str) const
{
    if (this == &str || d == str.d) {
        return true;
    }
    return !strcoll(d->m_str.data(), str.d->m_str.data());
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
    return strcoll(d->m_str.data(), str.d->m_str.data()) < 0;
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
