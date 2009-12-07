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

#ifndef OBJECT_H
#define OBJECT_H

#include <ideal_export.h>
#include <core/ideal_signal.h>

namespace IdealCore {

class Application;

/**
  * @class Object object.h core/object.h
  *
  * The base class for Ideal library usage. Inheriting this class will allow you to use signals
  * and connect them to methods in any class that inherits IdealCore::Object. It contains the basic
  * functionality offered by the Ideal Library.
  *
  * You can check how to work with signals with several examples:
  *     - @ref workingWithSignals
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
      * Sets whether emit() is blocked for this object.
      *
      * @note destroyed signal will always be emitted, even if emit() is blocked for this object.
      */
    void setEmitBlocked(bool emitBlocked);

    /**
      * @return Whether emit() is blocked for this object.
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
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connect(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        signal.connect(receiver, member);
    }

    /**
      * Connects @p signal to the synchronized method @p member on @p receiver.
      *
      * When @p signal is emitted, before calling to @p member on @p receiver, the mutex @p mutex
      * will be locked. When this call has finished, the mutex @p mutex will become unlocked.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connectSynchronized(const Signal<Param...> &signal, Receiver *receiver, Member member, Mutex &mutex)
    {
        signal.connectSynchronized(receiver, member, mutex);
    }

    /**
      * Connects @p signal to the multi method @p member on @p receiver.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connectMulti(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        signal.connectMulti(receiver, member);
    }

    /**
      * Connects @p signal to the multi synchronized method @p member on @p receiver.
      *
      * When @p signal is emitted, before calling to @p member on @p receiver, the mutex @p mutex
      * will be locked. When this call has finished, the mutex @p mutex will become unlocked.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void connectMultiSynchronized(const Signal<Param...> &signal, Receiver *receiver, Member member, Mutex &mutex)
    {
        signal.connectMultiSynchronized(receiver, member, mutex);
    }

    /**
      * Forwards @p signal to another signal @p receiver.
      *
      * See @ref workingWithSignals
      */
    template <typename... Param>
    static inline void connect(const Signal<Param...> &signal, const Signal<Param...> &receiver)
    {
        signal.connect(receiver);
    }

    /**
      * Connects @p signal to the static method or function @p member.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void connectStatic(const Signal<Param...> &signal, Member member)
    {
        signal.connectStatic(member);
    }

    /**
      * Connects @p signal to the static synchronized method or function @p member.
      *
      * When @p signal is emitted, before calling to @p member, the mutex @p mutex will be locked.
      * When this call has finished, the mutex @p mutex will become unlocked.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void connectStaticSynchronized(const Signal<Param...> &signal, Member member, Mutex &mutex)
    {
        signal.connectStaticSynchronized(member, mutex);
    }

    /**
      * Connects @p signal to the static multi method or function @p member.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void connectStaticMulti(const Signal<Param...> &signal, Member member)
    {
        signal.connectStaticMulti(member);
    }

    /**
      * Connects @p signal to the static multi synchronized method or function @p member.
      *
      * When @p signal is emitted, before calling to @p member, the mutex @p mutex will be locked.
      * When this call has finished, the mutex @p mutex will become unlocked.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void connectStaticMultiSynchronized(const Signal<Param...> &signal, Member member, Mutex &mutex)
    {
        signal.connectStaticMultiSynchronized(member, mutex);
    }

    /**
      * Disconnects @p signal from @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        signal.disconnect(receiver, member);
    }

    /**
      * Disconnects @p signal from synchronized @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnectSynchronized(const Signal<Param...> &signal, Receiver *receiver, Member member, Mutex &mutex)
    {
        signal.disconnectSynchronized(receiver, member, mutex);
    }

    /**
      * Disconnects @p signal from multi @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnectMulti(const Signal<Param...> &signal, Receiver *receiver, Member member)
    {
        signal.disconnectMulti(receiver, member);
    }

    /**
      * Disconnects @p signal from multi synchronized @p member on @p receiver.
      *
      * @note It is possible to connect the same signal to the same member and
      *       receiver more than one time. This will disconnect only the first
      *       match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename Member, typename... Param>
    static inline void disconnectMultiSynchronized(const Signal<Param...> &signal, Receiver *receiver, Member member, Mutex &mutex)
    {
        signal.disconnectMultiSynchronized(receiver, member, mutex);
    }

    /**
      * Removes the signal forwarding of @p signal to @p receiver.
      *
      * @note It is possible to forward the same signal to the same signal
      *       more than one time. This will disconnect only the first match,
      *       not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, const Signal<Param...> &receiver)
    {
        signal.disconnect(receiver);
    }

    /**
      * Disconnects @p signal from static method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void disconnectStatic(const Signal<Param...> &signal, Member member)
    {
        signal.disconnectStatic(member);
    }

    /**
      * Disconnects @p signal from static synchronized method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void disconnectStaticSynchronized(const Signal<Param...> &signal, Member member, Mutex &mutex)
    {
        signal.disconnectStaticSynchronized(member, mutex);
    }

    /**
      * Disconnects @p signal from multi static method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void disconnectStaticMulti(const Signal<Param...> &signal, Member member)
    {
        signal.disconnectStaticMulti(member);
    }

    /**
      * Disconnects @p signal from multi static synchronized method or function @p member.
      *
      * @note It is possible to connect the same signal to the same static
      *       method or function more than one time. This will disconnect
      *       only the first match, not all of them.
      *
      * See @ref workingWithSignals
      */
    template <typename Member, typename... Param>
    static inline void disconnectStaticMultiSynchronized(const Signal<Param...> &signal, Member member, Mutex &mutex)
    {
        signal.disconnectStaticMultiSynchronized(member, mutex);
    }

    /**
      * Disconnects @p signal completely from @p receiver.
      *
      * See @ref workingWithSignals
      */
    template <typename Receiver, typename... Param>
    static inline void disconnect(const Signal<Param...> &signal, Receiver *receiver)
    {
        signal.disconnect(receiver);
    }

    /**
      * All signals in @p sender will become disconnected from their receivers.
      *
      * @note This cannot be undone. If you want to temporarily fake this effect, use setEmitBlocked.
      *
      * See @ref workingWithSignals
      */
    static void disconnectSender(Object *sender);

    /**
      * All signals connected to @p receiver will be disconnected from it.
      *
      * @note If signals were connected to other receivers, those are still connected.
      *
      * @note This cannot be undone. If you want to temporarily fake this effect, use setBlockedSignals.
      *
      * See @ref workingWithSignals
      */
    static void disconnectReceiver(Object *receiver);

    /**
      * Equivalent to disconnectSender() and disconnectReceiver() on @p object.
      *
      * See @ref workingWithSignals
      */
    static void fullyDisconnect(Object *object);

    /**
      * Emits @p signal with parameters @p param.
      *
      * See @ref workingWithSignals
      */
    template <typename... Param>
    static void inline emit(const Signal<Param...> &signal, const Param&... param)
    {
        signal.emit(param...);
    }

    /**
      * Deletes this object right now.
      */
    void deleteNow();

    /**
      * Deletes this object on the next event loop.
      */
    void deleteLater();

protected:
    /**
      * @internal
      */
    virtual void signalCreated(const SignalBase *signal);

    /**
      * @internal
      */
    virtual void signalConnected(const SignalBase *signal);

    /**
      * @internal
      */
    virtual void signalDisconnected(const SignalBase *signal);

    /**
      * @internal
      */
    virtual List<const SignalBase*> signals() const;

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
