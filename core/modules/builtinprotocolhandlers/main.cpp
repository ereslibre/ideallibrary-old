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

#include "local.h"
#include "remote.h"

#include <core/module.h>
#include <core/extension.h>

namespace IdealCore {

class ModuleImpl
    : public Module
{
public:
    ModuleImpl()
    {
        List<String> l1;
        l1.push_back("http");
        l1.push_back("https");
        l1.push_back("ftp");
        l1.push_back("sftp");
        additionalInfol1.handlesProtocols = l1;
        List<String> l2;
        l2.push_back(String());
        l2.push_back("file");
        additionalInfol2.handlesProtocols = l2;
    }

    List<Module::ExtensionInfo> extensionInfoList()
    {
            List<Module::ExtensionInfo> res;

            Module::ExtensionInfo l1;
            l1.entryPoint = "builtinProtocolHandlersRemote";
            l1.extensionType = Module::ProtocolHandler;
            l1.additionalInfo = &additionalInfol1;
            l1.componentOwner = "ideallibrary";
            l1.name = "Builtin remote protocol handler set";
            l1.description = "Contains support for http, https, ftp and sftp protocols";
            l1.author = "Rafael Fernández López";
            l1.version = IDEALLIBRARY_VERSION;

            Module::ExtensionInfo l2;
            l2.entryPoint = "builtinProtocolHandlersLocal";
            l2.extensionType = Module::ProtocolHandler;
            l2.additionalInfo = &additionalInfol2;
            l2.componentOwner = "ideallibrary";
            l2.name = "Builtin local protocol handler";
            l2.description = "Contains support for local file operations";
            l2.author = "Rafael Fernández López";
            l2.version = IDEALLIBRARY_VERSION;

            res.push_back(l1);
            res.push_back(l2);

            return res;
    }

private:
    ProtocolHandler::AdditionalInfo additionalInfol1;
    ProtocolHandler::AdditionalInfo additionalInfol2;
};

}

GLOBAL_MODULE_ENTRY_POINT(IdealCore::ModuleImpl)
