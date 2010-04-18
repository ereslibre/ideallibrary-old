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
    T &peek();
    const T &peek() const;

    size_t size() const;
    bool empty() const;

    void clear();

    Stack &operator=(const Stack &stack);

private:
    class Private;
    Private *d;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class Stack<T>::Private
{
public:
    Private();
    virtual ~Private();

    Private *copy() const;
    void copyAndDetach(Stack<T> *stack);
    void newAndDetach(Stack<T> *stack);
    void clearContents();

    void ref();
    void deref();

    static Private *empty();

    T     *m_stack;
    size_t m_top;
    size_t m_capacity;
    size_t m_refs;

    static Private *m_privateEmpty;
    static T        m_emptyRes;
};

template <typename T>
typename Stack<T>::Private *Stack<T>::Private::m_privateEmpty = 0;

template <typename T>
T Stack<T>::Private::m_emptyRes = T();

////////////////////////////////////////////////////////////////////////////////////////////////////

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
typename Stack<T>::Private *Stack<T>::Private::copy() const
{
    Private *privateCopy = new Private;
    privateCopy->m_stack = new T[m_capacity];
    for (size_t i = 0; i < m_capacity; ++i) {
        privateCopy->m_stack[i] = m_stack[i];
    }
    privateCopy->m_top = m_top;
    privateCopy->m_capacity = m_capacity;
    return privateCopy;
}

template <typename T>
void Stack<T>::Private::copyAndDetach(Stack<T> *stack)
{
    if (m_refs > 1) {
        stack->d = copy();
        deref();
    } else if (this == m_privateEmpty) {
        m_privateEmpty = 0;
    }
}

template <typename T>
void Stack<T>::Private::newAndDetach(Stack<T> *stack)
{
    if (m_refs > 1) {
        stack->d = new Private;
        deref();
    } else if (this == m_privateEmpty) {
        m_privateEmpty = 0;
    } else {
        clearContents();
    }
}

template <typename T>
void Stack<T>::Private::clearContents()
{
    delete[] m_stack;
    m_stack = 0;
    m_top = 0;
    m_capacity = 0;
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
        if (this == m_privateEmpty) {
            m_privateEmpty = 0;
        }
        delete this;
    }
}

template <typename T>
typename Stack<T>::Private *Stack<T>::Private::empty()
{
    if (!m_privateEmpty) {
        m_privateEmpty = new Private;
    } else {
        m_privateEmpty->ref();
    }
    return m_privateEmpty;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Stack<T>::Stack()
    : d(Private::empty())
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
    d->copyAndDetach(this);
    if (d->m_top == d->m_capacity) {
        if (!d->m_capacity) {
            d->m_capacity = 2;
            d->m_stack = new T[2];
        } else {
            const size_t oldCapacity = d->m_capacity;
            d->m_capacity = oldCapacity * 2;
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
const T &Stack<T>::pop()
{
    if (d->m_top) {
        d->copyAndDetach(this);
        return d->m_stack[--d->m_top];
    }
    d->m_emptyRes = T();
    return d->m_emptyRes;
}

template <typename T>
T &Stack<T>::peek()
{
    if (d->m_top) {
        d->copyAndDetach(this);
        return d->m_stack[d->m_top - 1];
    }
    d->m_emptyRes = T();
    return d->m_emptyRes;
}

template <typename T>
const T &Stack<T>::peek() const
{
    if (d->m_top) {
        return d->m_stack[d->m_top - 1];
    }
    d->m_emptyRes = T();
    return d->m_emptyRes;
}

template <typename T>
size_t Stack<T>::size() const
{
    return d->m_top;
}

template <typename T>
bool Stack<T>::empty() const
{
    return d->m_top == 0;
}

template <typename T>
void Stack<T>::clear()
{
    if (d == Private::m_privateEmpty) {
        return;
    }
    d->deref();
    d = Private::empty();
}

template <typename T>
Stack<T> &Stack<T>::operator=(const Stack<T> &stack)
{
    if (d == stack.d) {
        return *this;
    }
    d->deref();
    stack.d->ref();
    d = stack.d;
    return *this;
}

}

#endif //STACK_H
