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

#include "ideal_locale.h"
#include "private/ideal_locale_p.h"

namespace IdealCore {

Locale::Private::Private(Locale *q)
    : m_refs(1)
    , q(q)
{
}

Locale::Private::~Private()
{
}

void Locale::Private::ref()
{
    ++m_refs;
}

void Locale::Private::deref()
{
    --m_refs;
    if (!m_refs) {
        delete this;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Locale::Locale()
    : d(new PrivateImpl(this))
{
}

Locale::Locale(const Locale &locale)
    : d(locale.d)
{
    locale.d->ref();
}

Locale::~Locale()
{
    d->deref();
}

}
