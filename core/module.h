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

#ifndef MODULE_H
#define MODULE_H

#include <ideal_export.h>
#include <core/ideal_string.h>

namespace IdealCore {

/**
  * @class Module module.h core/module.h
  *
  * A shared library can only contain one module. This module can contain several extensions that
  * are shown to the world by the method extensionInfoList. An example of a module with only one
  * extension would be:
  *
  * @code
  * class MyOwnModule
  *     : public Module
  * {
  * public:
  *     List<Module::ExtensionInfo> extensionInfoList()
  *     {
  *         List<Module::ExtensionInfo> res;
  *
  *         Module::ExtensionInfo extInfo;
  *         extInfo.entryPoint = "extension1"; // (*)
  *         extInfo.extensionType = VideoPluginType;
  *         extInfo.componentOwner = "myApplicationName";
  *         extInfo.name = "Video Plugin for Foo";
  *         extInfo.description = "A Video Plugin for Foo";
  *         extInfo.author = "The author of the plugin";
  *         extInfo.version = "3.141592";
  *
  *         res.push_back(extInfo);
  *
  *         return res;
  *     }
  * };
  *
  * GLOBAL_MODULE_ENTRY_POINT(MyOwnModule)
  * @endcode
  *
  * This example shows the shared library from the module point of view. It is not complete. For it
  * being complete, it is missing:
  *
  * @code
  * class VideoPluginFoo
  *     : public VideoPlugin // VideoPlugin usually will be an interface inheriting IdealCore::Extension
  * {
  *     // Code comes here
  * };
  *
  * // Also here...
  *
  * EXTENSION_ENTRY_POINT(extension1, VideoPluginFoo) // note that the first parameter is the entry
  *                                                   // point, set at (*)
  * @endcode
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Module
{
    friend class Extension;
    friend class Application;
    friend class ExtensionLoader;

public:
    enum ExtensionType {
        ProtocolHandler = 0,             ///< Protocol handler extension type
        UserDefinedExtensionType = 1000  ///< User defined extension types should start at this value
    };

    Module();
    virtual ~Module();

    /**
      * @return The path of the module. This property is read only, because this is automatically
      *         set when you call to ExtensionLoader::loadModule().
      */
    String path() const;

    /**
      * @return Whether this module has been used in an ExtensionLoader::loadExtension() call.
      *         If this module has not been used in a loadExtension() call, this module will not
      *         be autodeleted. This means that if true is returned here, the user is the responsible
      *         of freeing the memory of the module by deleting the pointer.
      *
      * @note The best way to go is to do something like this:
      *
      * @code
      * Module *module = ExtensionLoader::loadModule(path);
      * List<ExtensionInfo> extensionInfoList = module->extensionInfoList();
      * List<ExtensionInfo>::iterator it;
      * for (it = extensionInfoList.begin(); it != extensionInfoList.end(); ++it) {
      *     ExtensionInfo extensionInfo = *it;
      *     // do some checks based on extensionInfo.extensionType, extensionInfo.componentOwner and
      *     // possibly on extensionInfo.additionalInfo.
      *     if (checksSucceeded) {
      *         Whatever *whatever = ExtensionLoader::loadExtension<Whatever>(module, extensionInfo.entryPoint, &app);
      *         // do whatever with whatever
      *     }
      *     // you can see that if checksSucceeded was always false for all elements (no extension
      *     // is of your interest in the opened module), the module will never become autodeleted
      *     // because loadExtension has never been called with it.
      * }
      * // for this reason, we do something like this were appropriate
      * if (module->isUnused()) {
      *     delete module;
      * }
      * @endcode
      */
    bool isUnused() const;

    struct ExtensionInfo
    {
        /**
          * The entry point of the extension.
          */
        String entryPoint;

        /**
          * The extension type. @see Module::ExtensionType.
          */
        int extensionType;

        /**
          * If only extension type is not enough for determining whether to load the extension or not,
          * it can return additional info based on what extension type it is (so you know to what you
          * can cast it), and inspect the information it provides.
          */
        void *additionalInfo;

        /**
          * If several applications have used the same extensionType and are saving the modules in
          * the same folder (which should not happen), this will help to discriminate those
          * extensions that will fail when casting because they weren't written for this application
          * (or library).
          */
        String componentOwner;

        /**
          * The name of the extension (human readable).
          */
        String name;

        /**
          * The description of the extension.
          */
        String description;

        /**
          * The author (or authors) of the extension.
          */
        String author;

        /**
          * The version of the extension.
          */
        String version;
    };

    /**
      * Return the list of extensions that this module has built inside. Each extension contains an
      * entry point as well as an extension type to determine what is its purpose before loading
      * it.
      */
    virtual List<ExtensionInfo> extensionInfoList() = 0;

private:
    class Private;
    Private *const d;
};

}

#endif //MODULE_H
