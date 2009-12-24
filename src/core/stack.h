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
    const T pop();

    size_t size() const;

    void clear();

    Stack &operator=(const Stack &stack);

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
    size_t m_top;
    size_t m_capacity;
    size_t m_refs;
};

template <typename T>
Stack<T>::Private::Private()
    : m_stack(0)
    , m_top(0)
    , m_capacity(0)
    , m_refs(1)
{
}

template <typename T>
Stack<T>::Private::~Private()
{
    delete[] m_stack;
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
    : d(new Private)
{
}

template <typename T>
Stack<T>::Stack(const Stack<T> &stack)
{
    stack.d->ref();
    d = stack.d;
}

template <typename T>
Stack<T>::~Stack()
{
    d->deref();
}

template <typename T>
void Stack<T>::push(const T &t)
{
    if (d->m_top == d->m_capacity) {
        if (!d->m_capacity) {
            d->m_capacity = 2;
            d->m_stack = new T[2];
        } else {
            const size_t oldCapacity = d->m_capacity;
            d->m_capacity = (oldCapacity * 2);
            T *const oldStack = d->m_stack;
            d->m_stack = new T[d->m_capacity];
            for (size_t i = 0; i < oldCapacity; ++i) {
                d->m_stack[i] = oldStack[i];
            }
            delete[] oldStack;
        }
    }
    d->m_stack[d->m_top] = t;
    ++d->m_top;
}

template <typename T>
const T Stack<T>::pop()
{
    if (d->m_top) {
        return d->m_stack[--d->m_top];
    }
    return T();
}

template <typename T>
size_t Stack<T>::size() const
{
    return d->m_top;
}

template <typename T>
void Stack<T>::clear()
{
    delete[] d->m_stack;
    d->m_top = 0;
    d->m_capacity = 0;
}

template <typename T>
Stack<T> &Stack<T>::operator=(const Stack<T> &stack)
{
    if (this == &stack || d == stack.d) {
        return *this;
    }
    d->deref();
    stack.d->ref();
    d = stack.d;
    return *this;
}

}

#endif //STACK_H
