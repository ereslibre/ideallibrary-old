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

#ifndef OBJECT_P_H
#define OBJECT_P_H

#include <core/genious_pointer.h>

namespace IdealCore {

class Module;

class Object::Private
{
public:
    Private(Object *q);

    void addChild(Object *child);
    void removeChild(Object *child);
    /**
      * Recursively add to @p objectList all @p object predecessors. This is used when an object
      * is deleted, recursively delete all its childs.
      */
    void allPredecessors(Object *object, List<Object*> &objectList);
    void cleanConnections();

    Object                       *m_parent;
    bool                          m_deleteChildrenRecursively;
    bool                          m_blockedSignals;
    bool                          m_emitBlocked;
    List<Object*>                 m_children;
    List<const SignalBase*>       m_signals;
    List<GeniousPointer<Object>*> m_connectedObjects;
    Application                  *m_application;
    Object                       *q;
};

}

#endif //OBJECT_P_H
