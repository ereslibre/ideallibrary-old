/*
 * This file is part of the Ideal Library
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@ereslibre.es>
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

#ifndef IDEAL_VECTOR_H
#define IDEAL_VECTOR_H

#include <ideal_globals.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CONTAINER_SIZE 5

namespace IdealCore {

/**
  * @class Vector vector.h core/vector.h
  *
  * This class represents a vector. It will grow or shrink dynamically depending on how many
  * elements it contains.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
template <typename T>
class Vector
{
public:
    Vector();
    Vector(const Vector &vector);
    virtual ~Vector();

    void append(const T &t);
    void prepend(const T &t);
    void insertAt(const T &t, size_t i);
    void removeAt(size_t i);

    void clear();

    size_t size() const;
    bool isEmpty() const;

    T &operator[](size_t i);
    const T&operator[](size_t i) const;

private:
    class Private;
    Private *d;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Vector<T>::Private
{
public:
    Private();
    virtual ~Private();

    Private *copy() const;
    void copyAndDetach(Vector<T> *vector);
    void newAndDetach(Vector<T> *vector);
    void clearContents();

    void ref();
    void deref();

    static Private *empty();

    T     *m_vector;
    size_t m_size;
    size_t m_containerSize;
    size_t m_refs;

    static Private *m_privateEmpty;
    static T        m_emptyRes;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector<T>::Private::Private()
    : m_vector(0)
    , m_size(0)
    , m_containerSize(0)
    , m_refs(1)
{
}

template <typename T>
Vector<T>::Private::~Private()
{
    free(m_vector);
}

template <typename T>
typename Vector<T>::Private *Vector<T>::Private::copy() const
{
    Private *privateCopy = new Private;
    privateCopy->m_vector = (T*) calloc(m_containerSize, sizeof(T));
    memcpy(privateCopy->m_vector, m_vector, m_containerSize * sizeof(T));
    privateCopy->m_size = m_size;
    privateCopy->m_containerSize = m_containerSize;
    return privateCopy;
}

template <typename T>
void Vector<T>::Private::copyAndDetach(Vector<T> *vector)
{
    if (m_refs > 1) {
        vector->d = copy();
        deref();
    } else if (this == m_privateEmpty) {
        m_privateEmpty = 0;
    }
}

template <typename T>
void Vector<T>::Private::newAndDetach(Vector<T> *vector)
{
    if (m_refs > 1) {
        vector->d = new Private;
        deref();
    } else if (this == m_privateEmpty) {
        m_privateEmpty = 0;
    } else {
        clearContents();
    }
}

template <typename T>
void Vector<T>::Private::clearContents()
{
    free(m_vector);
    m_vector = 0;
    m_size = 0;
    m_containerSize = 0;
}

template <typename T>
void Vector<T>::Private::ref()
{
    ++m_refs;
}

template <typename T>
void Vector<T>::Private::deref()
{
    --m_refs;
    if (!m_refs) {
        if (this == m_privateEmpty) {
            m_privateEmpty = 0;
        }
        delete this;
    }
}

template <typename T>
typename Vector<T>::Private *Vector<T>::Private::empty()
{
    if (!m_privateEmpty) {
        m_privateEmpty = new Private;
    } else {
        m_privateEmpty->ref();
    }
    return m_privateEmpty;
}

template <typename T>
typename Vector<T>::Private *Vector<T>::Private::m_privateEmpty = 0;

template <typename T>
T Vector<T>::Private::m_emptyRes = T();

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector<T>::Vector()
    : d(Private::empty())
{
}

template <typename T>
Vector<T>::Vector(const Vector &vector)
{
    vector.d->ref();
    d = vector.d;
}

template <typename T>
Vector<T>::~Vector()
{
    d->deref();
}

template <typename T>
void Vector<T>::append(const T &t)
{
    d->copyAndDetach(this);
    if (!d->m_size) {
        d->m_vector = (T*) calloc(INITIAL_CONTAINER_SIZE, sizeof(T));
        d->m_containerSize = INITIAL_CONTAINER_SIZE;
    } else if (d->m_size > (0.7 * d->m_containerSize)) {
        d->m_containerSize *= 2;
        d->m_vector = (T*) realloc(d->m_vector, d->m_containerSize * sizeof(T));
        memset(&d->m_vector[d->m_size + 1], '\0', d->m_containerSize * sizeof(T) - (d->m_size + 1) * sizeof(T));
    }
    d->m_vector[d->m_size] = t;
    ++d->m_size;
}

template <typename T>
void Vector<T>::prepend(const T &t)
{
    d->copyAndDetach(this);
    if (!d->m_size) {
        d->m_vector = (T*) calloc(INITIAL_CONTAINER_SIZE, sizeof(T));
        d->m_containerSize = INITIAL_CONTAINER_SIZE;
    } else if (d->m_size > (0.7 * d->m_containerSize)) {
        d->m_containerSize *= 2;
        d->m_vector = (T*) realloc(d->m_vector, d->m_containerSize * sizeof(T));
        memmove(&d->m_vector[1], d->m_vector, d->m_size * sizeof(T));
        memset(&d->m_vector[d->m_size + 1], '\0', d->m_containerSize * sizeof(T) - (d->m_size + 1) * sizeof(T));
    } else {
        memmove(&d->m_vector[1], d->m_vector, d->m_size * sizeof(T));
    }
    d->m_vector[0] = t;
    ++d->m_size;
}

template <typename T>
void Vector<T>::insertAt(const T &t, size_t i)
{
    if (i > d->m_size) {
        return;
    }
    d->copyAndDetach(this);
    if (!d->m_size) {
        d->m_vector = (T*) calloc(INITIAL_CONTAINER_SIZE, sizeof(T));
        d->m_containerSize = INITIAL_CONTAINER_SIZE;
    } else if (d->m_size > (0.7 * d->m_containerSize)) {
        d->m_containerSize *= 2;
        d->m_vector = (T*) realloc(d->m_vector, d->m_containerSize * sizeof(T));
        memmove(&d->m_vector[i + 1], &d->m_vector[i], (d->m_size - i) * sizeof(T));
        memset(&d->m_vector[d->m_size + 1], '\0', d->m_containerSize * sizeof(T) - (d->m_size + 1) * sizeof(T));
    } else {
        memmove(&d->m_vector[i + 1], &d->m_vector[i], (d->m_size - i) * sizeof(T));
    }
    d->m_vector[i] = t;
    ++d->m_size;
}

template <typename T>
void Vector<T>::removeAt(size_t i)
{
    if (i >= d->m_size) {
        return;
    }
    d->copyAndDetach(this);
    if (d->m_containerSize > 5 && d->m_size < (0.3 * d->m_containerSize)) {
        d->m_containerSize /= 2;
        d->m_vector = (T*) realloc(d->m_vector, d->m_containerSize * sizeof(T));
        if (i != d->m_size - 1) {
            memmove(&d->m_vector[i], &d->m_vector[i + 1], (d->m_size - i - 1) * sizeof(T));
        }
        memset(&d->m_vector[d->m_size - 1], '\0', sizeof(T));
    } else {
        if (i != d->m_size - 1) {
            memmove(&d->m_vector[i], &d->m_vector[i + 1], (d->m_size - i - 1) * sizeof(T));
        }
        memset(&d->m_vector[d->m_size - 1], '\0', sizeof(T));
    }
    --d->m_size;
}

template <typename T>
void Vector<T>::clear()
{
    if (d == Private::m_privateEmpty) {
        return;
    }
    d->deref();
    d = Private::empty();
}

template <typename T>
size_t Vector<T>::size() const
{
    return d->m_size;
}

template <typename T>
bool Vector<T>::isEmpty() const
{
    return !d->m_size;
}

template <typename T>
T &Vector<T>::operator[](size_t i)
{
    if (i >= d->m_size) {
        d->m_emptyRes = T();
        return d->m_emptyRes;
    }
    d->copyAndDetach(this);
    return d->m_vector[i];
}

template <typename T>
const T&Vector<T>::operator[](size_t i) const
{
    if (i >= d->m_size) {
        d->m_emptyRes = T();
        return d->m_emptyRes;
    }
    return d->m_vector[i];
}

}

#endif //IDEAL_VECTOR_H
