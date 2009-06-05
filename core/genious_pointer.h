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

#ifndef GENIOUS_POINTER_H
#define GENIOUS_POINTER_H

#include <ideal_export.h>
#include <core/signal_resource.h>

namespace IdealCore {

/**
  * @class GeniousPointer genious_pointer.h core/genious_pointer.h
  *
  * A smart pointer in the sense that if the contents gets deleted, this
  * pointer will point to 0 automatically. This way, it is impossible to
  * have dangle pointers using GeniousPointer. Example:
  *
  * @code
  * MyObject *myObject = new MyObject; // MyObject class inherits IdealCore::Object
  * GeniousPointer<MyObject> myPointer(myObject);
  * myPointer->someMethod();
  * delete myObject;
  * myPointer->someMethod(); // error, myPointer is pointing to 0
  * @endcode
  *
  * @note For this to work, T class has to inherit IdealCore::Object.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
template <typename T>
class GeniousPointer
    : public SignalResource
{
public:
    /**
      * Creates the genious pointer with content @p content.
      *
      * @note T has to inherit IdealCore::Object.
      */
    GeniousPointer(T *content);

    GeniousPointer(const GeniousPointer &ptr);

    /**
      * @note This will not destroy the contents.
      */
    ~GeniousPointer();

    /**
      * @internal
      */
    void contentDestroyed();

    /**
      * @return The content. 0 if the content was destroyed.
      */
    T *content() const;

    /**
      * @return The content. 0 if the content was destroyed.
      *
      * @note This allows you to use GeniousPointer as a normal pointer.
      */
    T *operator->() const;

    GeniousPointer &operator=(const GeniousPointer &ptr);

    /**
      * @return Whether the content was destroyed or not.
      *
      * @note This is equivalent to ask content() == 0.
      */
    bool isContentDestroyed() const;

    /**
      * @internal
      */
    bool areSignalsBlocked() const;

private:
    T *m_t;
};

template <typename T>
GeniousPointer<T>::GeniousPointer(T *content)
    : m_t(content)
{
    if (m_t) {
        Object::connect(m_t->destroyed, this, &GeniousPointer<T>::contentDestroyed);
    }
}

template <typename T>
GeniousPointer<T>::GeniousPointer(const GeniousPointer &ptr)
    : m_t(ptr.m_t)
{
    if (m_t) {
        Object::connect(m_t->destroyed, this, &GeniousPointer<T>::contentDestroyed);
    }
}


template <typename T>
GeniousPointer<T>::~GeniousPointer()
{
    if (m_t) {
        Object::disconnect(m_t->destroyed, this);
    }
 }

template <typename T>
void GeniousPointer<T>::contentDestroyed()
{
    m_t = 0;
}

template <typename T>
T *GeniousPointer<T>::content() const
{
    return m_t;
}

template <typename T>
T *GeniousPointer<T>::operator->() const
{
    return m_t;
}

template <typename T>
GeniousPointer<T> &GeniousPointer<T>::operator=(const GeniousPointer &ptr)
{
    if (m_t) {
        Object::disconnect(m_t->destroyed, this);
    }
    m_t = ptr.m_t;
    if (m_t) {
        Object::connect(m_t->destroyed, this, &GeniousPointer<T>::contentDestroyed);
    }
    return *this;
}


template <typename T>
bool GeniousPointer<T>::isContentDestroyed() const
{
    return m_t == 0;
}

template <typename T>
bool GeniousPointer<T>::areSignalsBlocked() const
{
    return false;
}

}

#endif
