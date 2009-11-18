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
        : m_str((ichar*) malloc(sizeof(ichar)))
        , m_strMutex(Mutex::Recursive)
        , m_charMap(0)
        , m_charMapMutex(Mutex::Recursive)
        , m_size(0)
        , m_sizeMutex(Mutex::Recursive)
        , m_sizeCalculated(true)
        , m_refs(1)
        , m_refsMutex(Mutex::Recursive)
    {
        *m_str = '\0';
    }

    virtual ~Private()
    {
        {
            ContextMutexLocker cml(m_strMutex);
            free(m_str);
        }
        {
            ContextMutexLocker cml(m_charMapMutex);
            free(m_charMap);
        }
    }

    void init(const ichar *str)
    {
        const size_t rawLen = strlen(str);
        {
            ContextMutexLocker cml(m_strMutex);
            free(m_str);
            m_str = (ichar*) malloc(sizeof(ichar) * (rawLen + 1));
            memcpy(m_str, str, rawLen);
            m_str[rawLen] = '\0';
        }
        {
            ContextMutexLocker cml(m_sizeCalculatedMutex);
            m_sizeCalculated = false;
        }
    }

    Private *copy()
    {
        Private *const privateCopy = new Private;
        {
            ContextMutexLocker cml(m_strMutex);
            const size_t rawLen = strlen(m_str);
            free(privateCopy->m_str);
            privateCopy->m_str = (ichar*) malloc(sizeof(ichar) * (rawLen + 1));
            memcpy(privateCopy->m_str, m_str, rawLen);
            privateCopy->m_str[rawLen] = '\0';
        }
        {
            ContextMutexLocker cml(m_charMapMutex);
            privateCopy->m_charMap = (size_t*) malloc(sizeof(size_t) * calculateSize());
            ContextMutexLocker cml2(m_sizeMutex);
            memcpy(privateCopy->m_charMap, m_charMap, m_size * sizeof(size_t));
            privateCopy->m_size = m_size;
        }
        {
            ContextMutexLocker cml(m_sizeCalculatedMutex);
            privateCopy->m_sizeCalculated = true;
        }
        return privateCopy;
    }

    size_t calculateSize()
    {
        {
            ContextMutexLocker cml(m_sizeCalculatedMutex);
            if (m_sizeCalculated) {
                ContextMutexLocker cml(m_sizeMutex);
                return m_size;
            }
            m_sizeCalculated = true;
        }
        ContextMutexLocker cml(m_charMapMutex);
        free(m_charMap);
        ContextMutexLocker cml2(m_strMutex);
        const size_t rawLen = strlen(m_str);
        m_charMap = (size_t*) malloc(sizeof(size_t) * rawLen);
        bzero(m_charMap, rawLen * sizeof(size_t));
        size_t i = 0;
        size_t size = 0;
        while (true) {
            const ichar c = m_str[i];
            if (c == '\0') {
                break;
            }
            if (!(c & 0x80)) {
                m_charMap[size] = i;
                ++size;
            } else if (!(c & 0x20)) {
                m_charMap[size] = i;
                ++i;
                ++size;
            } else if (!(c & 0x10)) {
                m_charMap[size] = i;
                i += 2;
                ++size;
            } else if (!(c & 0x8)) {
                m_charMap[size] = i;
                i += 3;
                ++size;
            }
            ++i;
        }
        if (size < rawLen) {
            m_charMap = (size_t*) realloc(m_charMap, size * sizeof(size_t));
        }
        {
            ContextMutexLocker cml(m_sizeMutex);
            m_size = size;
        }
        return size;
    }

    void ref()
    {
        ContextMutexLocker cml(m_refsMutex);
        ++m_refs;
    }

    void deref()
    {
        m_refsMutex.lock();
        --m_refs;
        if (!m_refs) {
            m_refsMutex.unlock();
            delete this;
        } else {
            m_refsMutex.unlock();
        }
    }

    size_t refCount()
    {
        ContextMutexLocker cml(m_refsMutex);
        return m_refs;
    }

    Char getCharAt(size_t pos)
    {
        Char res;
        size_t mappedPos = 0;
        {
            ContextMutexLocker cml(m_charMapMutex);
            mappedPos = m_charMap[pos];
        }
        ichar c = 0;
        {
            ContextMutexLocker cml(m_strMutex);
            c = m_str[mappedPos];
        }
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
        ContextMutexLocker cml(m_strMutex);
        for (iuint32 i = 0; i < numberOfOctets; ++i) {
            res.c |= (m_str[mappedPos + i] & 0xff) << 8 * (numberOfOctets - i - 1);
        }
        return res;
    }

    void iint64toa(iint64 number, iuint32 base)
    {
        iuint64toa(number < 0 ? -number : number, base);
    }

    void iuint64toa(iuint64 number, iuint32 base)
    {
        ichar *const str = (ichar*) malloc(sizeof(ichar) * 66);
        ichar *p = str + 64;
        bzero(str, 66);
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
        init(++p);
        free(str);
    }

    void dtoa(double number, iuint8 format, iuint32 precision)
    {
        ichar *const str = (ichar*) malloc(sizeof(ichar) * 10);
        bzero(str, 5);
        sprintf(str, "%%.%dl%c", precision, format);
        ichar *const res = (ichar*) malloc(sizeof(ichar) * 65);
        bzero(res, 65);
        sprintf(res, str, number);
        init(res);
        free(str);
        free(res);
    }

    ichar  *m_str;
    Mutex   m_strMutex;
    size_t *m_charMap;
    Mutex   m_charMapMutex;
    size_t  m_size;
    Mutex   m_sizeMutex;
    bool    m_sizeCalculated;
    Mutex   m_sizeCalculatedMutex;
    size_t  m_refs;
    Mutex   m_refsMutex;
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
    d->init(str.data());
}

String::String(const ichar *str)
    : d(new Private)
{
    if (str) {
        d->init(str);
    }
}

String::String(const ichar *str, size_t n)
    : d(new Private)
{
    if (str && n) {
        const size_t rawLength = strlen(str);
        ContextMutexLocker cml(d->m_strMutex);
        free(d->m_str);
        d->m_str = (ichar*) malloc(sizeof(ichar) * ((n == npos ? rawLength : n) * 4 + 1));
        ContextMutexLocker cml2(d->m_charMapMutex);
        d->m_charMap = (size_t*) malloc(sizeof(size_t) * (n == npos ? rawLength : n));
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
        {
            ContextMutexLocker cml(d->m_sizeMutex);
            d->m_size = count;
        }
        {
            ContextMutexLocker cml(d->m_sizeCalculatedMutex);
            d->m_sizeCalculated = true;
        }
        if (curr < (n == npos ? rawLength * 4 : n * 4)) {
            d->m_str = (ichar*) realloc(d->m_str, curr + 1);
        }
        if (count < (n == npos ? rawLength : n)) {
            d->m_charMap = (size_t*) realloc(d->m_charMap, count * sizeof(size_t));
        }
    }
}

String::String(Char c)
    : d(new Private)
{
    const iint32 numberOfOctets = c.octetsRequired();
    ContextMutexLocker cml(d->m_strMutex);
    free(d->m_str);
    d->m_str = (ichar*) malloc(sizeof(ichar) * (numberOfOctets + 1));
    const iuint32 value = c.value();
    for (iint32 i = 0; i < numberOfOctets; ++i) {
        const iint32 offset = 8 * (numberOfOctets - i - 1);
        d->m_str[i] = (value >> offset) & 0xff;
    }
    d->m_str[numberOfOctets] = '\0';
    {
        ContextMutexLocker cml(d->m_charMapMutex);
        d->m_charMap = (size_t*) malloc(sizeof(size_t));
        d->m_charMap[0] = 0;
    }
    {
        ContextMutexLocker cml(d->m_sizeMutex);
        d->m_size = 1;
    }
    {
        ContextMutexLocker cml(d->m_sizeCalculatedMutex);
        d->m_sizeCalculated = true;
    }
}

String::~String()
{
    d->deref();
}

void String::clear()
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    } else {
        {
            ContextMutexLocker cml(d->m_strMutex);
            free(d->m_str);
            d->m_str = (ichar*) malloc(sizeof(ichar));
            *d->m_str = '\0';
        }
        {
            ContextMutexLocker cml(d->m_charMapMutex);
            free(d->m_charMap);
            d->m_charMap = 0;
        }
        {
            ContextMutexLocker cml(d->m_sizeMutex);
            d->m_size = 0;
        }
        {
            ContextMutexLocker cml(d->m_sizeCalculatedMutex);
            d->m_sizeCalculated = true;
        }
    }
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
    ContextMutexLocker cml(d->m_sizeMutex);
    size_t offset = 0;
    {
        ContextMutexLocker cml(str.d->m_sizeMutex);
        if (!d->calculateSize() || !str.d->calculateSize() || str.d->m_size > d->m_size) {
            return npos;
        }
        offset = str.d->m_size - 1;
    }
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
    ContextMutexLocker cml(d->m_strMutex);
    return d->m_str;
}

String String::substr(size_t pos, size_t n) const
{
    if (pos < d->calculateSize()) {
        ContextMutexLocker cml(d->m_strMutex);
        ContextMutexLocker cml2(d->m_charMapMutex);
        return String(&d->m_str[d->m_charMap[pos]], n);
    }
    return String();
}

iint32 String::compare(const ichar *s) const
{
    ContextMutexLocker cml(d->m_strMutex);
    return strcoll(d->m_str, s);
}

List<String> String::split(Char separator) const
{
    List<String> res;
    if (!d->calculateSize()) {
        return res;
    }
    size_t length = 0;
    {
        ContextMutexLocker cml(d->m_strMutex);
        length = strlen(d->m_str);
    }
    ichar *curr = (ichar*) malloc(sizeof(ichar) * (length + 1));
    bzero(curr, length + 1);
    size_t pos = 0;
    ContextMutexLocker cml(d->m_sizeMutex);
    for (size_t i = 0; i < d->m_size; ++i) {
        const Char currChar = d->getCharAt(i);
        if (pos && currChar == separator) {
            curr[pos] = '\0';
            res.push_back(String(curr));
            pos = 0;
            bzero(curr, length + 1);
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
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    ichar *curr = 0;
    {
        size_t rawLength = 0;
        ContextMutexLocker cml(d->m_strMutex);
        {
            ContextMutexLocker cml(str.d->m_strMutex);
            rawLength = strlen(d->m_str) + strlen(str.d->m_str);
        }
        curr = (ichar*) malloc(sizeof(ichar) * (rawLength + 1));
        bzero(curr, rawLength + 1);
        sprintf(curr, "%s%s", str.data(), d->m_str);
    }
    d->init(curr);
    free(curr);
    return *this;
}

String &String::prepend(const ichar *str)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    ichar *curr = 0;
    {
        ContextMutexLocker cml(d->m_strMutex);
        const size_t rawLength = strlen(d->m_str) + strlen(str);
        curr = (ichar*) malloc(sizeof(ichar) * (rawLength + 1));
        bzero(curr, rawLength + 1);
        sprintf(curr, "%s%s", str, d->m_str);
    }
    d->init(curr);
    free(curr);
    return *this;
}

String &String::prepend(Char c)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    const iint32 numberOfOctets = c.octetsRequired();
    ContextMutexLocker cml(d->m_strMutex);
    const size_t rawLength = strlen(d->m_str) + numberOfOctets;
    ichar *curr = (ichar*) malloc(sizeof(ichar) * (rawLength + 1));
    bzero(curr, rawLength + 1);
    const iuint32 value = c.value();
    switch (numberOfOctets) {
        case 1:
            sprintf(curr, "%c%s", value & 0xff, d->m_str);
            break;
        case 2:
            sprintf(curr, "%c%c%s", (value >> 8) & 0xff, value & 0xff, d->m_str);
            break;
        case 3:
            sprintf(curr, "%c%c%c%s", (value >> 16) & 0xff, (value >> 8) & 0xff, value & 0xff, d->m_str);
            break;
        case 4:
            sprintf(curr, "%c%c%c%c%s", (value >> 24) & 0xff, (value >> 16) & 0xff, (value >> 8) & 0xff, value & 0xff, d->m_str);
            break;
        default:
            break;
    }
    d->init(curr);
    free(curr);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
    const double res = strtod(d->m_str, &p);
    if (ok) {
        *ok = (p != d->m_str);
    }
    return res;
}

String &String::setNumber(iint32 n, iuint32 base)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->iint64toa(n, base);
    return *this;
}

String &String::setNumber(iuint32 n, iuint32 base)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->iuint64toa(n, base);
    return *this;
}

String &String::setNumber(long n, iuint32 base)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->iint64toa(n, base);
    return *this;
}

String &String::setNumber(iulong n, iuint32 base)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->iuint64toa(n, base);
    return *this;
}

String &String::setNumber(iint64 n, iuint32 base)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->iint64toa(n, base);
    return *this;
}

String &String::setNumber(iuint64 n, iuint32 base)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->iuint64toa(n, base);
    return *this;
}

String &String::setNumber(float n, iuint8 format, iuint32 precision)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->dtoa(n, format, precision);
    return *this;
}

String &String::setNumber(double n, iuint8 format, iuint32 precision)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
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
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->init(str);
    return *this;
}

String &String::operator=(Char c)
{
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    const iint32 numberOfOctets = c.octetsRequired();
    {
        ContextMutexLocker cml(d->m_strMutex);
        free(d->m_str);
        d->m_str = (ichar*) malloc(sizeof(ichar) * (numberOfOctets + 1));
        const iuint32 value = c.value();
        for (iint32 i = 0; i < numberOfOctets; ++i) {
            const iint32 offset = 8 * (numberOfOctets - i - 1);
            d->m_str[i] = (value >> offset) & 0xff;
        }
        d->m_str[numberOfOctets] = '\0';
    }
    {
        ContextMutexLocker cml(d->m_charMapMutex);
        free(d->m_charMap);
        d->m_charMap = (size_t*) malloc(sizeof(size_t));
        d->m_charMap[0] = 0;
    }
    {
        ContextMutexLocker cml(d->m_sizeMutex);
        d->m_size = 1;
    }
    {
        ContextMutexLocker cml(d->m_sizeCalculatedMutex);
        d->m_sizeCalculated = true;
    }
    return *this;
}

String &String::operator+=(const String &str)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    {
        ContextMutexLocker cml(d->m_strMutex);
        const size_t oldRawLength = strlen(d->m_str);
        size_t newRawLength = 0;
        {
            ContextMutexLocker cml(str.d->m_strMutex);
            newRawLength = oldRawLength + strlen(str.d->m_str);
        }
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
    }
    {
        ContextMutexLocker cml(d->m_sizeCalculatedMutex);
        d->m_sizeCalculated = false;
    }
    return *this;
}

String &String::operator+=(const ichar *str)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    const size_t rawLength = strlen(str);
    {
        ContextMutexLocker cml(d->m_strMutex);
        const size_t oldRawLength = strlen(d->m_str);
        const size_t newRawLength = oldRawLength + rawLength;
        d->m_str = (ichar*) realloc(d->m_str, newRawLength + 1);
        memcpy(&d->m_str[oldRawLength], str, rawLength);
        d->m_str[newRawLength] = '\0';
    }
    {
        ContextMutexLocker cml(d->m_sizeCalculatedMutex);
        d->m_sizeCalculated = false;
    }
    return *this;
}

String &String::operator+=(Char c)
{
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    const iint32 numberOfOctets = c.octetsRequired();
    {
        ContextMutexLocker cml(d->m_strMutex);
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
    }
    {
        ContextMutexLocker cml(d->m_sizeCalculatedMutex);
        d->m_sizeCalculated = false;
    }
    return *this;
}

String String::operator+(const String &str) const
{
    ContextMutexLocker cml(d->m_strMutex);
    String res(d->m_str);
    res += str;
    return res;
}

String String::operator+(const ichar *str) const
{
    ContextMutexLocker cml(d->m_strMutex);
    String res(d->m_str);
    res += str;
    return res;
}

String String::operator+(Char c) const
{
    ContextMutexLocker cml(d->m_strMutex);
    String res(d->m_str);
    res += c;
    return res;
}

bool String::operator==(const String &str) const
{
    if (this == &str || d == str.d) {
        return true;
    }
    ContextMutexLocker cml(d->m_strMutex);
    ContextMutexLocker cml2(str.d->m_strMutex);
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
    ContextMutexLocker cml(d->m_strMutex);
    ContextMutexLocker cml2(str.d->m_strMutex);
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
