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

#ifndef IDEAL_SIGNAL_H
#define IDEAL_SIGNAL_H

#include <core/mutex.h>
#include <core/list.h>
#include <core/signal_resource.h>

namespace IdealCore {

class Object;
class SignalBase;

static List<SignalBase*> deletedSignalsOnEmit;
static Mutex deletedSignalsOnEmitMutex;

/**
  * @internal
  */
class CallbackDummy
{
public:
    virtual ~CallbackDummy()
    {
        m_receiver = 0;
    }

    SignalResource *m_receiver;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
class CallbackBase
    : public CallbackDummy
{
public:
    virtual ~CallbackBase()
    {
    }

    virtual void operator()(const Param&... param) = 0;

    template <typename Receiver, typename Member>
    static CallbackBase<Param...> *make(Receiver *receiver, Member member);

    template <typename Receiver, typename Member>
    static CallbackBase<Param...> *makeSynchronized(Receiver *receiver, Member member, Mutex &mutex);

    template <typename Receiver, typename Member>
    static CallbackBase<Param...> *makeMulti(SignalResource *resource, Receiver *receiver, Member member);

    template <typename Receiver, typename Member>
    static CallbackBase<Param...> *makeMultiSynchronized(SignalResource *resource, Receiver *receiver, Member member, Mutex &mutex);

    template <typename Member>
    static CallbackBase<Param...> *makeStatic(Member member);

    template <typename Member>
    static CallbackBase<Param...> *makeStaticSynchronized(Member member, Mutex &mutex);

    template <typename Member>
    static CallbackBase<Param...> *makeStaticMulti(SignalResource *resource, Member member);

    template <typename Member>
    static CallbackBase<Param...> *makeStaticMultiSynchronized(SignalResource *resource, Member member, Mutex &mutex);

    static CallbackBase<Param...> *makeForward(const SignalBase &signal);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Receiver, typename Member, typename... Param>
class Callback
    : public CallbackBase<Param...>
{
public:
    Callback(Receiver *receiver, Member member)
        : m_member(member)
    {
        this->m_receiver = receiver;
    }

    virtual void operator()(const Param&... param)
    {
        Receiver *receiver = 0;
        {
            ContextMutexLocker cml(this->m_receiver->m_mutex);
            if (static_cast<Receiver*>(this->m_receiver)->areSignalsBlocked()) {
                return;
            }
            receiver = static_cast<Receiver*>(this->m_receiver);
        }
        (receiver->*m_member)(param...);
    }

    Member m_member;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Receiver, typename Member, typename... Param>
class CallbackSynchronized
    : public Callback<Receiver, Member, Param...>
{
public:
    CallbackSynchronized(Receiver *receiver, Member member, Mutex &mutex)
        : Callback<Receiver, Member, Param...>(receiver, member)
        , m_mutex(mutex)
    {
    }

    virtual void operator()(const Param&... param)
    {
        ContextMutexLocker cml(m_mutex);
        Callback<Receiver, Member, Param...>::operator()(param...);
    }

    Mutex &m_mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Receiver, typename Member, typename... Param>
class CallbackMulti
    : public CallbackBase<Param...>
{
public:
    CallbackMulti(SignalResource *signalResource, Receiver *receiver, Member member)
        : m_member(member)
        , m_sender(reinterpret_cast<Object*>(signalResource))
    {
        this->m_receiver = receiver;
    }

    virtual void operator()(const Param&... param)
    {
        Receiver *receiver = 0;
        {
            ContextMutexLocker cml(this->m_receiver->m_mutex);
            if (static_cast<Receiver*>(this->m_receiver)->areSignalsBlocked()) {
                return;
            }
            receiver = static_cast<Receiver*>(this->m_receiver);
        }
        (receiver->*m_member)(m_sender, param...);
    }

    Member         m_member;
    Object * const m_sender;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Receiver, typename Member, typename... Param>
class CallbackMultiSynchronized
    : public CallbackMulti<Receiver, Member, Param...>
{
public:
    CallbackMultiSynchronized(SignalResource *signalResource, Receiver *receiver, Member member, Mutex &mutex)
        : CallbackMulti<Receiver, Member, Param...>(signalResource, receiver, member)
        , m_mutex(mutex)
    {
    }

    virtual void operator()(const Param&... param)
    {
        ContextMutexLocker cml(m_mutex);
        CallbackMulti<Receiver, Member, Param...>::operator()(param...);
    }

    Mutex &m_mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Member, typename... Param>
class CallbackStatic
    : public CallbackBase<Param...>
{
public:
    CallbackStatic(Member member)
        : m_member(member)
    {
        this->m_receiver = 0;
    }

    virtual void operator()(const Param&... param)
    {
        (*m_member)(param...);
    }

    Member m_member;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Member, typename... Param>
class CallbackStaticSynchronized
    : public CallbackStatic<Member, Param...>
{
public:
    CallbackStaticSynchronized(Member member, Mutex &mutex)
        : CallbackStatic<Member, Param...>(member)
        , m_mutex(mutex)
    {
    }

    virtual void operator()(const Param&... param)
    {
        ContextMutexLocker cml(m_mutex);
        CallbackStatic<Member, Param...>::operator()(param...);
    }

    Mutex &m_mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Member, typename... Param>
class CallbackStaticMulti
    : public CallbackBase<Param...>
{
public:
    CallbackStaticMulti(SignalResource *signalResource, Member member)
        : m_member(member)
        , m_sender(reinterpret_cast<Object*>(signalResource))
    {
        this->m_receiver = 0;
    }

    virtual void operator()(const Param&... param)
    {
        (*m_member)(m_sender, param...);
    }

    Member         m_member;
    Object * const m_sender;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename Member, typename... Param>
class CallbackStaticMultiSynchronized
    : public CallbackStaticMulti<Member, Param...>
{
public:
    CallbackStaticMultiSynchronized(SignalResource *signalResource, Member member, Mutex &mutex)
        : CallbackStaticMulti<Member, Param...>(signalResource, member)
        , m_mutex(mutex)
    {
    }

    virtual void operator()(const Param&... param)
    {
        ContextMutexLocker cml(m_mutex);
        CallbackStaticMulti<Member, Param...>::operator()(param...);
    }

    Mutex &m_mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Receiver, typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::make(Receiver *receiver, Member member)
{
    return new Callback<Receiver, Member, Param...>(receiver, member);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Receiver, typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::makeSynchronized(Receiver *receiver, Member member, Mutex &mutex)
{
    return new CallbackSynchronized<Receiver, Member, Param...>(receiver, member, mutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Receiver, typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::makeMulti(SignalResource *signalResource, Receiver *receiver, Member member)
{
    return new CallbackMulti<Receiver, Member, Param...>(signalResource, receiver, member);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Receiver, typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::makeMultiSynchronized(SignalResource *signalResource, Receiver *receiver, Member member, Mutex &mutex)
{
    return new CallbackMultiSynchronized<Receiver, Member, Param...>(signalResource, receiver, member, mutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::makeStatic(Member member)
{
    return new CallbackStatic<Member, Param...>(member);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::makeStaticSynchronized(Member member, Mutex &mutex)
{
    return new CallbackStaticSynchronized<Member, Param...>(member, mutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::makeStaticMulti(SignalResource *signalResource, Member member)
{
    return new CallbackStaticMulti<Member, Param...>(signalResource, member);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
template <typename Member>
CallbackBase<Param...> *CallbackBase<Param...>::makeStaticMultiSynchronized(SignalResource *signalResource, Member member, Mutex &mutex)
{
    return new CallbackStaticMultiSynchronized<Member, Param...>(signalResource, member, mutex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
class SignalBase
{
    friend class Object;

public:
    SignalBase(SignalResource *parent)
        : m_parent(parent)
        , m_isDestroyedSignal(true)
        , m_connectionsMutex(Mutex::Recursive)
        , m_beingEmitted(false)
        , m_beingEmittedMutex(Mutex::Recursive)
    {
        parent->signalCreated(this);
    }

    SignalBase(SignalResource *parent, const ichar */* name */, const ichar */* signature */)
        : m_parent(parent)
        , m_isDestroyedSignal(false)
        , m_connectionsMutex(Mutex::Recursive)
        , m_beingEmitted(false)
        , m_beingEmittedMutex(Mutex::Recursive)
    {
        parent->signalCreated(this);
    }

    virtual ~SignalBase()
    {
        ContextMutexLocker cml(m_beingEmittedMutex);
        if (m_beingEmitted) {
            ContextMutexLocker cml(deletedSignalsOnEmitMutex);
            deletedSignalsOnEmit.push_back(this);
        }
    }

    SignalResource *parent() const
    {
        return m_parent;
    }

    virtual void disconnect(SignalResource *receiver) const = 0;

    void disconnect() const
    {
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            delete *it;
        }
        m_connections.clear();
    }

protected:
    static void notifyReceiverConnection(SignalResource *receiver, const SignalBase *signalBase)
    {
        if (!signalBase->m_isDestroyedSignal) {
            ContextMutexLocker cml(receiver->m_mutex);
            receiver->signalConnected(signalBase);
        }
    }

    static void notifyReceiverDisconnection(SignalResource *receiver, const SignalBase *signalBase)
    {
        if (!signalBase->m_isDestroyedSignal) {
            ContextMutexLocker cml(receiver->m_mutex);
            receiver->signalDisconnected(signalBase);
        }
    }

    SignalResource       * const m_parent;
    const bool                   m_isDestroyedSignal;
    mutable List<CallbackDummy*> m_connections;
    mutable Mutex                m_connectionsMutex;
    mutable bool                 m_beingEmitted;
    mutable Mutex                m_beingEmittedMutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
class Signal
    : public SignalBase
{
    friend class Object;
    template <typename... Params>
    friend class SignalCallback;

public:
    Signal() __attribute__((error("missing call to IDEAL_SIGNAL_INIT on your class constructor")));

    Signal(SignalResource *parent, const ichar *name, const ichar *signature)
        : SignalBase(parent, name, signature)
    {
    }

    virtual ~Signal()
    {
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            delete *it;
        }
        m_connections.clear();
    }

    virtual void disconnect(SignalResource *receiver) const
    {
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end();) {
            const CallbackDummy *const curr = *it;
            if (curr->m_receiver == static_cast<void*>(receiver)) {
                notifyReceiverDisconnection(receiver, this);
                it = m_connections.erase(it);
                delete curr;
                continue;
            }
            ++it;
        }
    }

    template <typename Receiver, typename Member>
    void connect(Receiver *receiver, Member member) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("connection failed. NULL receiver");
            return;
        }
        notifyReceiverConnection(receiver, this);
        CallbackBase<Param...> *callback = CallbackBase<Param...>::make(receiver, member);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    template <typename Receiver, typename Member>
    void connectSynchronized(Receiver *receiver, Member member, Mutex &mutex) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("connection failed. NULL receiver");
            return;
        }
        notifyReceiverConnection(receiver, this);
        CallbackBase<Param...> *callback = CallbackBase<Param...>::makeSynchronized(receiver, member, mutex);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    template <typename Receiver, typename Member>
    void connectMulti(Receiver *receiver, Member member) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("connection failed. NULL receiver");
            return;
        }
        notifyReceiverConnection(receiver, this);
        CallbackBase<Param...> *callback = CallbackBase<Param...>::makeMulti(m_parent, receiver, member);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    template <typename Receiver, typename Member>
    void connectMultiSynchronized(Receiver *receiver, Member member, Mutex &mutex) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("connection failed. NULL receiver");
            return;
        }
        notifyReceiverConnection(receiver, this);
        CallbackBase<Param...> *callback = CallbackBase<Param...>::makeMultiSynchronized(m_parent, receiver, member, mutex);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    void connect(const Signal<Param...> &signal) const
    {
        notifyReceiverConnection(signal.parent(), this);
        CallbackBase<Param...> *signalForward = CallbackBase<Param...>::makeForward(signal);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(signalForward);
    }

    template <typename Member>
    void connectStatic(Member member) const
    {
        CallbackBase<Param...> *callback = CallbackBase<Param...>::makeStatic(member);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    template <typename Member>
    void connectStaticSynchronized(Member member, Mutex &mutex) const
    {
        CallbackBase<Param...> *callback = CallbackBase<Param...>::makeStaticSynchronized(member, mutex);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    template <typename Member>
    void connectStaticMulti(Member member) const
    {
        CallbackBase<Param...> *callback = CallbackBase<Param...>::makeStaticMulti(m_parent, member);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    template <typename Member>
    void connectStaticMultiSynchronized(Member member, Mutex &mutex) const
    {
        CallbackBase<Param...> *callback = CallbackBase<Param...>::makeStaticMultiSynchronized(m_parent, member, mutex);
        ContextMutexLocker cml(m_connectionsMutex);
        m_connections.push_back(callback);
    }

    template <typename Receiver, typename Member>
    void disconnect(Receiver *receiver, Member member) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("disconnection failed. NULL receiver");
            return;
        }
        notifyReceiverDisconnection(receiver, this);
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            Callback<Receiver, Member, Param...> *const curr = dynamic_cast<Callback<Receiver, Member, Param...>*>(*it);
            if (curr && curr->m_receiver == static_cast<void*>(receiver) && curr->m_member == member) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no slot disconnected. No previous connection found.");
    }

    template <typename Receiver, typename Member>
    void disconnectSynchronized(Receiver *receiver, Member member, Mutex &mutex) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("disconnection failed. NULL receiver");
            return;
        }
        notifyReceiverDisconnection(receiver, this);
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            CallbackSynchronized<Receiver, Member, Param...> *const curr = dynamic_cast<CallbackSynchronized<Receiver, Member, Param...>*>(*it);
            if (curr && curr->m_receiver == static_cast<void*>(receiver) && curr->m_member == member && curr->m_mutex == mutex) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no synchronized slot disconnected. No previous connection found.");
    }

    template <typename Receiver, typename Member>
    void disconnectMulti(Receiver *receiver, Member member) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("disconnection failed. NULL receiver");
            return;
        }
        notifyReceiverDisconnection(receiver, this);
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            CallbackMulti<Receiver, Member, Param...> *const curr = dynamic_cast<CallbackMulti<Receiver, Member, Param...>*>(*it);
            if (curr && curr->m_receiver == static_cast<void*>(receiver) && curr->m_member == member) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no multi slot disconnected. No previous connection found.");
    }

    template <typename Receiver, typename Member>
    void disconnectMultiSynchronized(Receiver *receiver, Member member, Mutex &mutex) const
    {
        if (!receiver) {
            IDEAL_DEBUG_WARNING("disconnection failed. NULL receiver");
            return;
        }
        notifyReceiverDisconnection(receiver, this);
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            CallbackMultiSynchronized<Receiver, Member, Param...> *const curr = dynamic_cast<CallbackMultiSynchronized<Receiver, Member, Param...>*>(*it);
            if (curr && curr->m_receiver == static_cast<void*>(receiver) && curr->m_member == member && curr->m_mutex == mutex) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no multi synchronized slot disconnected. No previous connection found.");
    }

    void disconnect(const Signal<Param...> &signal) const;

    template <typename Member>
    void disconnectStatic(Member member) const
    {
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            CallbackStatic<Member, Param...> *const curr = dynamic_cast<CallbackStatic<Member, Param...>*>(*it);
            if (curr && curr->m_member == member) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no static slot disconnected. No previous connection found.");
    }

    template <typename Member>
    void disconnectStaticSynchronized(Member member, Mutex &mutex) const
    {
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            CallbackStaticSynchronized<Member, Param...> *const curr = dynamic_cast<CallbackStaticSynchronized<Member, Param...>*>(*it);
            if (curr && curr->m_member == member && curr->m_mutex == mutex) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no static synchronized slot disconnected. No previous connection found.");
    }

    template <typename Member>
    void disconnectStaticMulti(Member member) const
    {
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            CallbackStaticMulti<Member, Param...> *const curr = dynamic_cast<CallbackStaticMulti<Member, Param...>*>(*it);
            if (curr && curr->m_member == member) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no static multi slot disconnected. No previous connection found.");
    }

    template <typename Member>
    void disconnectStaticMultiSynchronized(Member member, Mutex &mutex) const
    {
        List<CallbackDummy*>::iterator it;
        ContextMutexLocker cml(m_connectionsMutex);
        for (it = m_connections.begin(); it != m_connections.end(); ++it) {
            CallbackStaticMultiSynchronized<Member, Param...> *const curr = dynamic_cast<CallbackStaticMultiSynchronized<Member, Param...>*>(*it);
            if (curr && curr->m_member == member && curr->m_mutex == mutex) {
                m_connections.erase(it);
                delete curr;
                return;
            }
        }
        IDEAL_SDEBUG("no static multi synchronized slot disconnected. No previous connection found.");
    }

    void emit(const Param&... param) const
    {
        if (m_parent->isEmitBlocked() && !m_isDestroyedSignal) {
            return;
        }
        {
            ContextMutexLocker cml(m_beingEmittedMutex);
            m_beingEmitted = true;
        }
        List<CallbackDummy*> connections;
        {
            ContextMutexLocker cml(m_connectionsMutex);
            connections = m_connections;
        }
        List<CallbackDummy*>::const_iterator it;
        for (it = connections.begin(); it != connections.end(); ++it) {
            CallbackBase<Param...> *callbackBase = static_cast<CallbackBase<Param...>*>(*it);
            (*callbackBase)(param...);
            List<SignalBase*>::iterator it;
            ContextMutexLocker cml(deletedSignalsOnEmitMutex);
            for (it = deletedSignalsOnEmit.begin(); it != deletedSignalsOnEmit.end(); ++it) {
                if (*it == this) {
                    deletedSignalsOnEmit.erase(it);
                    return;
                }
            }
        }
        {
            ContextMutexLocker cml(m_beingEmittedMutex);
            m_beingEmitted = false;
        }
    }

private:
    Signal(SignalResource *parent)
        : SignalBase(parent)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
class SignalCallback
    : public CallbackBase<Param...>
{
public:
    SignalCallback(Signal<Param...> *signal)
        : m_signal(signal)
    {
        this->m_receiver = 0;
    }

    virtual void operator()(const Param&... param)
    {
        m_signal->emit(param...);
    }

    Signal<Param...> *m_signal;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
CallbackBase<Param...> *CallbackBase<Param...>::makeForward(const SignalBase &signal)
{
    Signal<Param...> *receiver = static_cast<Signal<Param...>*>(const_cast<SignalBase*>(&signal));
    return new SignalCallback<Param...>(receiver);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
  * @internal
  */
template <typename... Param>
void Signal<Param...>::disconnect(const Signal<Param...> &signal) const
{
    notifyReceiverDisconnection(signal.parent(), this);
    List<CallbackDummy*>::iterator it;
    ContextMutexLocker cml(m_connectionsMutex);
    for (it = m_connections.begin(); it != m_connections.end(); ++it) {
        SignalCallback<Param...> *const curr = dynamic_cast<SignalCallback<Param...>*>(*it);
        if (curr && curr->m_signal == &signal) {
            m_connections.erase(it);
            delete curr;
            return;
        }
    }
    IDEAL_SDEBUG("no signal forward disconnected. No previous forward found.");
}

}

#endif //IDEAL_SIGNAL_H
