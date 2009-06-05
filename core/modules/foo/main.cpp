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

#include <core/module.h>
#include <core/extension.h>

#include <core/interfaces/protocol_handler.h>

using namespace IdealCore;

// Extension #1

class ProtocolHandlerFoo
    : public ProtocolHandler
{
public:
    void cd(const Uri &uri)
    {
    }

    void mkdir(const Uri &uri)
    {
    }

    void rm(const Uri &uri)
    {
    }

    void stat(const Uri &uri)
    {
    }

    bool canBeReusedWith(const Uri &uri) const
    {
        return false;
    }
};

// Extension #2

class ExtensionBar
    : public Extension
{
    // Code will come here
};

// Module

class ModuleImpl
    : public Module
{
public:
    enum ExtensionTypeExtended {
        MyOwnType = UserDefinedExtensionType
    };

    ModuleImpl()
    {
        List<String> handlesProtocols;
        handlesProtocols.push_back("foo");
        additionalInfoExtension1.handlesProtocols = handlesProtocols;
    }

    List<Module::ExtensionInfo> extensionInfoList()
    {
            List<Module::ExtensionInfo> res;

            Module::ExtensionInfo l1;
            l1.entryPoint = "extension1";
            l1.extensionType = Module::ProtocolHandler;
            {
                l1.additionalInfo = &additionalInfoExtension1;
            }
            l1.componentOwner = "extensionLoaderTest";
            l1.name = "ProtocolHandlerFoo";
            l1.description = "A Foo ProtocolHandler";
            l1.author = "Rafael Fernández López";
            l1.version = "1.0.1";

            Module::ExtensionInfo l2;
            l2.entryPoint = "extension2";
            l2.extensionType = MyOwnType;
            l2.additionalInfo = 0;
            l2.componentOwner = "extensionLoaderTest";
            l2.name = "Extension Bar";
            l2.description = "An own extension type";
            l2.author = "Rafael Fernández López";
            l2.version = "0.0.5";

            res.push_back(l1);
            res.push_back(l2);

            return res;
    }

private:
    ProtocolHandler::AdditionalInfo additionalInfoExtension1;
};

GLOBAL_MODULE_ENTRY_POINT(ModuleImpl)
EXTENSION_ENTRY_POINT(extension1, ProtocolHandlerFoo)
EXTENSION_ENTRY_POINT(extension2, ExtensionBar)
