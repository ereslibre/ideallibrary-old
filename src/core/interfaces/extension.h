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

#ifndef EXTENSION_H
#define EXTENSION_H

#include <ideal_export.h>
#include <core/uri.h>
#include <core/object.h>

#define EXTENSION_ENTRY_POINT(name, type)                  \
    extern "C" IDEAL_EXPORT IdealCore::Extension *name() { \
        return new type;                                   \
    }

#define GLOBAL_MODULE_ENTRY_POINT(module)                                 \
    extern "C" IDEAL_EXPORT IdealCore::Module *globalModuleEntryPoint() { \
        return new module;                                                \
    }

namespace IdealCore {

/**
  * @class Extension extension.h core/extension.h
  *
  * The base class for writing an extension that can be easily loaded using the Ideal Library.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  *
  * @see ExtensionLoader, Module
  */
class IDEAL_EXPORT Extension
    : public Object
{
    friend class ExtensionLoader;

public:
    Extension();
    virtual ~Extension();

    /**
      * @return The name of the extension in a human readable way
      */
    virtual String name() const = 0;

    /**
      * @return The description of the extension
      */
    virtual String description() const = 0;

    /**
      * @return The author of the extension
      */
    virtual String author() const = 0;

    /**
      * @return The version of the extension
      */
    virtual String version() const = 0;

private:
    class Private;
    Private *const d;
};

}

#endif //EXTENSION_H
