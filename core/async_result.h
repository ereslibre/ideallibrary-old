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

#ifndef ASYNC_RESULT_H
#define ASYNC_RESULT_H

#include <boost/any.hpp>

#include <ideal_export.h>
#include <core/object.h>

namespace IdealCore {

class IDEAL_EXPORT AsyncResult
    : public Object
{
public:
    AsyncResult(Object *parent);
    ~AsyncResult();

    template <typename... Values>
    void set(const Values&... values);

    template <typename T>
    T get(int i) const;

private:
    boost::any *m_values;
    int         m_size;
};

template <typename... Values>
void AsyncResult::set(const Values&... values)
{
    if (m_values) {
        delete[] m_values;
    }
    m_size = sizeof...(Values);
    m_values = new boost::any[m_size];
    boost::any val[sizeof...(Values)] = {values...};
    for (int i = 0; i < m_size; ++i) {
        m_values[i] = val[i];
    }
}

template <typename T>
T AsyncResult::get(int i) const
{
    if (i >= 0 && i < m_size) {
        return boost::any_cast<T>(m_values[i]);
    }
    IDEAL_DEBUG_WARNING("the requested parameter is out of range");
    return T();
}

}

#endif //ASYNC_RESULT_H
