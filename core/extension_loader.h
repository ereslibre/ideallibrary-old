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
    /**
      * Tries to load a module with name @p name.
      *
      * When you are given the result, you can ask the Module class for the extensions it
      * contains. You can load extensions using ExtensionLoader::loadExtension method.
      *
      * @return The module with name @p name if the loading of the module was successful. 0
      *         otherwise.
      *
      * @note You should never delete the returned content. The Ideal Library will take care of it.
      *       The way it works is: when you call to loadExtension() a reference counter goes up 1 on
      *       the module that contains that extension. When that extension is deleted the reference
      *       counter goes down 1. If this counter hits 0, the module is automatically deleted and
      *       removed from memory.
      *
      * @note For the automatic module freeing it is necessary that Application::exec() has been
      *       called (there is an event loop running).
      *
      * @warning If you call to loadModule() but never call to loadExtension() using that module,
      *          it will never be autodeleted. You are responsible of deleting it as if it were a
      *          regular pointer. You can use the method Module::isUnused() to find out if you can
      *          directly delete it.
      *
      * @see Module
      */
    static Module *loadModule(const String &name, Object *object)
    {
        return Private::loadModule(name, object);
    }

    /**
      * Loads an extension that is at entry point @p entryPoint in @p module.
      *
      * When you load an extension (and it succeeds), the module reference counter goes up 1.
      *
      * @return On success, the extension is loaded and returned, reparented to @p parent. 0 is
      *         returned otherwise.
      *
      * @note The ownership of the pointer is transferred to the user. You are responsible of
      *       deleting the returned pointer when you are done with it.
      *
      * @note The returned object is automatically reparented before being returned, so if you
      *       delete the parent object (and it did not have the isDeleteChildrenRecursively
      *       disabled), this object will also become deleted.
      */
    template <typename T>
    static T *loadExtension(Module *module, const String &entryPoint, Object *parent)
    {
        if (!parent) {
            IDEAL_DEBUG_WARNING("the extension parent cannot be NULL");
            return 0;
        }
        Extension *extension = Private::loadExtension(module, entryPoint);
        if (extension) {
            extension->reparent(parent);
        }
        return static_cast<T*>(extension);
    }

    class ExtensionLoadDecider
    {
    public:
        virtual bool loadExtension(const Module::ExtensionInfo &extensionInfo) const = 0;
    };

    template <typename T>
    static List<T*> findExtensions(ExtensionLoadDecider *extensionLoadDecider, Object *parent)
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
        const List<Extension*> extensionList = Private::findExtensions(extensionLoadDecider, parent);
        List<Extension*>::const_iterator it;
        for (it = extensionList.begin(); it != extensionList.end(); ++it) {
            Extension *const extension = *it;
            extension->reparent(parent);
            retList.push_back(static_cast<T*>(extension));
        }
        return retList;
    }

private:
    class Private
    {
    public:
        static Module *loadModule(const String &path, Object *parent);
        static Extension *loadExtension(Module *module, const String &entryPoint);
        static List<Extension*> findExtensions(ExtensionLoadDecider *extensionLoadDecider, Object *parent);
    };
};

}

#endif //EXTENSION_LOADER_H
