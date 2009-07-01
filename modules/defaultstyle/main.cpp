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

#include "defaultstyle.h"

#include <core/module.h>

namespace IdealGUI {

class ModuleImpl
    : public IdealCore::Module
{
public:
    IdealCore::List<IdealCore::Module::ExtensionInfo> extensionInfoList()
    {
            IdealCore::List<IdealCore::Module::ExtensionInfo> res;

            IdealCore::Module::ExtensionInfo l1;
            l1.entryPoint = "defaultStyle";
            l1.extensionType = IdealCore::Module::Style;
            l1.additionalInfo = 0;
            l1.componentOwner = "ideallibrary";

            res.push_back(l1);

            return res;
    }
};

}

GLOBAL_MODULE_ENTRY_POINT(IdealGUI::ModuleImpl)
