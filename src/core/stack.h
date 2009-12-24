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

#ifndef STACK_H
#define STACK_H

namespace IdealCore {

/**
  * @class Stack stack.h core/stack.h
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
template <typename T>
class Stack
{
public:
    Stack();
    Stack(const Stack &stack);
    virtual ~Stack();

    void push(const T &t);
    const T &pop();

    Stack &operator=(const Stack &stack);

    bool operator==(const Stack &stack) const;
    bool operator!=(const Stack &stack) const;

private:
    class Private;
    Private *d;
};

template <typename T>
class Stack<T>::Private
{
public:
    Private();
    virtual ~Private();

    void ref();
    void deref();

    T     *m_stack;
    size_t m_refs;
};

template <typename T>
Stack<T>::Private::Private()
    : m_stack(0)
    , m_refs(1)
{
}

template <typename T>
Stack<T>::Private::~Private()
{
    free(m_stack);
}

template <typename T>
void Stack<T>::Private::ref()
{
    ++m_refs;
}

template <typename T>
void Stack<T>::Private::deref()
{
    --m_refs;
    if (!m_refs) {
        delete this;
    }
}

template <typename T>
Stack<T>::Stack()
{
}

template <typename T>
Stack<T>::Stack(const Stack<T> &stack)
{
}

template <typename T>
Stack<T>::~Stack()
{
    d->deref();
}

template <typename T>
void Stack<T>::push(const T &t)
{
}

template <typename T>
const T &Stack<T>::pop()
{
}

template <typename T>
Stack<T> &Stack<T>::operator=(const Stack<T> &stack)
{
    return *this;
}

template <typename T>
bool Stack<T>::operator==(const Stack<T> &stack) const
{
    return false;
}

template <typename T>
bool Stack<T>::operator!=(const Stack<T> &stack) const
{
    return true;
}

}

#endif //STACK_H
