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

#ifndef EXTENSION_LOAD_DECIDER_H
#define EXTENSION_LOAD_DECIDER_H

#include <ideal_export.h>
#include <core/module.h>

namespace IdealCore {

/**
  * @class ExtensionLoadDecider extension_load_decider.h core/interfaces/extension_load_decider.h
  *
  * Reimplement if you need to load extensions.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT ExtensionLoadDecider
{
public:
    ExtensionLoadDecider();
    virtual ~ExtensionLoadDecider();

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

}

#endif //EXTENSION_LOAD_DECIDER_H
