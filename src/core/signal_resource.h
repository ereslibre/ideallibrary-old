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

#ifndef SIGNAL_RESOURCE_H
#define SIGNAL_RESOURCE_H

#include <ideal_export.h>
#include <core/list.h>
#include <core/mutex.h>

namespace IdealCore {

class SignalBase;

/**
  * @internal
  */
class IDEAL_EXPORT SignalResource
{
    friend class Object;
    friend class SignalBase;
    template <typename Receiver, typename Member, typename... Param>
    friend class Callback;
    template <typename Receiver, typename Member, typename... Param>
    friend class CallbackSynchronized;
    template <typename Receiver, typename Member, typename... Param>
    friend class CallbackMulti;
    template <typename Receiver, typename Member, typename... Param>
    friend class CallbackMultiSynchronized;

public:
    SignalResource();
    virtual ~SignalResource();

    /**
      * Notifies the signal resource that @p signal has been created. This allows us to store
      * meta signal information.
      */
    virtual void signalCreated(const SignalBase *signal);

    /**
      * Notifies the signal resource that @p signal has been connected. This allows us to be aware
      * of the signals connected to this signal resource.
      */
    virtual void signalConnected(const SignalBase *signal);

    /**
      * Notifies the signal resource that @p signal has been disconnected. This allows us to be aware
      * of the signals connected to this signal resource.
      */
    virtual void signalDisconnected(const SignalBase *signal);

    /**
      * Returns the list of signals that exist on this object.
      */
    virtual List<const SignalBase*> signals() const;

    /**
      * Returns whether emit() is blocked for this object or not.
      */
    virtual bool isEmitBlocked() const;

private:
    Mutex m_mutex;
};

}

#endif //SIGNAL_RESOURCE_H
