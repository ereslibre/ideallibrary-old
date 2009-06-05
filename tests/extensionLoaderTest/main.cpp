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

#include <core/timer.h>
#include <core/module.h>
#include <core/application.h>
#include <core/extension_loader.h>
#include <core/interfaces/protocol_handler.h>
#include <ideal_conf.h>

using namespace IdealCore;
using namespace std;

int main(int argc, char **argv)
{
    Application app(argc, argv);

    Module *module = ExtensionLoader::loadModule(IDEALLIBRARY_PREFIX "/lib/ideal/libfoo.so", &app);
    List<Extension*> loadedExtensions;
    List<Module::ExtensionInfo> extensionList = module->extensionInfoList();
    List<Module::ExtensionInfo>::iterator it;
    int i = 1;
    for (it = extensionList.begin(); it != extensionList.end(); ++it, ++i) {
        Module::ExtensionInfo extensionInfo = *it;
        if (!extensionInfo.componentOwner.compare("extensionLoaderTest")) {
            IDEAL_SDEBUG("*** Extension #" << i);
            IDEAL_SDEBUG("\t*** Entry point:\t\t" << extensionInfo.entryPoint);
            IDEAL_SDEBUG("\t*** Extension type:\t\t" << extensionInfo.extensionType);
            if (!extensionInfo.additionalInfo) {
                IDEAL_SDEBUG("\t*** Additional info:\t\tNo additional info");
            } else {
                IDEAL_SDEBUG("\t*** Additional info:\t\t" << extensionInfo.additionalInfo);
            }
            IDEAL_SDEBUG("\t*** Name:\t\t\t" << extensionInfo.name);
            IDEAL_SDEBUG("\t*** Description:\t\t" << extensionInfo.description);
            IDEAL_SDEBUG("\t*** Author:\t\t\t" << extensionInfo.author);
            IDEAL_SDEBUG("\t*** Version:\t\t\t" << extensionInfo.version);
            // OK ! Load the extension !
            Extension *extension = ExtensionLoader::loadExtension<Extension>(module, extensionInfo.entryPoint, &app);
            loadedExtensions.push_back(extension);
        }
    }

    {
        IDEAL_SDEBUG("*** Manually removing extensions");
        List<Extension*>::iterator it;
        for (it = loadedExtensions.begin(); it != loadedExtensions.end(); ++it) {
            delete *it;
        }
    }

    // This will never happen in this example (because we call to ExtensionLoader::loadExtension),
    // but is the way to go.
    if (module->isUnused()) {
        delete module;
    }

    Timer::callAfter(0, &app, &Application::quit);
    return app.exec();
}
