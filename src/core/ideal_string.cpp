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
        , m_sizeCalculated(true)
        , m_refs(1)
    {
    }

    virtual ~Private()
    {
        free(m_str);
        free(m_charMap);
    }

    void init(const ichar *str)
    {
        const size_t rawLen = strlen(str);
        m_str = (ichar*) malloc((rawLen + 1) * sizeof(ichar));
        memcpy(m_str, str, rawLen);
        m_str[rawLen] = '\0';
        m_sizeCalculated = false;
    }

    Private *copy()
    {
        const size_t rawLen = strlen(m_str);
        Private *privateCopy = new Private;
        privateCopy->m_str = (ichar*) malloc((rawLen + 1) * sizeof(ichar));
        memcpy(privateCopy->m_str, m_str, rawLen);
        privateCopy->m_str[rawLen] = '\0';
        privateCopy->m_charMap = (size_t*) malloc(calculateSize() * sizeof(size_t));
        memcpy(privateCopy->m_charMap, m_charMap, m_size * sizeof(size_t));
        privateCopy->m_size = m_size;
        privateCopy->m_sizeCalculated = true;
        return privateCopy;
    }

    void copyAndDetach(String *str)
    {
        if (m_refs > 1) {
            str->d = copy();
            deref();
        } else if (this == m_privateEmpty) {
            m_privateEmpty = 0;
        }
    }

    void newAndDetach(String *str)
    {
        if (m_refs > 1) {
            str->d = new Private;
            deref();
        } else if (this == m_privateEmpty) {
            m_privateEmpty = 0;
        } else {
            free(m_str);
        }
    }

    size_t calculateSize()
    {
        if (m_sizeCalculated) {
            return m_size;
        }
        m_sizeCalculated = true;
        free(m_charMap);
        const size_t rawLen = strlen(m_str);
        m_charMap = (size_t*) calloc(rawLen, sizeof(size_t));
        size_t i = 0;
        m_size = 0;
        while (true) {
            const ichar c = m_str[i];
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
        if (m_size < rawLen) {
            m_charMap = (size_t*) realloc(m_charMap, m_size * sizeof(size_t));
        }
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
            if (this == m_privateEmpty) {
                m_privateEmpty = 0;
            }
            delete this;
        }
    }

    Char getCharAt(size_t pos) const
    {
        Char res;
        const size_t mappedPos = m_charMap[pos];
        const ichar c = m_str[mappedPos];
        iuint32 numberOfOctets;
        if (!(c & 0x80)) {
            numberOfOctets = 1;
        } else if (!(c & 0x20)) {
            numberOfOctets = 2;
        } else if (!(c & 0x10)) {
            numberOfOctets = 3;
        } else {
            numberOfOctets = 4;
        }
        for (iuint32 i = 0; i < numberOfOctets; ++i) {
            res.c |= (m_str[mappedPos + i] & 0xff) << 8 * (numberOfOctets - i - 1);
        }
        return res;
    }

    void iint64toa(iint64 number, iuint32 base)
    {
        iuint64toa(number < 0 ? -number : number, base, number < 0);
    }

    void iuint64toa(iuint64 number, iuint32 base, bool negative = false)
    {
        ichar *const str = (ichar*) calloc(67, sizeof(ichar));
        ichar *p = str + 65;
        while (number) {
            const iint32 n = number % base;
            if (n < 10) {
                *p = '0' + n;
            } else {
                *p = 'a' + (n - 10);
            }
            --p;
            number /= base;
        }
        if (negative) {
            *p = '-';
        } else {
            ++p;
        }
        init(p);
        free(str);
    }

    void dtoa(double number, iuint8 format, iuint32 precision)
    {
        ichar *const str = (ichar*) calloc(10, sizeof(ichar));
        sprintf(str, "%%.%dl%c", precision, format);
        ichar *const res = (ichar*) calloc(65, sizeof(ichar));
        sprintf(res, str, number);
        init(res);
        free(str);
        free(res);
    }

    static Private *empty();

    ichar  *m_str;
    size_t *m_charMap;
    size_t  m_size;
    bool    m_sizeCalculated;
    size_t  m_refs;

    class PrivateEmpty;
    static Private *m_privateEmpty;
};

String::Private *String::Private::m_privateEmpty = 0;

class String::Private::PrivateEmpty
    : public Private
{
public:
    PrivateEmpty()
    {
        m_str = (ichar*) malloc(sizeof(ichar));
        *m_str = '\0';
    }

    virtual ~PrivateEmpty()
    {
    }
};

String::Private *String::Private::empty()
{
    if (!m_privateEmpty) {
        m_privateEmpty = new PrivateEmpty;
    } else {
        m_privateEmpty->ref();
    }
    return m_privateEmpty;
}

String::String()
    : d(Private::empty())
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
    d->init(str.data());
}

String::String(const ichar *str)
{
    if (str) {
        d = new Private;
        d->init(str);
    } else {
        d = Private::empty();
    }
}

String::String(const ichar *str, size_t n)
{
    if (str && n) {
        d = new Private;
        const size_t rawLength = strlen(str);
        d->m_str = (ichar*) malloc(((n == npos ? rawLength : n) * 4 + 1) * sizeof(ichar));
        d->m_charMap = (size_t*) malloc((n == npos ? rawLength : n) * sizeof(size_t));
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
            d->m_str = (ichar*) realloc(d->m_str, curr + 1);
        }
        if (count < (n == npos ? rawLength : n)) {
            d->m_charMap = (size_t*) realloc(d->m_charMap, count * sizeof(size_t));
        }
    } else {
        d = Private::empty();
    }
}

String::String(Char c)
    : d(new Private)
{
    const iint32 numberOfOctets = c.octetsRequired();
    d->m_str = (ichar*) malloc((numberOfOctets + 1) * sizeof(ichar));
    const iuint32 value = c.value();
    for (iint32 i = 0; i < numberOfOctets; ++i) {
        const iint32 offset = 8 * (numberOfOctets - i - 1);
        d->m_str[i] = (value >> offset) & 0xff;
    }
    d->m_str[numberOfOctets] = '\0';
    d->m_charMap = (size_t*) malloc(sizeof(size_t));
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
    if (d == Private::m_privateEmpty) {
        return;
    }
    d->deref();
    d = Private::empty();
}

bool String::empty() const
{
    return d == Private::m_privateEmpty || d->calculateSize() == 0;
}

size_t String::size() const
{
    return d->calculateSize();
}

bool String::contains(Char c) const
{
    for (size_t i = 0; i < d->calculateSize(); ++i) {
        if (d->getCharAt(i) == c) {
            return true;
        }
    }
    return false;
}

size_t String::find(Char c) const
{
    for (size_t i = 0; i < d->calculateSize(); ++i) {
        if (d->getCharAt(i) == c) {
            return i;
        }
    }
    return npos;
}

size_t String::rfind(Char c) const
{
    for (size_t i = d->calculateSize() - 1; i > 0; --i) {
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
    const size_t offset = str.d->m_size - 1;
    size_t i = offset;
    Char hint = str.d->getCharAt(offset);
    while (i < d->m_size) {
        if (d->getCharAt(i) == hint) {
            bool fullMatch = true;
            size_t k = i - 1;
            for (size_t j = offset - 1; j > 0; --j) {
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

const ichar *String::data() const
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

iint32 String::compare(const ichar *s) const
{
    return strcoll(d->m_str, s);
}

List<String> String::split(Char separator) const
{
    List<String> res;
    if (!d->calculateSize()) {
        return res;
    }
    const iint32 length = strlen(d->m_str);
    ichar *curr = (ichar*) calloc(length + 1, sizeof(ichar));
    size_t pos = 0;
    for (size_t i = 0; i < d->m_size; ++i) {
        const Char currChar = d->getCharAt(i);
        if (pos && currChar == separator) {
            curr[pos] = '\0';
            res.push_back(String(curr));
            pos = 0;
            bzero(curr, (length + 1) * sizeof(ichar));
        } else if (currChar != separator) {
            union {
                iuint32 value;
                ichar v[4];
            } fragmentedValue;
            fragmentedValue.value = currChar.value();
            const iint32 octetsRequired = currChar.octetsRequired();
            for (iint32 i = 0; i < octetsRequired; ++i) {
                curr[pos] = fragmentedValue.v[octetsRequired - i - 1];
                ++pos;
            }
        }
    }
    if (strlen(curr)) {
        curr[pos] = '\0';
        res.push_back(String(curr));
    }
    free(curr);
    return res;
}

String &String::prepend(const String &str)
{
    d->copyAndDetach(this);
    const size_t rawLength = strlen(str.d->m_str);
    const size_t currRawLength = strlen(d->m_str);
    d->m_str = (ichar*) realloc(d->m_str, (rawLength + currRawLength + 1) * sizeof(ichar));
    if (currRawLength) {
        memmove(&d->m_str[rawLength], d->m_str, (currRawLength + 1) * sizeof(ichar));
    }
    memcpy(d->m_str, str.d->m_str, rawLength * sizeof(ichar));
    d->m_sizeCalculated = false;
    return *this;
}

String &String::prepend(const ichar *str)
{
    d->copyAndDetach(this);
    const size_t rawLength = strlen(str);
    const size_t currRawLength = strlen(d->m_str);
    d->m_str = (ichar*) realloc(d->m_str, (rawLength + currRawLength + 1) * sizeof(ichar));
    if (currRawLength) {
        memmove(&d->m_str[rawLength], d->m_str, (currRawLength + 1) * sizeof(ichar));
    }
    memcpy(d->m_str, str, rawLength * sizeof(ichar));
    d->m_sizeCalculated = false;
    return *this;
}

String &String::prepend(Char c)
{
    d->copyAndDetach(this);
    const size_t currRawLength = strlen(d->m_str);
    const iint32 numberOfOctets = c.octetsRequired();
    d->m_str = (ichar*) realloc(d->m_str, (numberOfOctets + currRawLength + 1) * sizeof(ichar));
    if (currRawLength) {
        memmove(&d->m_str[numberOfOctets], d->m_str, (currRawLength + 1) * sizeof(ichar));
    }
    const iuint32 value = c.value();
    for (iint32 i = 0; i < numberOfOctets; ++i) {
        const iint32 offset = 8 * (numberOfOctets - i - 1);
        d->m_str[i] = (value >> offset) & 0xff;
    }
    d->m_sizeCalculated = false;
    return *this;
}

String &String::append(const String &str)
{
    this->operator+=(str);
    return *this;
}

String &String::append(const ichar *str)
{
    this->operator+=(str);
    return *this;
}

String &String::append(Char c)
{
    this->operator+=(c);
    return *this;
}

iint8 String::toChar(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtol(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iuint8 String::toUChar(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtoul(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iint16 String::toShort(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtol(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iuint16 String::toUShort(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtoul(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iint32 String::toInt(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtol(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iuint32 String::toUInt(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtoul(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

long String::toLong(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtol(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iulong String::toULong(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const ilong res = strtoul(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iint64 String::toLongLong(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const iint64 res = strtoll(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

iuint64 String::toULongLong(bool *ok, iuint32 base) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const iuint64 res = strtoull(d->m_str, &p, base);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

float String::toFloat(bool *ok) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const float res = strtof(d->m_str, &p);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

ireal String::toDouble(bool *ok) const
{
    if (!d->calculateSize()) {
        if (ok) {
            *ok = false;
        }
        return 0;
    }
    ichar *p = 0;
    const double res = strtod(d->m_str, &p);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

String &String::setNumber(iint32 n, iuint32 base)
{
    d->newAndDetach(this);
    d->iint64toa(n, base);
    return *this;
}

String &String::setNumber(iuint32 n, iuint32 base)
{
    d->newAndDetach(this);
    d->iuint64toa(n, base);
    return *this;
}

String &String::setNumber(long n, iuint32 base)
{
    d->newAndDetach(this);
    d->iint64toa(n, base);
    return *this;
}

String &String::setNumber(iulong n, iuint32 base)
{
    d->newAndDetach(this);
    d->iuint64toa(n, base);
    return *this;
}

String &String::setNumber(iint64 n, iuint32 base)
{
    d->newAndDetach(this);
    d->iint64toa(n, base);
    return *this;
}

String &String::setNumber(iuint64 n, iuint32 base)
{
    d->newAndDetach(this);
    d->iuint64toa(n, base);
    return *this;
}

String &String::setNumber(float n, iuint8 format, iuint32 precision)
{
    d->newAndDetach(this);
    d->dtoa(n, format, precision);
    return *this;
}

String &String::setNumber(double n, iuint8 format, iuint32 precision)
{
    d->newAndDetach(this);
    d->dtoa(n, format, precision);
    return *this;
}

String String::number(iint32 n, iuint32 base)
{
    String str;
    str.setNumber(n, base);
    return str;
}

String String::number(iuint32 n, iuint32 base)
{
    String str;
    return str.setNumber(n, base);
}

String String::number(long n, iuint32 base)
{
    String str;
    return str.setNumber(n, base);
}

String String::number(iulong n, iuint32 base)
{
    String str;
    return str.setNumber(n, base);
}

String String::number(iint64 n, iuint32 base)
{
    String str;
    return str.setNumber(n, base);
}

String String::number(iuint64 n, iuint32 base)
{
    String str;
    return str.setNumber(n, base);
}

String String::number(float n, iuint8 format, iuint32 precision)
{
    String str;
    return str.setNumber(n, format, precision);
}

String String::number(double n, iuint8 format, iuint32 precision)
{
    String str;
    return str.setNumber(n, format, precision);
}

Char String::operator[](size_t pos) const
{
    if (pos < d->calculateSize()) {
        return d->getCharAt(pos);
    }
    return Char();
}

String &String::operator=(const String &str)
{
    if (this == &str || d == str.d) {
        return *this;
    }
    d->deref();
    str.d->ref();
    d = str.d;
    return *this;
}

String &String::operator=(const ichar *str)
{
    if (!str) {
        return *this;
    }
    d->newAndDetach(this);
    d->init(str);
    return *this;
}

String &String::operator=(Char c)
{
    d->newAndDetach(this);
    const iint32 numberOfOctets = c.octetsRequired();
    d->m_str = (ichar*) malloc((numberOfOctets + 1) * sizeof(ichar));
    const iuint32 value = c.value();
    for (iint32 i = 0; i < numberOfOctets; ++i) {
        const iint32 offset = 8 * (numberOfOctets - i - 1);
        d->m_str[i] = (value >> offset) & 0xff;
    }
    d->m_str[numberOfOctets] = '\0';
    free(d->m_charMap);
    d->m_charMap = (size_t*) malloc(sizeof(size_t));
    d->m_charMap[0] = 0;
    d->m_size = 1;
    d->m_sizeCalculated = true;
    return *this;
}

String &String::operator+=(const String &str)
{
    d->copyAndDetach(this);
    const size_t oldRawLength = strlen(d->m_str);
    const size_t newRawLength = oldRawLength + strlen(str.d->m_str);
    d->m_str = (ichar*) realloc(d->m_str, newRawLength + 1);
    union FragmentedValue {
        iuint32 value;
        ichar v[4];
    };
    FragmentedValue fragmentedValue;
    size_t pos = oldRawLength;
    for (size_t i = 0; i < str.d->calculateSize(); ++i) {
        const Char currChar = str[i];
        fragmentedValue.value = currChar.value();
        const iint32 octetsRequired = currChar.octetsRequired();
        for (iint32 j = 0; j < octetsRequired; ++j) {
            d->m_str[pos] = fragmentedValue.v[octetsRequired - j - 1];
            ++pos;
        }
    }
    d->m_str[newRawLength] = '\0';
    d->m_sizeCalculated = false;
    return *this;
}

String &String::operator+=(const ichar *str)
{
    d->copyAndDetach(this);
    const size_t rawLength = strlen(str);
    const size_t oldRawLength = strlen(d->m_str);
    const size_t newRawLength = oldRawLength + rawLength;
    d->m_str = (ichar*) realloc(d->m_str, newRawLength + 1);
    memcpy(&d->m_str[oldRawLength], str, rawLength);
    d->m_str[newRawLength] = '\0';
    d->m_sizeCalculated = false;
    return *this;
}

String &String::operator+=(Char c)
{
    d->copyAndDetach(this);
    const iint32 numberOfOctets = c.octetsRequired();
    const size_t rawLength = strlen(d->m_str);
    const size_t newRawLength = rawLength + numberOfOctets;
    const iuint32 value = c.value();
    d->m_str = (ichar*) realloc(d->m_str, newRawLength + 1);
    iint32 pos = 0;
    for (size_t i = rawLength; i < newRawLength; ++i) {
        const iint32 offset = 8 * (numberOfOctets - pos - 1);
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

String String::operator+(const ichar *str) const
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
    stream << '\"' << str.data() << '\"';
    return stream;
}
