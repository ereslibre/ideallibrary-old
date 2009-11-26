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
#include <core/interfaces/extension.h>
#include <core/object.h>
#include <core/ideal_string.h>
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
      * @struct ExtensionLoadDecider extension_loader.h core/extension_loader.h
      *
      * Reimplement if you need to load extensions.
      *
      * @author Rafael Fernández López <ereslibre@ereslibre.es>
      */
    struct ExtensionLoadDecider
    {
        /**
          * @return Whether the extension with information @p extensionInfo should be loaded or not.
          *
          * @code
          * struct Decider
          *     : public ExtensionLoader::ExtensionLoadDecider
          * {
          *     bool loadExtension(const Module::ExtensionInfo &extensionInfo) const
          *     {
          *         return extensionInfo.extensionType == MyExtensionType &&
          *                !extensionInfo.componentOwner.compare("myapplication");
          *     }
          * }
          * @endcode
          *
          * This example is pretty vague, since it will load absolutely all extensions of type
          * MyExtensionType and whose owner is "myapplication". You will probably want to be more
          * strict depending on the context in which you want results.
          *
          * @see Module::ExtensionInfo
          */
        virtual bool loadExtension(const Module::ExtensionInfo &extensionInfo) const = 0;
    };

    /**
      * Find all extensions that satisfy @p extensionLoadDecider, reparenting them to @p parent
      * before being returned.
      *
      * @note The path in which modules (and thus, extensions) are searched is:
      *           - ${APP_PREFIX}/lib/${APP_NAME}/modules
      *           - ${IDEAL_PREFIX}/lib/ideal/modules
      *
      * This means that if you are locating your own modules (and thus, extensions), you will need
      * to reimplement Application class, providing a correct Application::prefix() return value.
      * In this same reasoning, you will also need to provide to the Application constructor the
      * ${APP_NAME} in order to correctly look into that folder for modules.
      *
      * @code
      * #include <myapplication_conf.h> // this includes MYAPPLICATION_PREFIX, which is generated by the buildsystem containing the PREFIX
      * #include <core/extension_loader.h>
      *
      * // We reimplement Application, so we can give the app name and the app prefix. When using
      * // ExtensionLoader modules will be searched on:
      * // "MYAPPLICATION_PREFIX/lib/myapplication/modules/:${IDEAL_PREFIX}/lib/ideal/modules/"
      * class MyApplication
      *     : public Application
      * {
      * public:
      *     MyApplication(iint32 argc, ichar **argv)
      *         : Application(argc, argv, "myapplication")
      *     {
      *     }
      *
      *     virtual String prefix() const
      *     {
      *         return MYAPPLICATION_PREFIX;
      *     }
      * }
      *
      * int main(int argc, char **argv)
      * {
      *     MyApplication myApp(argc, argv);
      *
      *     const List<MyExtension*> myExtensionList = ExtensionLoader::findExtensions<MyExtension>(decider, &myApp);
      *
      *     // do something useful
      *
      *     return myApp.exec();
      * }
      * @endcode
      */
    template <typename T>
    static List<T*> findExtensions(ExtensionLoadDecider *extensionLoadDecider, Object *parent);

    /**
      * @return All extension info of those extensions which satisfy @p extensionLoadDecider. In this
      *         case @p parent has no effect, but a valid @p parent object needs to be specified in
      *         order for this method to work correctly.
      */
    static List<Module::ExtensionInfo> findExtensionsInfo(ExtensionLoadDecider *extensionLoadDecider, Object *parent);

    /**
      * @return The first extension found matching @p extensionLoadDecider, reparented to @p parent
      *         before being returned. If no extensions are found, or no extensions satisfy
      *         @p extensionLoadDecider, 0 is returned.
      */
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
    if (!extensionLoadDecider) {
        IDEAL_DEBUG_WARNING("the extension load decider cannot be NULL");
        return retList;
    }
    if (!parent) {
        IDEAL_DEBUG_WARNING("the extension parent cannot be NULL");
        delete extensionLoadDecider;
        return retList;
    }
    const List<Extension*> extensionList = Private::findExtensions(extensionLoadDecider, parent, Private::FindAll);
    delete extensionLoadDecider;
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
    if (!extensionLoadDecider) {
        IDEAL_DEBUG_WARNING("the extension load decider cannot be NULL");
        return 0;
    }
    if (!parent) {
        IDEAL_DEBUG_WARNING("the extension parent cannot be NULL");
        delete extensionLoadDecider;
        return 0;
    }
    const List<Extension*> extensionList = Private::findExtensions(extensionLoadDecider, parent, Private::StopAtFirst);
    delete extensionLoadDecider;
    if (extensionList.empty()) {
        return 0;
    }
    return static_cast<T*>(extensionList.front());
}

}

#endif //EXTENSION_LOADER_H
