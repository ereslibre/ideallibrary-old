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

#ifndef EXTENSION_LOADER_H
#define EXTENSION_LOADER_H

#include <ideal_export.h>
#include <core/object.h>
#include <core/ideal_string.h>
#include <core/extension.h>
#include <core/module.h>

namespace IdealCore {

/**
  * @class ExtensionLoader extension_loader.h core/extension_loader.h
  *
  * The Ideal Library extension (and module) loader.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT ExtensionLoader
{
public:
    class ExtensionLoadDecider
    {
    public:
        virtual bool loadExtension(const Module::ExtensionInfo &extensionInfo) const = 0;
    };

    template <typename T>
    static List<T*> findExtensions(ExtensionLoadDecider *extensionLoadDecider, Object *parent);

    template <typename T>
    static T *findFirstExtension(ExtensionLoadDecider *extensionLoadDecider, Object *parent);

private:
    class Private
    {
    public:
        enum Behavior {
            StopAtFirst = 0,
            FindAll
        };

        static Module *loadModule(const String &path, Object *parent);
        static Extension *loadExtension(Module *module, const String &entryPoint);
        static List<Extension*> findExtensions(ExtensionLoadDecider *extensionLoadDecider, Object *parent, Behavior behavior);
    };
};

template <typename T>
List<T*> ExtensionLoader::findExtensions(ExtensionLoadDecider *extensionLoadDecider, Object *parent)
{
    List<T*> retList;
    if (!parent) {
        IDEAL_DEBUG_WARNING("the extension parent cannot be NULL");
        return retList;
    }
    if (!extensionLoadDecider) {
        IDEAL_DEBUG_WARNING("the extension load decider cannot be NULL");
        return retList;
    }
    const List<Extension*> extensionList = Private::findExtensions(extensionLoadDecider, parent, Private::FindAll);
    List<Extension*>::const_iterator it;
    for (it = extensionList.begin(); it != extensionList.end(); ++it) {
        Extension *const extension = *it;
        extension->reparent(parent);
        retList.push_back(static_cast<T*>(extension));
    }
    return retList;
}

template <typename T>
T *ExtensionLoader::findFirstExtension(ExtensionLoadDecider *extensionLoadDecider, Object *parent)
{
    if (!parent) {
        IDEAL_DEBUG_WARNING("the extension parent cannot be NULL");
        return 0;
    }
    if (!extensionLoadDecider) {
        IDEAL_DEBUG_WARNING("the extension load decider cannot be NULL");
        return 0;
    }
    const List<Extension*> extensionList = Private::findExtensions(extensionLoadDecider, parent, Private::StopAtFirst);
    if (extensionList.empty()) {
        return 0;
    }
    return static_cast<T*>(extensionList.front());
}

}

#endif //EXTENSION_LOADER_H
