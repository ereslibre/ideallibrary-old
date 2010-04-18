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

#include <ideal_export.h>
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
  * It provides a very easy way of storing and accessing to information. It also provides helpful
  * Java-like iterators.
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

    /**
      * Appends element @p t to the vector.
      */
    void append(const T &t);

    /**
      * Prepends element @p to the vector.
      */
    void prepend(const T &t);

    /**
      * Inserts element @p t at position @p i in the vector.
      *
      * @note All positions start counting from 0.
      */
    void insertAt(const T &t, size_t i);

    /**
      * Removes element at position @p i in the vector.
      *
      * @note All positions start counting from 0.
      */
    void removeAt(size_t i);

    /**
      * Clears the content of this vector.
      */
    void clear();

    /**
      * @return The number of elements this vector is currently holding.
      */
    size_t size() const;

    /**
      * @return Whether this vector is empty or not.
      */
    bool isEmpty() const;

    /**
      * @return The element at position @p i as a non-const reference.
      *
      * @note Since a non-const reference is returned, this can cause the vector to perform a deep
      *       copy of the information. If you are not modifying any elements, please use the operator[]
      *       returning a const reference.
      */
    T &operator[](size_t i);

    /**
      * @return The element at position @p i as a const reference.
      *
      * @note This is the preferred way to access to information if you do not plan to modify it. It
      *       will never perform a deep copy of the information.
      */
    const T&operator[](size_t i) const;

    /**
      * Appends element @p t to the vector.
      */
    Vector<T> &operator<<(const T &t);

    /**
      * @return Whether this vector is equals to @p vector at contents level.
      *
      * @note The class being saved at the vector has to implement operator==.
      */
    bool operator==(const Vector<T> &vector) const;

    /**
      * @return Whether this vector is different to @p vector at contents level.
      *
      * @note The class being saved at the vector has to implement operator==.
      */
    bool operator!=(const Vector<T> &vector) const;

////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
      * @class Iterator
      *
      * This class implements an iterator able to alter the contents of the vector being iterated,
      * as well modifying its structure.
      *
      * Using this iterator could force a deep copy of the vector being iterated. If you do not plan
      * to modify the vector, please use ConstIterator instead.
      *
      * A typical example follows:
      *
      * @code
      * IdealCore::Vector<MyClass>::Iterator it(myVector);
      * while (it.hasNext()) {
      *     MyClass &c = it.next();
      *     // Do whatever with c
      * }
      * @endcode
      *
      * @author Rafael Fernández López <ereslibre@ereslibre.es>
      */
    class Iterator
    {
    public:
        /**
          * Constructs a non-const iterator for @p vector starting at @p initialPos.
          */
        Iterator(Vector<T> &vector, size_t initialPos = 0);
        virtual ~Iterator();

        /**
          * @return Whether we can call to @p next for continuing data fetching.
          */
        bool hasNext() const;

        /**
          * @return The element at the current iterator position.
          */
        T &next();

        /**
          * Inserts element @p t before the current iterator position.
          *
          * @code
          * [ a b c x d e ... ]
          *             ^ Next iterator position in case next() is feasible.
          *           ^ Current iterator position.
          *         ^ New inserted element.
          * @endcode
          *
          * This meaning that the iterator will not iterate over the new inserted element.
          */
        void insertBefore(const T &t);

        /**
          * Inserts element @p t after the current iterator position.
          *
          * @code
          * [ a b c d x e ... ]
          *           ^ New inserted element. Next iterator position in case next() is feasible.
          *         ^ Current iterator position.
          * @endcode
          */
        void insertAfter(const T &t);

        /**
          * Removes the element at the current iterator position.
          */
        void remove();

        /**
          * Rewinds the iterator position to the initial position.
          */
        void rewind();

        /**
          * Rewinds the iterator position to @p i position.
          *
          * @note This also sets the initial position to @p i, so calling to rewind() later will
          *       rewind the iterator to position @p i.
          */
        void rewind(size_t i);

    private:
        Vector<T> &m_vector;
        size_t     m_i;
        size_t     m_initialPos;
    };

////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
      * @class ConstIterator
      *
      * This class implements an iterator able only to retrieve information, without any altering
      * capabilities.
      *
      * This is the preferred way to iterate over a vector in the case that you only want to
      * retrieve information.
      *
      * It will never perform a deep copy of the vector being iterated.
      *
      * A typical example follows:
      *
      * @code
      * IdealCore::Vector<MyClass>::ConstIterator it(myVector);
      * while (it.hasNext()) {
      *     const MyClass &c = it.next();
      *     // Do whatever with c
      * }
      * @endcode
      *
      * @author Rafael Fernández López <ereslibre@ereslibre.es>
      */
    class ConstIterator
    {
    public:
        /**
          * Constructs a const iterator for @p vector starting at @p initialPos.
          */
        ConstIterator(const Vector<T> &vector, size_t initialPos = 0);
        virtual ~ConstIterator();

        /**
          * @return Whether we can call to @p next for continuing data fetching.
          */
        bool hasNext() const;

        /**
          * @return The element at the current iterator position.
          */
        const T &next();

        /**
          * Rewinds the iterator position to the initial position.
          */
        void rewind();

        /**
          * Rewinds the iterator position to @p i position.
          *
          * @note This also sets the initial position to @p i, so calling to rewind() later will
          *       rewind the iterator to position @p i.
          */
        void rewind(size_t i);

    private:
        const Vector<T> &m_vector;
        size_t           m_i;
        size_t           m_initialPos;
    };

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
        memset(&d->m_vector[d->m_size + 1], '\0', (d->m_containerSize - (d->m_size + 1)) * sizeof(T));
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
        memset(&d->m_vector[d->m_size + 1], '\0', (d->m_containerSize - (d->m_size + 1)) * sizeof(T));
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
        memset(&d->m_vector[d->m_size + 1], '\0', (d->m_containerSize - (d->m_size + 1)) * sizeof(T));
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
        IDEAL_DEBUG_WARNING("index out of range");
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
        IDEAL_DEBUG_WARNING("index out of range");
        d->m_emptyRes = T();
        return d->m_emptyRes;
    }
    return d->m_vector[i];
}

template <typename T>
Vector<T> &Vector<T>::operator<<(const T &t)
{
    append(t);
    return *this;
}

template <typename T>
bool Vector<T>::operator==(const Vector<T> &vector) const
{
    if (d == vector.d) {
        return true;
    }
    if (d->m_size != vector.d->m_size) {
        return false;
    }
    for (size_t i = 0; i < d->m_size; ++i) {
        if (!(d->m_vector[i] == vector.d->m_vector[i])) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool Vector<T>::operator!=(const Vector<T> &vector) const
{
    return !(*this == vector);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector<T>::Iterator::Iterator(Vector<T> &vector, size_t initialPos)
    : m_vector(vector)
    , m_i(initialPos)
    , m_initialPos(initialPos)
{
}

template <typename T>
Vector<T>::Iterator::~Iterator()
{
}

template <typename T>
bool Vector<T>::Iterator::hasNext() const
{
    return m_i < m_vector.size();
}

template <typename T>
T &Vector<T>::Iterator::next()
{
    return m_vector[m_i++];
}

template <typename T>
void Vector<T>::Iterator::insertBefore(const T &t)
{
    m_vector.insertAt(t, m_i - 1);
}

template <typename T>
void Vector<T>::Iterator::insertAfter(const T &t)
{
    m_vector.insertAt(t, m_i);
}

template <typename T>
void Vector<T>::Iterator::remove()
{
    m_vector.removeAt(--m_i);
}

template <typename T>
void Vector<T>::Iterator::rewind()
{
    m_i = m_initialPos;
}

template <typename T>
void Vector<T>::Iterator::rewind(size_t i)
{
    m_i = i;
    m_initialPos = i;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Vector<T>::ConstIterator::ConstIterator(const Vector<T> &vector, size_t initialPos)
    : m_vector(vector)
    , m_i(initialPos)
    , m_initialPos(initialPos)
{
}

template <typename T>
Vector<T>::ConstIterator::~ConstIterator()
{
}

template <typename T>
bool Vector<T>::ConstIterator::hasNext() const
{
    return m_i < m_vector.size();
}

template <typename T>
const T &Vector<T>::ConstIterator::next()
{
    return m_vector[m_i++];
}

template <typename T>
void Vector<T>::ConstIterator::rewind()
{
    m_i = m_initialPos;
}

template <typename T>
void Vector<T>::ConstIterator::rewind(size_t i)
{
    m_i = i;
    m_initialPos = i;
}

}

#endif //IDEAL_VECTOR_H
