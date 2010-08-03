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
      * Sets whether emit() is blocked for all signals of this object.
      *
      * @note destroyed signal will always be emitted, even if emit() is blocked for this object.
      */
    void setEmitBlocked(bool emitBlocked);

    /**
      * @return Whether emit() is blocked for all signals of this object.
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
