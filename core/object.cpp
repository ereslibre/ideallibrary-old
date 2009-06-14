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

#include "object.h"
#include "private/object_p.h"

#include <core/application.h>
#include "private/application_p.h"

namespace IdealCore {

Object::Private::Private()
    : m_deleteChildrenRecursively(true)
    , m_blockedSignals(false)
    , m_emitBlocked(false)
{
}

void Object::Private::addChild(Object *child)
{
    ContextMutexLocker cml(m_childrenMutex);
    m_children.push_back(child);
}

void Object::Private::removeChild(Object *child)
{
    ContextMutexLocker cml(m_childrenMutex);
    m_children.remove(child);
}

void Object::Private::allPredecessors(Object *object, List<Object*> &objectList)
{
    List<Object*>::iterator it;
    ContextMutexLocker cml(object->d->m_childrenMutex);
    for (it = object->d->m_children.begin(); it != object->d->m_children.end(); ++it) {
        allPredecessors(*it, objectList);
    }
    objectList.push_back(object);
}

void Object::Private::cleanConnections()
{
    List<const SignalBase*> signalsToDisconnect;
    {
        ContextMutexLocker cml(m_connectedObjectsMutex);
        List<GeniousPointer<Object>*>::iterator it = m_connectedObjects.begin();
        while (it != m_connectedObjects.end()) {
            GeniousPointer<Object> *connectedObject = *it;
            if (!connectedObject->isContentDestroyed()) {
                List<const SignalBase*> connectedObjectSignals = connectedObject->content()->signals();
                List<const SignalBase*>::iterator sigIt = connectedObjectSignals.begin();
                while (sigIt != connectedObjectSignals.end()) {
                    const SignalBase *signalBase = *sigIt;
                    if (signalBase->m_parent != q) {
                        signalsToDisconnect.push_back(signalBase);
                    }
                    ++sigIt;
                }
                ++it;
            } else {
                it = m_connectedObjects.erase(it);
                delete connectedObject;
            }
        }
    }
    List<const SignalBase*>::iterator sigIt;
    for (sigIt = signalsToDisconnect.begin(); sigIt != signalsToDisconnect.end(); ++sigIt) {
        (*sigIt)->disconnect(q);
    }
}

Object::Object(Object *parent)
    : d(new Private)
    , IDEAL_SIGNAL_INIT(destroyed)
{
    d->m_parent = parent;
    if (!parent) {
        IDEAL_DEBUG_WARNING("parent of object is null. This is wrong and will cause problems");
        d->m_application = 0;
        return;
    }
    d->m_application = parent->d->m_application;
    parent->d->addChild(this);
}

Object::~Object()
{
    m_mutex.tryLock();
    m_mutex.unlock();
    emit(destroyed);
    d->cleanConnections();
    {
        ContextMutexLocker cml(d->m_parentMutex);
        if (d->m_parent) {
            d->m_parent->d->removeChild(this);
        }
    }
    d->m_deleteChildrenRecursivelyMutex.lock();
    if (d->m_deleteChildrenRecursively) {
        List<Object*> objectsToDelete;
        d->allPredecessors(this, objectsToDelete);
        objectsToDelete.pop_back(); // 'this' is the last element, and we are already deleting it =)
        List<Object*>::iterator it;
        for (it = objectsToDelete.begin(); it != objectsToDelete.end(); ++it) {
            Object *const currObject = *it;
            currObject->m_mutex.lock();
            delete currObject;
        }
    }
    d->m_deleteChildrenRecursivelyMutex.unlock();
    delete d;
}

void Object::setDeleteChildrenRecursively(bool deleteChildrenRecursively)
{
    ContextMutexLocker cml(d->m_deleteChildrenRecursivelyMutex);
    d->m_deleteChildrenRecursively = deleteChildrenRecursively;
}

bool Object::isDeleteChildrenRecursively() const
{
    ContextMutexLocker cml(d->m_deleteChildrenRecursivelyMutex);
    return d->m_deleteChildrenRecursively;
}

void Object::setBlockedSignals(bool blockedSignals)
{
    ContextMutexLocker cml(d->m_blockedSignalsMutex);
    d->m_blockedSignals = blockedSignals;
}

bool Object::areSignalsBlocked() const
{
    ContextMutexLocker cml(d->m_blockedSignalsMutex);
    return d->m_blockedSignals;
}

void Object::setEmitBlocked(bool emitBlocked)
{
    ContextMutexLocker cml(d->m_emitBlockedMutex);
    d->m_emitBlocked = emitBlocked;
}

bool Object::isEmitBlocked() const
{
    ContextMutexLocker cml(d->m_emitBlockedMutex);
    return d->m_emitBlocked;
}

List<Object*> Object::children() const
{
    ContextMutexLocker cml(d->m_childrenMutex);
    return d->m_children;
}

Object *Object::parent() const
{
    ContextMutexLocker cml(d->m_parentMutex);
    return d->m_parent;
}

void Object::reparent(Object *parent)
{
    ContextMutexLocker cml(d->m_parentMutex);
    if (d->m_parent == parent) {
        return;
    }
    if (d->m_application && (d->m_application != parent->d->m_application)) {
        IDEAL_DEBUG_WARNING("could not reparent. Trying to reparent a child to a parent that is in a different Application object");
        return;
    }
    if (d->m_parent) {
        d->m_parent->d->removeChild(this);
    }
    d->m_parent = parent;
    if (parent) {
        parent->d->addChild(this);
    }
}

Application *Object::application() const
{
    return d->m_application;
}

void Object::disconnectSender(Object *sender)
{
    List<const SignalBase*>::iterator it;
    ContextMutexLocker cml(sender->d->m_signalsMutex);
    for (it = sender->d->m_signals.begin(); it != sender->d->m_signals.end(); ++it) {
        (*it)->disconnect();
    }
}

void Object::disconnectReceiver(Object *receiver)
{
    List<const SignalBase*> signalsToDisconnect;
    List<GeniousPointer<Object>*>::iterator it;
    {
        ContextMutexLocker cml(receiver->d->m_connectedObjectsMutex);
        for (it = receiver->d->m_connectedObjects.begin(); it != receiver->d->m_connectedObjects.end(); ++it) {
            GeniousPointer<Object> *const object = *it;
            if (!object->isContentDestroyed()) {
                Object *const obj = object->content();
                List<const SignalBase*>::iterator sigIt;
                ContextMutexLocker cml(obj->d->m_signalsMutex);
                for (sigIt = obj->d->m_signals.begin(); sigIt != obj->d->m_signals.end(); ++sigIt) {
                    signalsToDisconnect.push_back(*sigIt);
                }
            }
        }
    }
    List<const SignalBase*>::iterator sigIt;
    for (sigIt = signalsToDisconnect.begin(); sigIt != signalsToDisconnect.end(); ++sigIt) {
        (*sigIt)->disconnect(receiver);
    }
}

void Object::fullyDisconnect(Object *object)
{
    disconnectSender(object);
    disconnectReceiver(object);
}

void Object::deleteNow()
{
    delete this;
}

void Object::deleteLater()
{
    List<Object*>::iterator it;
    ContextMutexLocker cml(d->m_application->d->m_markedForDeletionMutex);
    for (it = d->m_application->d->m_markedForDeletion.begin(); it != d->m_application->d->m_markedForDeletion.end(); ++it) {
        if (*it == this) {
            return;
        }
    }
    d->m_application->d->m_markedForDeletion.push_back(this);
}

void *Object::virtual_hook(int id, void *param)
{
    return 0;
}

void Object::signalCreated(const SignalBase *signal)
{
    ContextMutexLocker cml(d->m_signalsMutex);
    d->m_signals.push_back(signal);
}

void Object::signalConnected(const SignalBase *signal)
{
    ContextMutexLocker cml(d->m_connectedObjectsMutex);
    d->m_connectedObjects.push_back(new GeniousPointer<Object>(dynamic_cast<Object*>(signal->m_parent)));
}

void Object::signalDisconnected(const SignalBase *signal)
{
    List<GeniousPointer<Object>*>::iterator it;
    ContextMutexLocker cml(d->m_connectedObjectsMutex);
    for (it = d->m_connectedObjects.begin(); it != d->m_connectedObjects.end(); ++it) {
        GeniousPointer<Object> *connectedObject = *it;
        if (connectedObject->content() == signal->m_parent) {
            d->m_connectedObjects.erase(it);
            delete connectedObject;
            return;
        }
    }
}

List<const SignalBase*> Object::signals() const
{
    ContextMutexLocker cml(d->m_signalsMutex);
    return d->m_signals;
}

Object::Object()
    : d(new Private)
    , IDEAL_SIGNAL_INIT(destroyed)
{
    d->m_parent = 0;
    d->m_application = 0;
}

}
