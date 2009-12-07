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

#include <ownmodulestest_conf.h>

#include <core/application.h>
#include <core/extension_loader.h>
#include <core/interfaces/extension_load_decider.h>

#include "foointerface.h"

using namespace IdealCore;

class OwnModulesTestApplication
    : public Application
{
public:
    OwnModulesTestApplication(iint32 argc, ichar **argv)
        : Application(argc, argv, "ownModulesTest")
    {
    }

    virtual String prefix() const
    {
        return OWNMODULESTEST_PREFIX;
    }
};

struct ExtensionOwnLoadDecider
    : public ExtensionLoadDecider
{
    bool loadExtension(const Module::ExtensionInfo &extensionInfo) const
    {
        return extensionInfo.extensionType == Module::UserDefinedExtensionType &&
               !extensionInfo.additionalInfo &&
               !extensionInfo.componentOwner.compare("ownModulesTest");
    }
};

int main(int argc, char **argv)
{
    OwnModulesTestApplication app(argc, argv);

    FooInterface *fooInterface = ExtensionLoader::findFirstExtension<FooInterface>(new ExtensionOwnLoadDecider, &app);
    if (fooInterface) {
        IDEAL_SDEBUG("*** Module found. Info following:");
        IDEAL_SDEBUG("\tInterface test call:\t" << fooInterface->testInterface());
        IDEAL_SDEBUG("\tName:\t\t\t" << fooInterface->name());
        IDEAL_SDEBUG("\tDescription:\t\t" << fooInterface->description());
        IDEAL_SDEBUG("\tAuthor:\t\t\t" << fooInterface->author());
        IDEAL_SDEBUG("\tVersion:\t\t" << fooInterface->version());
    } else {
        IDEAL_SDEBUG("*** Module not found");
    }

    delete fooInterface;

    return 0;
}
