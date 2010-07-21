/*
 * This file is part of the Ideal Library
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

#include "any.h"

#include <cxxabi.h>

namespace IdealCore {

Any::Any()
    : m_s(0)
{
}

template <>
Any::Any(const Any &any)
{
    if (any.m_s) {
        any.m_s->ref();
    }
    m_s = any.m_s;
}

Any::Any(const Any &any)
{
    if (any.m_s) {
        any.m_s->ref();
    }
    m_s = any.m_s;
}

Any::~Any()
{
    if (m_s) {
        m_s->deref();
    }
}

bool Any::isEmpty() const
{
    return m_s == 0;
}

const std::type_info &Any::type() const
{
    if (m_s) {
        return m_s->type();
    }
    return typeid(Any);
}

String Any::typeName() const
{
    ichar *const typeName = abi::__cxa_demangle((m_s ? m_s->type().name() : typeid(Any).name()), 0, 0, 0);
    String res(typeName);
    free(typeName);
    return res;
}

template <>
Any &Any::operator=(const Any &any)
{
    if (m_s == any.m_s) {
        return *this;
    }
    if (m_s) {
        m_s->deref();
    }
    if (any.m_s) {
        any.m_s->ref();
    }
    m_s = any.m_s;
    return *this;
}

Any &Any::operator=(const Any &any)
{
    if (m_s == any.m_s) {
        return *this;
    }
    if (m_s) {
        m_s->deref();
    }
    if (any.m_s) {
        any.m_s->ref();
    }
    m_s = any.m_s;
    return *this;
}

bool Any::operator==(const Any &any) const
{
    if (m_s == any.m_s) {
        return true;
    }
    return m_s->equals(any);
}

bool Any::operator!=(const Any &any) const
{
    return !(*this == any);
}

}
