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

#ifndef ANY_H
#define ANY_H

#include <ideal_export.h>

namespace IdealCore {

/**
  * @class Any any.h core/any.h
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Any
{
public:
    Any();
    template <typename T>
    Any(T &t);
    Any(const Any &any);
    virtual ~Any();

    template <typename T>
    T get() const;

    Any &operator=(const Any &any);

private:
    class GenericStorage;
    template <typename T>
    class Storage;
    GenericStorage *m_s;
};

class Any::GenericStorage
{
public:
    GenericStorage()
        : m_refs(1)
    {
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

    virtual void *get() const = 0;

private:
    unsigned int m_refs;
};

template <typename T>
class Any::Storage
    : public GenericStorage
{
public:
    Storage(T &t)
        : m_t(&t)
    {
    }

    virtual void *get() const
    {
        return (void*) m_t;
    }

    T *const m_t;
};

template <typename T>
Any::Any(T &t)
    : m_s(new Storage<T>(t))
{
}

template <typename T>
T Any::get() const
{
    if (!m_s) {
        return T();
    }
    return *static_cast<T*>(m_s->get());
}

}

#endif //ANY_H
