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

#include <dlfcn.h>
#include <core/extension_loader.h>
#include <core/private/module_p.h>
#include <core/private/extension_p.h>

namespace IdealCore {

typedef IdealCore::Module *moduleFactoryCreator();
typedef void moduleFactoryDestructor(IdealCore::Module*);

typedef IdealCore::Extension *extensionFactoryCreator();
typedef void extensionFactoryDestructor(IdealCore::Extension*);

Module *ExtensionLoader::Private::loadModule(const String &path, Object *object)
{
    void *handle = dlopen(path.data(), RTLD_LAZY);
    if (!handle) {
        IDEAL_DEBUG_WARNING("there was a problem dlopening a shared library: " << dlerror());
        return 0;
    }
    char *symbolError = dlerror();
    moduleFactoryCreator *factoryCreator = (moduleFactoryCreator*) dlsym(handle, "globalModuleEntryPoint");
    symbolError = dlerror();
    if (symbolError) {
        IDEAL_DEBUG_WARNING("there was a problem loading a module extension: " << symbolError);
        return 0;
    }
    Module *module = factoryCreator();
    module->d->m_handle = handle;
    module->d->m_path = path;
    module->d->m_application = object->application();
    return module;
}

Extension *ExtensionLoader::Private::loadExtension(Module *module, const String &entryPoint)
{
    if (!module) {
        IDEAL_DEBUG_WARNING("the module you are trying to load the extension from is NULL");
        return 0;
    }
    char *symbolError = dlerror();
    extensionFactoryCreator *factoryCreator = (extensionFactoryCreator*) dlsym(module->d->m_handle, entryPoint.data());
    symbolError = dlerror();
    if (symbolError) {
        IDEAL_DEBUG_WARNING("there was a problem loading a module extension: " << symbolError);
        return 0;
    }
    Extension *extension = factoryCreator();
    module->d->m_unused = false;
    ++module->d->m_refs;
    extension->d->m_module = module;
    return extension;
}

}
