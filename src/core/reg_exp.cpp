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

#include "reg_exp.h"

#include <vector>
#include <pcre.h>
#include <string.h>

namespace IdealCore {

class RegExp::Private
{
public:
    Private()
        : m_pcre(0)
        , m_refs(1)
    {
    }

    ~Private()
    {
        if (m_pcre) {
            pcre_free(m_pcre);
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
            if (this == m_privateEmpty) {
                m_privateEmpty = 0;
            }
            delete this;
        }
    }

    size_t refCount() const
    {
        return m_refs;
    }

    static Private *empty();

    String              m_regExp;
    pcre               *m_pcre;
    std::vector<String> m_captures;
    size_t              m_refs;

    class PrivateEmpty;
    static Private *m_privateEmpty;
};

RegExp::Private *RegExp::Private::m_privateEmpty = 0;

class RegExp::Private::PrivateEmpty
    : public Private
{
public:
    PrivateEmpty()
    {
    }

    virtual ~PrivateEmpty()
    {
    }
};

RegExp::Private *RegExp::Private::empty()
{
    if (!m_privateEmpty) {
        m_privateEmpty = new PrivateEmpty;
    } else {
        m_privateEmpty->ref();
    }
    return m_privateEmpty;
}

RegExp::RegExp()
    : d(Private::empty())
{
}

RegExp::RegExp(const RegExp &regExp)
{
    regExp.d->ref();
    d = regExp.d;
}

RegExp::RegExp(const String &regExp)
    : d(new Private)
{
    d->m_regExp = regExp;
}

RegExp::~RegExp()
{
    d->deref();
}

void RegExp::setRegExp(const String &regExp)
{
    if (regExp == d->m_regExp) {
        return;
    }
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->m_regExp = regExp;
    if (d->m_pcre) {
        pcre_free(d->m_pcre);
    }
    d->m_pcre = 0;
}

String RegExp::regExp() const
{
    return d->m_regExp;
}

bool RegExp::match(const String &str) const
{
    d->m_captures.clear();
    if (!d->m_pcre) {
        const char *error;
        iint32 erroffset;
        d->m_pcre = pcre_compile(d->m_regExp.data(), PCRE_UTF8, &error, &erroffset, 0);
    }
    if (d->m_pcre) {
        const iint32 oveccount = (str.size() + d->m_regExp.size()) * 3;
        iint32 *const ovector = (iint32*) calloc(oveccount, sizeof(iint32));
        const iint32 rc = pcre_exec(d->m_pcre, 0, str.data(), strlen(str.data()), 0, 0, ovector, oveccount);
        if (rc > 0) {
            for (iint32 i = 1; i < rc; ++i) {
                d->m_captures.push_back(str.substr(ovector[2 * i], ovector[2 * i + 1] - ovector[2 * i]));
            }
        }
        free(ovector);
        return rc > 0;
    }
    return false;
}

size_t RegExp::numCaptures() const
{
    return d->m_captures.size();
}

String RegExp::getCapture(size_t i) const
{
    if (i >= d->m_captures.size()) {
        return String();
    }
    return d->m_captures.at(i);
}

RegExp &RegExp::operator=(const RegExp &regExp)
{
    if (this == &regExp || d == regExp.d) {
        return *this;
    }
    d->deref();
    regExp.d->ref();
    d = regExp.d;
    return *this;
}

RegExp &RegExp::operator=(const String &regExp)
{
    d->deref();
    d = new Private;
    d->m_regExp = regExp;
    return *this;
}

bool RegExp::operator==(const RegExp &regExp) const
{
    if (this == &regExp || d == regExp.d) {
        return true;
    }
    return d->m_regExp == regExp.d->m_regExp;
}

bool RegExp::operator!=(const RegExp &regExp) const
{
    return !(*this == regExp);
}

}

std::ostream &operator<<(std::ostream &stream, const IdealCore::RegExp &regExp)
{
    stream << '\"' << regExp.regExp().data() << '\"';
    return stream;
}
