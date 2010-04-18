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

#include "mutex.h"
#include "private/mutex_p.h"

namespace IdealCore {

Mutex::Private::Private()
    : m_refs(1)
{
}

Mutex::Private::~Private()
{
}

void Mutex::Private::ref()
{
    ++m_refs;
}

void Mutex::Private::deref()
{
    --m_refs;
    if (!m_refs) {
        delete this;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Mutex::Mutex(RecursionType recursionType)
    : d(new PrivateImpl(recursionType))
{
}

Mutex::Mutex(const Mutex &mutex)
{
    mutex.d->ref();
    d = mutex.d;
}

Mutex::~Mutex()
{
    d->deref();
}

bool Mutex::operator==(const Mutex &mutex) const
{
    return (d == mutex.d);
}

bool Mutex::operator!=(const Mutex &mutex) const
{
    return !(*this == mutex);
}

}
