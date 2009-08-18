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

#include <core/async_result.h>

namespace IdealCore {

AsyncResult::AsyncResult(Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(resultSet)
    , m_values(0)
    , m_size(0)
    , m_resultReceived(false)
{
}

AsyncResult::~AsyncResult()
{
    delete[] m_values;
}

int AsyncResult::size() const
{
    return m_size;
}

bool AsyncResult::resultReceived() const
{
    return m_resultReceived;
}

void AsyncResult::clear()
{
    delete[] m_values;
    m_values = 0;
    m_size = 0;
    m_resultReceived = false;
}

}
