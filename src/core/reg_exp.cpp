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

#include <pcrecpp.h>

namespace IdealCore {

class RegExp::Private
{
public:
    Private()
        : m_regExp("", pcrecpp::UTF8())
        , m_args(0)
        , m_argsString(0)
        , m_numCaptures(0)
        , m_refs(1)
    {
    }

    Private *copy()
    {
        Private *privateCopy = new Private;
        privateCopy->m_regExp = m_regExp;
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
            for (size_t i = 0; i < m_numCaptures; ++i) {
                delete m_args[i];
            }
            delete[] m_args;
            delete[] m_argsString;
            delete this;
        }
    }

    size_t refCount() const
    {
        return m_refs;
    }

    pcrecpp::RE    m_regExp;
    pcrecpp::Arg **m_args;
    std::string   *m_argsString;
    size_t         m_numCaptures;
    size_t         m_refs;
};

RegExp::RegExp()
    : d(new Private)
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
    d->m_regExp = pcrecpp::RE(regExp.data(), pcrecpp::UTF8());
}

RegExp::~RegExp()
{
    d->deref();
}

void RegExp::setRegExp(const String &regExp)
{
    if (regExp == d->m_regExp.pattern()) {
        return;
    }
    if (d->refCount() > 1) {
        d->deref();
        d = new Private;
    }
    d->m_regExp = pcrecpp::RE(regExp.data(), pcrecpp::UTF8());
}

String RegExp::regExp() const
{
    return d->m_regExp.pattern();
}

bool RegExp::match(const String &str, size_t numCaptures) const
{
    for (size_t i = 0; i < d->m_numCaptures; ++i) {
        delete d->m_args[i];
    }
    delete[] d->m_args;
    delete[] d->m_argsString;
    d->m_numCaptures = numCaptures;
    d->m_args = new pcrecpp::Arg*[numCaptures];
    d->m_argsString = new std::string[numCaptures];
    for (size_t i = 0; i < numCaptures; ++i) {
        d->m_args[i] = new pcrecpp::Arg;
        *d->m_args[i] = &d->m_argsString[i];
    }
    iint32 consumed;
    return d->m_regExp.DoMatch(str.data(), pcrecpp::RE::ANCHOR_BOTH, &consumed, d->m_args, numCaptures);
}

String RegExp::getCapture(size_t i) const
{
    if (i >= d->m_numCaptures) {
        return String();
    }
    return d->m_argsString[i];
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
    if (d->refCount() > 1) {
        Private *const old_d = d;
        d = d->copy();
        old_d->deref();
    }
    d->m_regExp = pcrecpp::RE(regExp.data(), pcrecpp::UTF8());
    return *this;
}

bool RegExp::operator==(const RegExp &regExp) const
{
    if (this == &regExp || d == regExp.d) {
        return true;
    }
    return d->m_regExp.pattern() == regExp.d->m_regExp.pattern();
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
