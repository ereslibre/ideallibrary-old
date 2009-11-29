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

#include "foointerface.h"

#include <core/module.h>

class OwnModulesTestFooInterface
    : public FooInterface
{
public:
    virtual String testInterface() const
    {
        return "Yes, it is working";
    }

    virtual String name() const
    {
        return "OwnModulesTestFooInterface";
    }

    virtual String description() const
    {
        return "Just a test";
    }

    virtual String author() const
    {
        return "Rafael Fernández López";
    }

    virtual String version() const
    {
        return "0.0.1";
    }
};

class ModuleImpl
    : public Module
{
public:
    List<Module::ExtensionInfo> extensionInfoList()
    {
        List<Module::ExtensionInfo> res;

        Module::ExtensionInfo l1;
        l1.entryPoint = "ownModulesTestFooInterface";
        l1.extensionType = Module::UserDefinedExtensionType;
        l1.additionalInfo = 0;
        l1.componentOwner = "ownModulesTest";
        res.push_back(l1);

        return res;
    }
};

GLOBAL_MODULE_ENTRY_POINT(ModuleImpl)
EXTENSION_ENTRY_POINT(ownModulesTestFooInterface, OwnModulesTestFooInterface)
