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

#include <wchar.h>
#include <stdlib.h>
#include <string.h>

namespace IdealCore {

class String::Private
{
public:
    Private()
        : m_ansiStr(0)
        , m_refs(1)
    {
    }

    ~Private()
    {
        delete m_ansiStr;
    }

    Private *copy()
    {
        Private *privateCopy = new Private;
        privateCopy->m_str = m_str;
        return privateCopy;
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

    void initWideStr(const char *data)
    {
        delete m_ansiStr;
        m_ansiStr = 0;
        size_t count = mbstowcs(NULL, data, 0);
        if (count == (size_t) -1) {
            IDEAL_DEBUG_WARNING("string could not be converted");
            return;
        }
        ++count;
        wchar_t *wideStr = new wchar_t[count];
        mbstowcs(wideStr, data, count);
        m_str = wideStr;
        delete[] wideStr;
    }

    void initAnsiStr()
    {
        size_t count = wcstombs(NULL, m_str.data(), 0);
        if (count == (size_t) -1) {
            IDEAL_DEBUG_WARNING("string could not be converted");
            return;
        }
        ++count;
        char *ansiStr = new char[count];
        wcstombs(ansiStr, m_str.data(), count);
        m_ansiStr = new std::string(ansiStr);
        delete[] ansiStr;
    }

    std::wstring m_str;
    std::string* m_ansiStr;
    int          m_refs;
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
    d->initWideStr(str.data());
}

String::String(const char *str)
    : d(new Private)
{
    if (str) {
        d->initWideStr(str);
    }
}

String::String(const char *str, size_t n)
    : d(new Private)
{
    if (str) {
        const std::string theString(str, n);
        d->initWideStr(theString.data());
    }
}

String::String(wchar_t c)
    : d(new Private)
{
    d->m_str = c;
}

String::~String()
{
    d->deref();
}

void String::clear()
{
    if (d->refCount() > 1) {
        d->deref();
        d = d->copy();
    }
    d->m_str.clear();
}

bool String::empty() const
{
    return d->m_str.empty();
}

size_t String::size() const
{
    return d->m_str.size();
}

bool String::contains(wchar_t c) const
{
    return d->m_str.find(c) != std::string::npos;
}

size_t String::find(wchar_t c) const
{
    return d->m_str.find(c);
}

const char *String::data() const
{
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    return d->m_ansiStr->data();
}

String String::substr(size_t pos, size_t n) const
{
    String res;
    res.d->m_str = d->m_str.substr(pos, n);
    return res;
}

int String::compare(const char *s) const
{
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    return strcoll(d->m_ansiStr->data(), s);
}

List<String> String::split(char separator) const
{
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    List<String> res;
    size_t pos = 0;
    size_t oldPos = 0;
    while((pos = d->m_ansiStr->find(separator, pos)) != String::npos) {
        if (pos > 0 && pos <= d->m_ansiStr->size() - 1) {
            res.push_back(d->m_ansiStr->substr(oldPos, pos - oldPos));
        }
        ++pos;
        oldPos = pos;
    }
    if (oldPos <= d->m_ansiStr->size() - 1) {
        res.push_back(d->m_ansiStr->substr(oldPos));
    }
    return res;
}

wchar_t String::operator[](int pos) const
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
    if (d->refCount() > 1) {
        d->deref();
        d = d->copy();
    }
    d->initWideStr(str);
    return *this;
}

String &String::operator=(wchar_t c)
{
    if (d->refCount() > 1) {
        d->deref();
        d = d->copy();
    }
    d->m_str = c;
    delete d->m_ansiStr;
    d->m_ansiStr = 0;
    return *this;
}

String &String::operator+=(const String &str)
{
    if (d->refCount() > 1) {
        d->deref();
        d = d->copy();
    }
    d->m_str += str.d->m_str;
    delete d->m_ansiStr;
    d->m_ansiStr = 0;
    return *this;
}

String &String::operator+=(const char *str)
{
    if (d->refCount() > 1) {
        d->deref();
        d = d->copy();
    }
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    std::string res(*d->m_ansiStr);
    res.append(str);
    d->initWideStr(res.data());
    return *this;
}

String &String::operator+=(wchar_t c)
{
    if (d->refCount() > 1) {
        d->deref();
        d = d->copy();
    }
    d->m_str += c;
    delete d->m_ansiStr;
    d->m_ansiStr = 0;
    return *this;
}

String String::operator+(const String &str) const
{
    String res;
    res.d->m_str = d->m_str + str.d->m_str;
    return res;
}

String String::operator+(const char *str) const
{
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    String res;
    std::string tempRes(*d->m_ansiStr);
    tempRes.append(str);
    res.d->initWideStr(tempRes.data());
    return res;
}

String String::operator+(wchar_t c) const
{
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    String res;
    res.d->m_str = d->m_str + c;
    return res;
}

bool String::operator==(const String &str) const
{
    if (this == &str) {
        return true;
    }
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    if (!str.d->m_ansiStr) {
        str.d->initAnsiStr();
    }
    return !strcoll(d->m_ansiStr->data(), str.d->m_ansiStr->data());
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
    if (!d->m_ansiStr) {
        d->initAnsiStr();
    }
    if (!str.d->m_ansiStr) {
        str.d->initAnsiStr();
    }
    return strcoll(d->m_ansiStr->data(), str.d->m_ansiStr->data()) < 0;
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
