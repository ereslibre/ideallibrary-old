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

#ifndef OBJECT_H
#define OBJECT_H

/*!
 * @mainpage Ideal Library
 *
 * The Ideal Library is a free, intuitive, easy-to-use and consistent library meant to easily
 * develop applications.
 *
 * The Ideal Library will allow you to declare signals, which can be later connected to any
 * other method. It is important to note that you can only declare signals in classes that
 * inherit IdealCore::Object. The same goes for methods that are called by signals. A method
 * that can be called by a signal has no special treatment. It is a regular method.
 *
 * Declaring a signal on Ideal Library is very easy:
 *
 * @code
 * class MyClass
 *     : public IdealCore::Object
 * {
 * public:
 *     MyClass(IdealCore::Object *parent);
 *     ~MyClass();
 *
 *     IDEAL_SIGNAL(finished); // a signal called 'finished'
 * }
 * @endcode
 *
 * The second step is to initialize it on the definition of the constructor(s):
 *
 * @code
 * MyClass::MyClass(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(finished)
 * {
 * }
 * @endcode
 *
 * Declaring a signal with one parameter, for example, is not harder:
 *
 * @code
 * class MyClass
 *     : public IdealCore::Object
 * {
 * public:
 *     MyClass(IdealCore::Object *parent);
 *     ~MyClass();
 *
 *     IDEAL_SIGNAL(finished, int);
 * }
 * @endcode
 *
 * @code
 * MyClass::MyClass(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(finished, int)
 * {
 * }
 * @endcode
 *
 * You can have an indetermined number of parameters, and of course they can be complex:
 *
 * @code
 * class MyClass
 *     : public IdealCore::Object
 * {
 * public:
 *     MyClass(IdealCore::Object *parent);
 *     ~MyClass();
 *
 *     IDEAL_SIGNAL(finished, int, IdealCore::Object*, MyClass*, IdealCore::String);
 * }
 * @endcode
 *
 * @code
 * MyClass::MyClass(Object *parent)
 *     : Object(parent)
 *     , IDEAL_SIGNAL_INIT(finished, int, Object*, MyClass*, String)
 * {
 * }
 * @endcode
 */

#include <ideal_export.h>
#include <core/ideal_signal.h>
#include <core/signal_resource.h>

/**
  * The Ideal core namespace.
  */
namespace IdealCore {

class Application;

/**
  * The base class for Ideal library usage. Inheriting this class will allow you to use signals
  * and connect them to methods in any class that inherits IdealCore::Object.
  *
  * Examples of declaring signals are:
  *
  * @code
  * class MyClass
  *     : public IdealCore::Object
  * {
  * public:
  *     MyClass(IdealCore::Object *parent);
  *     ~MyClass();
  *
  *     void randomMethod(); // does something
  *
  *     IDEAL_SIGNAL(finished); // notifies that something has finished
  * ];
  * @endcode
  *
  * Then in the CPP when defining the methods... don't forget the constructor as follows:
  *
  * @code
  * MyClass::MyClass(Object *parent)
  *     : Object(parent)
  *     , IDEAL_SIGNAL_INIT(finished)
  * {
  * }
  * @endcode
  *
  * Following this example, suppose randomMethod() method emits finished, it would do the following:
  *
  * @code
  * void MyClass::randomMethod()
  * {
  *     // some operations here
  *     emit(finished);
  * }
  * @endcode
  *
  * An example of a signal taking two arguments:
  *
  * @code
  * class MyClass
  *     : public IdealCore::Object
  * {
  * public:
  *     MyClass(IdealCore::Object *parent);
  *     ~MyClass();
  *
  *     void randomMethod();
  *
  *     IDEAL_SIGNAL(finished, int, IdealCore::Object*);
  *
  * private:
  *     IdealCore::Object *otherObject;
  * ];
  * @endcode
  *
  * Then in the CPP when defining the methods don't forget the constructor as follows:
  *
  * @code
  * MyClass::MyClass(Object *parent)
  *     : Object(parent)
  *     , IDEAL_SIGNAL_INIT(finished, int, Object*)
  * {
  *     // init here otherObject for example
  * }
  * @endcode
  *
  * Following this example, suppose randomMethod() method emits finished, it would do the following:
  *
  * @code
  * void MyClass::randomMethod()
  * {
  *     int value = 0;
  *     // some operations here and value updated
  *
  *     emit(finished, value, otherObject);
  * }
  * @endcode
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Object
    : public SignalResource
{
    // This friend classes are allowed to use the default constructor
    friend class Application;
    friend class Extension;
    friend class Module;
    friend class Timer;

public:
    Object(Object *parent);
    virtual ~Object();

    /**
      * Sets whether children of this object should be deleted recursively when this object is.
      */
    void setDeleteChildrenRecursively(bool deleteChildrenRecursively);

    /**
      * @return Whether this object children will be removed when this object is.
      *
      * @note true by default.
      */
    bool isDeleteChildrenRecursively() const;

    /**
      * Sets whether signals are blocked for this object or not. If @p blockedSignals is true, this
      * object won't receive any call coming from a signal.
      */
    void setBlockedSignals(bool blockedSignals);

    /**
      * @return Whether signals are blocked for this object or not.
      *
      * @note false by default.
      */
    bool areSignalsBlocked() const;

    /**
      * Sets whether this object can emit signals.
      *
      * @note destroyed signal will be emitted even if emit is blocked for that object.
      */
    void setEmitBlocked(bool emitBlocked);

    /**
      * @return Whether this object can emit signals.
      *
      * @note false by default.
      */
    bool isEmitBlocked() const;

    /**
      * @return The list of children of this object.
      */
    List<Object*> children() const;

    /**
      * @return The parent of this object.
      */
    Object *parent() const;

    /**
      * Reparents this object to @p parent.
      */
    void reparent(Object *parent);

    /**
      * @return The application this object belongs to.
      */
    Application *application() const;

    /**
      * Connects @p signal to the method @p member on @p receiver.
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connect(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connect(receiver, member);
    }

    /**
      * Connects @p signal to the multi method @p member on @p receiver.
      *
      * @note The first parameter of the slot is the sender of the signal.
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connectMulti(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connectMulti(receiver, member);
    }

    /**
      * Forwards @p signal to another signal @p receiver.
      *
      * @note @p signal and @p receiver should have the same signature.
      */
    template <typename... Param>
    static inline void connect(const Signal<Param...> &signal, const Signal<Param...> &receiver)
    {
        const_cast<Signal<Param...>*>(&signal)->connect(receiver);
    }

    /**
      * Connects @p signal to the static method or function @p member.
      */
    template <typename Member, typename... Param>
    static inline void connectStatic(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connectStatic(member);
    }

    /**
      * Connects @p signal to the static multi method or function @p member. Example:
      *
      * @code
      * #include <core/object.h>
      * #include <core/application.h>
      *
      * using namespace IdealCore;
      *
      * static void myStaticMultiSlot(Object *sender)
      * {
      *     IDEAL_DEBUG("I was triggered by " << sender);
      * }
      *
      * class Test
      *     : public Object
      * {
      * public:
      *     IDEAL_SIGNAL(test);
      *
      *     Test(Object *parent)
      *         : Object(parent)
      *         , IDEAL_SIGNAL_INIT(test)
      *     {
      *     }
      *
      *     void emitTest() {
      *         emit(test);
      *     }
      * };
      *
      * int main(int argc, char **argv)
      * {
      *     Application app(argc, argv);
      *     Test testObject(&app);
      *     Object::connectStaticMulti(testObject.test, myStaticMultiSlot);
      *     testObject.emitTest();
      *
      *     return 0;
      * }
      * @endcode
      *
      * @note The first parameter of the slot is the sender of the signal.
      */
    template <typename Member, typename... Param>
    static inline void connectStaticMulti(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->connectStaticMulti(member);
    }

    /**
      * Disconnects @p signal from @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnect(receiver, member);
    }

    /**
      * Disconnects @p signal from multi @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnectMulti(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnectMulti(receiver, member);
    }

    /**
      * Removes the signal forwarding of @p signal to @p receiver.
      *
      * @note It is possible to forward the same signal to the same signal
      *       more than one time. This will disconnect only the first match,
      *       not all of them.
      */
    template <typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, const Signal<Param...> &receiver)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnect(receiver);
    }

    /**
      * Disconnects @p signal from static method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      */
    template <typename Member, typename... Param>
    static inline void disconnectStatic(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnectStatic(member);
    }

    /**
      * Disconnects @p signal from multi static method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      */
    template <typename Member, typename... Param>
    static inline void disconnectStaticMulti(const Signal<Param...> &signal, Member member)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnectStaticMulti(member);
    }

    /**
      * Disconnects @p signal completely from @p receiver.
      *
      * @note This will effectively disconnect absolutely all methods connected
      *       to this signal on @p receiver.
      */
    template <typename Receiver, typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, Receiver *receiver)
    {
        const_cast<Signal<Param...>*>(&signal)->disconnect(receiver);
    }

    /**
      * All signals in @p sender will become disconnected from their receivers.
      *
      * @note This cannot be undone. If you want to temporarily fake this effect, use setEmitBlocked.
      */
    static void disconnectSender(Object *sender);

    /**
      * All signals connected to @p receiver will be disconnected from it.
      *
      * @note If signals were connected to other receivers, those are still connected.
      *
      * @note This cannot be undone. If you want to temporarily fake this effect, use setBlockedSignals.
      */
    static void disconnectReceiver(Object *receiver);

    /**
      * Equivalent to disconnectSender() and disconnectReceiver() on @p object.
      */
    static void fullyDisconnect(Object *object);

    /**
      * Deletes this object right now.
      */
    void deleteNow();

    /**
      * Deletes this object on the next event loop.
      */
    void deleteLater();

    /**
      * @internal
      * @note Defined for binary compatibility reasons.
      */
    virtual void *virtual_hook(int id, void *param);

protected:
    /**
      * Emits @p signal with parameters @p param.
      */
    template <typename... Param>
    void inline emit(const Signal<Param...> &signal, const Param&... param) const
    {
        if (signal.parent() == this && (((void*) &signal) == ((void*) &destroyed) || !isEmitBlocked())) {
            const_cast<Signal<Param...>*>(&signal)->emit(param...);
        }
    }

    /**
      * @internal
      */
    virtual void signalCreated(SignalBase *signal);

    /**
      * @internal
      */
    virtual void signalConnected(SignalBase *signal);

    /**
      * @internal
      */
    virtual void signalDisconnected(SignalBase *signal);

    /**
      * @internal
      */
    virtual List<SignalBase*> signals() const;

private:
    /**
      * @internal
      */
    Object();

    class Private;
    Private *const d;

public:
    IDEAL_SIGNAL(destroyed);
};

}

#endif //OBJECT_H
