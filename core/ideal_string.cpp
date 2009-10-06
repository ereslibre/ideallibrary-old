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
#include <assert.h>

namespace IdealCore {

class String::Private
{
public:
    Private()
        : m_str(new char[1])
        , m_charMap(0)
        , m_size(0)
        , m_sizeCalculated(false)
        , m_refs(1)
    {
        *m_str = '\0';
    }

    ~Private()
    {
        delete[] m_str;
        delete[] m_charMap;
    }

    void init(const char *str)
    {
        const unsigned int rawLen = strlen(str);
        delete[] m_str;
        m_str = new char[rawLen + 1];
        memcpy(m_str, str, rawLen);
        m_str[rawLen] = '\0';
        m_sizeCalculated = false;
    }

    Private *copy()
    {
        const unsigned int rawLen = strlen(m_str);
        Private *privateCopy = new Private;
        privateCopy->m_str = new char[rawLen + 1];
        memcpy(privateCopy->m_str, m_str, rawLen);
        privateCopy->m_str[rawLen] = '\0';
        privateCopy->m_charMap = new unsigned int[calculateSize()];
        memcpy(privateCopy->m_charMap, m_charMap, m_size * sizeof(unsigned int));
        privateCopy->m_size = m_size;
        privateCopy->m_sizeCalculated = true;
        return privateCopy;
    }

    unsigned int calculateSize()
    {
        if (m_sizeCalculated) {
            return m_size;
        }
        m_sizeCalculated = true;
        delete[] m_charMap;
        if (!m_str) {
            m_charMap = 0;
            m_size = 0;
            return 0;
        }
        const unsigned int rawLen = strlen(m_str);
        m_charMap = new unsigned int[rawLen];
        bzero(m_charMap, rawLen * sizeof(unsigned int));
        size_t i = 0;
        m_size = 0;
        while (true) {
            const char c = m_str[i];
            if (c == '\0') {
                break;
            }
            if (!(c & 0x80)) {
                m_charMap[m_size] = i;
                ++m_size;
            } else if (!(c & 0x20)) {
                m_charMap[m_size] = i;
                ++i;
                ++m_size;
            } else if (!(c & 0x10)) {
                m_charMap[m_size] = i;
                i += 2;
                ++m_size;
            } else if (!(c & 0x8)) {
                m_charMap[m_size] = i;
                i += 3;
                ++m_size;
            }
            ++i;
        }
        m_charMap = (unsigned int*) realloc(m_charMap, m_size * sizeof(unsigned int));
        return m_size;
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

    Char getCharAt(unsigned int pos)
    {
        Char res;
        const unsigned int mappedPos = m_charMap[pos];
        const char c = m_str[mappedPos];
        unsigned int numberOfOctets;
        if (!(c & 0x80)) {
            numberOfOctets = 1;
        } else if (!(c & 0x20)) {
            numberOfOctets = 2;
        } else if (!(c & 0x10)) {
            numberOfOctets = 3;
        } else {
            numberOfOctets = 4;
        }
        for (unsigned int i = 0; i < numberOfOctets; ++i) {
            res.c |= (m_str[mappedPos + i] & 0xff) << 8 * (numberOfOctets - i - 1);
        }
        return res;
    }

    char         *m_str;
    unsigned int *m_charMap;
    unsigned int  m_size;
    bool          m_sizeCalculated;
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
        d->init(str);
    }
}

String::String(const char *str, size_t n)
    : d(new Private)
{
    if (str && n) {
        const size_t rawLength = strlen(str);
        d->m_str = new char[(n == npos ? rawLength : n) * 4 + 1];
        d->m_charMap = new unsigned int[(n == npos ? rawLength : n)];
        size_t count = 0;
        bool breakNext = false;
        size_t curr = 0;
        for (size_t i = 0; i < rawLength; ++i) {
            if ((str[i] & 0xc0) != 0x80) {
                if (breakNext) {
                    break;
                }
                d->m_charMap[count] = i;
                ++count;
                if (count == n) {
                    breakNext = true;
                }
            }
            d->m_str[i] = str[i];
            ++curr;
        }
        d->m_str[curr] = '\0';
        d->m_size = count;
        d->m_sizeCalculated = true;
        if (curr < (n == npos ? rawLength * 4 : n * 4)) {
            d->m_str = (char*) realloc(d->m_str, curr + 1);
        }
        if (count < (n == npos ? rawLength : n)) {
            d->m_charMap = (unsigned int*) realloc(d->m_charMap, count * sizeof(unsigned int));
        }
    }
}

String::String(Char c)
    : d(new Private)
{
    const int numberOfOctets = c.octetsRequired();
    d->m_str = new char[numberOfOctets + 1];
    const unsigned int value = c.value();
    for (int i = 0; i < numberOfOctets; ++i) {
        const int offset = 8 * (numberOfOctets - i - 1);
        d->m_str[i] = (value >> offset) & 0xff;
    }
    d->m_str[numberOfOctets] = '\0';
    d->m_charMap = new unsigned int[1];
    d->m_charMap[0] = 0;
    d->m_size = 1;
    d->m_sizeCalculated = true;
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
    d->m_sizeCalculated = true;
}

bool String::empty() const
{
    return d->calculateSize() == 0;
}

size_t String::size() const
{
    return d->calculateSize();
}

bool String::contains(Char c) const
{
    for (unsigned int i = 0; i < d->calculateSize(); ++i) {
        if (d->getCharAt(i) == c) {
            return true;
        }
    }
    return false;
}

size_t String::find(Char c) const
{
    for (unsigned int i = 0; i < d->calculateSize(); ++i) {
        if (d->getCharAt(i) == c) {
            return i;
        }
    }
    return npos;
}

size_t String::rfind(Char c) const
{
    for (unsigned int i = d->calculateSize() - 1; i > 0; --i) {
        if (d->getCharAt(i) == c) {
            return i;
        }
    }
    if (d->calculateSize() && d->getCharAt(0) == c) {
        return 0;
    }
    return npos;
}

size_t String::find(const String &str) const
{
    if (!d->calculateSize() || !str.d->calculateSize() || str.d->m_size > d->m_size) {
        return npos;
    }
    const unsigned int offset = str.d->m_size - 1;
    unsigned int i = offset;
    Char hint = str.d->getCharAt(offset);
    while (i < d->m_size) {
        if (d->getCharAt(i) == hint) {
            bool fullMatch = true;
            unsigned int k = i - 1;
            for (unsigned int j = offset - 1; j > 0; --j) {
                if (d->getCharAt(k) != str.d->getCharAt(j)) {
                    fullMatch = false;
                    break;
                }
                --k;
            }
            fullMatch = fullMatch && d->getCharAt(k) == str.d->getCharAt(0);
            if (fullMatch) {
                return k;
            }
        }
        ++i;
    }
    return npos;
}

const char *String::data() const
{
    return d->m_str;
}

String String::substr(size_t pos, size_t n) const
{
    if (pos < d->calculateSize()) {
        return String(&d->m_str[d->m_charMap[pos]], n);
    }
    return String();
}

int String::compare(const char *s) const
{
    return strcoll(d->m_str, s);
}

List<String> String::split(Char separator) const
{
    List<String> res;
    if (!d->calculateSize()) {
        return res;
    }
    const int length = strlen(d->m_str);
    char *curr = new char[length];
    unsigned int pos = 0;
    for (unsigned int i = 0; i < d->m_size; ++i) {
        const Char currChar = d->getCharAt(i);
        if (pos && currChar == separator) {
            curr[pos] = '\0';
            res.push_back(String(curr));
            pos = 0;
            bzero(curr, length);
        } else if (currChar != separator) {
            union {
                unsigned int value;
                char v[4];
            } fragmentedValue;
            fragmentedValue.value = currChar.value();
            const int octetsRequired = currChar.octetsRequired();
            for (int i = 0; i < octetsRequired; ++i) {
                curr[pos] = fragmentedValue.v[octetsRequired - i - 1];
                ++pos;
            }
        }
    }
    if (strlen(curr)) {
        curr[pos] = '\0';
        res.push_back(String(curr));
    }
    delete[] curr;
    return res;
}

Char String::operator[](unsigned int pos) const
{
    if (pos < d->calculateSize()) {
        return d->getCharAt(pos);
    }
    return Char();
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
    d->init(str);
    return *this;
}

String &String::operator=(Char c)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    const int numberOfOctets = c.octetsRequired();
    delete[] d->m_str;
    d->m_str = new char[numberOfOctets + 1];
    const unsigned int value = c.value();
    for (int i = 0; i < numberOfOctets; ++i) {
        const int offset = 8 * (numberOfOctets - i - 1);
        d->m_str[i] = (value >> offset) & 0xff;
    }
    d->m_str[numberOfOctets] = '\0';
    delete[] d->m_charMap;
    d->m_charMap = new unsigned int[1];
    d->m_charMap[0] = 0;
    d->m_size = 1;
    d->m_sizeCalculated = true;
    return *this;
}

String &String::operator+=(const String &str)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    const unsigned int oldRawLength = (d->m_str ? strlen(d->m_str) : 0);
    const unsigned int newRawLength = oldRawLength + (str.d->m_str ? strlen(str.d->m_str) : 0);
    d->m_str = (char*) realloc(d->m_str, newRawLength + 1);
    union FragmentedValue {
        unsigned int value;
        char v[4];
    };
    FragmentedValue fragmentedValue;
    unsigned int pos = oldRawLength;
    for (unsigned int i = 0; i < str.d->calculateSize(); ++i) {
        const Char currChar = str[i];
        fragmentedValue.value = currChar.value();
        const int octetsRequired = currChar.octetsRequired();
        for (int j = 0; j < octetsRequired; ++j) {
            d->m_str[pos] = fragmentedValue.v[octetsRequired - j - 1];
            ++pos;
        }
    }
    d->m_str[newRawLength] = '\0';
    d->m_sizeCalculated = false;
    return *this;
}

String &String::operator+=(const char *str)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    const unsigned int rawLength = strlen(str);
    const unsigned int oldRawLength = d->m_str ? strlen(d->m_str) : 0;
    const unsigned int newRawLength = oldRawLength + rawLength;
    d->m_str = (char*) realloc(d->m_str, newRawLength + 1);
    memcpy(&d->m_str[oldRawLength], str, rawLength);
    d->m_str[newRawLength] = '\0';
    d->m_sizeCalculated = false;
    return *this;
}

String &String::operator+=(Char c)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    const int numberOfOctets = c.octetsRequired();
    const unsigned int rawLength = d->m_str ? strlen(d->m_str) : 0;
    const unsigned int newRawLength = rawLength + numberOfOctets;
    const unsigned int value = c.value();
    d->m_str = (char*) realloc(d->m_str, newRawLength + 1);
    int pos = 0;
    for (unsigned int i = rawLength; i < newRawLength; ++i) {
        const int offset = 8 * (numberOfOctets - pos - 1);
        d->m_str[i] = (value >> offset) & 0xff;
        ++pos;
    }
    d->m_str[newRawLength] = '\0';
    d->m_sizeCalculated = false;
    return *this;
}

String String::operator+(const String &str) const
{
    String res(d->m_str);
    res += str;
    return res;
}

String String::operator+(const char *str) const
{
    String res(d->m_str);
    res += str;
    return res;
}

String String::operator+(Char c) const
{
    String res(d->m_str);
    res += c;
    return res;
}

bool String::operator==(const String &str) const
{
    if (this == &str || d == str.d) {
        return true;
    }
    if ((!d->m_str && str.d->m_str) || (d->m_str && !str.d->m_str)) {
        return false;
    }
    if (!d->m_str && !str.d->m_str) {
        return true;
    }
    return !strcoll(d->m_str, str.d->m_str);
}

bool String::operator!=(const String &str) const
{
    return !(*this == str);
}

bool String::operator<(const String &str) const
{
    if (this == &str || d == str.d) {
        return false;
    }
    if (!d->m_str || !str.d->m_str) {
        return false;
    }
    return strcoll(d->m_str, str.d->m_str) < 0;
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
