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

#ifndef OBJECT_P_H
#define OBJECT_P_H

#include <core/genious_pointer.h>

namespace IdealCore {

class Module;

class Object::Private
{
public:
    Private(Object *q);
    virtual ~Private();

    void addChild(Object *child);
    void removeChild(Object *child);
    /**
      * Recursively add to @p objectList all @p object predecessors. This is used when an object
      * is deleted, recursively delete all its childs.
      */
    void allPredecessors(Object *object, List<Object*> &objectList);
    void cleanConnections();

    Object                       *m_parent;
    Mutex                         m_parentMutex;
    bool                          m_deleteChildrenRecursively;
    Mutex                         m_deleteChildrenRecursivelyMutex;
    bool                          m_blockedSignals;
    Mutex                         m_blockedSignalsMutex;
    bool                          m_emitBlocked;
    Mutex                         m_emitBlockedMutex;
    List<Object*>                 m_children;
    Mutex                         m_childrenMutex;
    List<const SignalBase*>       m_signals;
    Mutex                         m_signalsMutex;
    List<GeniousPointer<Object>*> m_connectedObjects;
    Mutex                         m_connectedObjectsMutex;
    Application                  *m_application;
    Object                       *q;
};

}

#endif //OBJECT_P_H
