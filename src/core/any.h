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

#ifndef ANY_H
#define ANY_H

#include <ideal_export.h>
#include <core/ideal_string.h>

#include <typeinfo>

namespace IdealCore {

/**
  * @class Any any.h core/any.h
  *
  * This class can hold a instance of any type. It also provides functionality to find out
  * the type of a instance in runtime.
  *
  * Its usage is very simple:
  *
  * @code
  * String myString("Testing any");
  * Any any(myString);
  * IDEAL_SDEBUG("myString contains: " << any.get<String>());
  * @endcode
  *
  * It can also be reutilized, like in the following example:
  *
  * @code
  * String myString("Testing any");
  * iint32 myInt = 100;
  * float myFloat = 100.0;
  * Any any(myString);
  * any = myInt;
  * any = myFloat;
  * @endcode
  *
  * It can also work out of the box with user defined types:
  *
  * @code
  * struct MyStruct
  * {
  *     bool operator==(const MyStruct &myStruct) const
  *     {
  *         return a == myStruct.a && b == myStruct.b && c == myStruct.c;
  *     }
  *
  *     iint32 a;
  *     float b;
  *     String c;
  * };
  * MyStruct myStruct;
  * iint32 myInt = 100;
  * float myFloat = 100.0;
  * Any any(myStruct);
  * any = myInt;
  * any = myFloat;
  * @endcode
  *
  * @note the types being encapsulated in the Any instance need to implement operator==, since
  *       it is possible to directly compare Any objects, being called this operator in case that
  *       types match.
  *
  * @code
  * MyString myString("This is a test");
  * MyString otherString("This is a test");
  * iint32 myInt = 100;
  * Any myAny(myString);
  * Any otherAny(otherString);
  * // will print "yes"
  * IDEAL_SDEBUG("Are myString and otherString equal? " << (myAny == otherAny ? "yes" : "no"));
  * otherAny = myInt;
  * // will print "no", types don't even match
  * IDEAL_SDEBUG("Are myString and myInt equal? " << (myAny == otherAny ? "yes": "no"));
  * myAny = myInt;
  * // will print "yes"
  * IDEAL_SDEBUG("Are myInt and myInt equal? " << (myAny == otherAny ? "yes" : "no"));
  * @endcode
  *
  * @note constructing Any instances from other Any instance, or assignment between them is a special
  *       case. Their content will be the same, an Any instance will not encapsulate another Any
  *       instance.
  *
  * @code
  * iint32 myInt = 100;
  * Any myAny(myInt);
  * Any otherAny(myAny);
  * // will print "yes", since otherAny does not encapsulate myAny, it just points to myInt too
  * IDEAL_SDEBUG("Are myAny and otherAny equal? " << (myAny == otherAny ? "yes" : "no"));
  * @endcode
  *
  * Same happens with the assignment:
  *
  * @code
  * iint32 myInt = 100;
  * Any myAny(myInt);
  * Any otherAny;
  * otherAny = myAny;
  * // will print "yes", since otherAny does not encapsulate myAny, it just points to myInt too
  * IDEAL_SDEBUG("Are myAny and otherAny equal? " << (myAny == otherAny ? "yes" : "no"));
  * @endcode
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Any
{
public:
    /**
      * Constructs an empty Any instance.
      *
      * @note since it has not morphed yet, type() will return information for Any class, and
      *       a call to typeName() will return IdealCore::Any, until it is reassigned and it
      *       morphs.
      */
    Any();

    /**
      * Constructs an Any instance of type T.
      */
    template <typename T>
    Any(const T &t);
    Any(const Any &any);
    virtual ~Any();

    /**
      * @return Whether this Any instance is currently empty or not. This is equivalent to asking if
      *         this instance == Any().
      */

    bool isEmpty() const;

    /**
      * @return The contents of this Any instance with the desired type T. It must match the
      *         type of the encapsulated instance.
      */
    template <typename T>
    T get() const;

    /**
      * @return The type name of the encapsulated type. IdealCore::Any if no encapsulated instance.
      */
    String typeName() const;

    /**
      * @return The type of the encapsulated instance.
      */
    const std::type_info &type() const;

    /**
      * @return The type name of type T.
      */
    template <typename T>
    static String typeName(const T &t);

    /**
      * @return The type as in std::type_info of type T. std::type_info has the ability to be
      *         compared.
      */
    template <typename T>
    static std::type_info &type(const T&t);

    /**
      * Makes this Any instance to morph to type T and encapsulate the @p t instance of type T.
      */
    template <typename T>
    Any &operator=(const T &t);
    Any &operator=(const Any &any);

    /**
      * @return true if this Any instance is of the same type and is equal to @p any using
      *         operator== on the encapsulated type. false otherwise.
      */
    bool operator==(const Any &any) const;
    bool operator!=(const Any &any) const;

private:
    class GenericStorage;
    template <typename T>
    class Storage;
    GenericStorage *m_s;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  *
  * DO NOT SPLIT DECLARATION/DEFINITION
  */
class Any::GenericStorage
{
public:
    GenericStorage()
        : m_refs(1)
    {
    }

    virtual ~GenericStorage()
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

    virtual const std::type_info &type() const = 0;
    virtual bool equals(const Any &any) const = 0;

private:
    size_t m_refs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename T>
class Any::Storage
    : public GenericStorage
{
public:
    Storage(const T &t);

    const std::type_info &type() const;

    bool equals(const Any &any) const;

public:
    T m_t;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Any::Storage<T>::Storage(const T &t)
    : m_t(t)
{
}

template <typename T>
const std::type_info &Any::Storage<T>::type() const
{
    return typeid(T);
}

template <typename T>
bool Any::Storage<T>::equals(const Any &any) const
{
    if (typeid(T) != any.m_s->type()) {
        return false;
    }
    return m_t == static_cast<Storage<T>*>(any.m_s)->m_t;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
Any::Any(const T &t)
    : m_s(new Storage<T>(t))
{
}

template <typename T>
T Any::get() const
{
    if (!m_s) {
        IDEAL_DEBUG_WARNING("get() on an empty Any class");
        return T();
    }
    if (m_s->type() != typeid(T)) {
        T t;
        Any any(t);
        IDEAL_DEBUG_WARNING("get() of an invalid type " << any.typeName() << " when contents "
                            "were of type " << typeName());
        return t;
    }
    return static_cast<Storage<T>*>(m_s)->m_t;
}

template <typename T>
String Any::typeName(const T &t)
{
    Any any(t);
    return any.typeName();
}

template <typename T>
std::type_info &Any::type(const T &t)
{
    return typeid(T);
}

template <typename T>
Any &Any::operator=(const T &t)
{
    if (m_s) {
        m_s->deref();
    }
    m_s = new Storage<T>(t);
    return *this;
}

}

#endif //ANY_H
