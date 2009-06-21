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

#include <core/file.h>
#include <core/module.h>
#include <core/application.h>
#include <core/extension_loader.h>
#include <core/interfaces/protocol_handler.h>

using namespace IdealCore;

static void statResultSlot(ProtocolHandler::StatResult statResult)
{
    IDEAL_SDEBUG("\t*** Got a stat result");
    IDEAL_SDEBUG("\t\t*** Is valid?\t\t" << (statResult.exists ? "yes" : "no"));
    if (!statResult.exists) {
        return;
    }
    IDEAL_SDEBUG("\t\t*** Is directory?\t" << ((statResult.type & File::Directory) ? "yes" : "no"));
}

class ExtensionLoadDecider
    : public ExtensionLoader::ExtensionLoadDecider
{
public:
    virtual bool loadExtension(const Module::ExtensionInfo &extensionInfo) const
    {
        if (extensionInfo.componentOwner.compare("ideallibrary") ||
            extensionInfo.extensionType != Module::ProtocolHandler) {
            return false;
        }
        ProtocolHandler::AdditionalInfo *additionalInfo = static_cast<ProtocolHandler::AdditionalInfo*>(extensionInfo.additionalInfo);
        return additionalInfo->handlesProtocols.contains("file");
    }
};

int main(int argc, char **argv)
{
    Application app(argc, argv);

    ProtocolHandler *protocolHandler = 0;

    ExtensionLoadDecider *extensionLoadDecider = new ExtensionLoadDecider;
    List<ProtocolHandler*> protocolHandlerList = ExtensionLoader::findExtensions<ProtocolHandler>(extensionLoadDecider, &app);
    delete extensionLoadDecider;

    if (!protocolHandlerList.empty()) {
        protocolHandler = protocolHandlerList.front();
    }

    if (protocolHandler) {
        Object::connectStatic(protocolHandler->statResult, statResultSlot);
        IDEAL_SDEBUG("*** Going to stat " << app.getPath(Application::Home));
        protocolHandler->stat(app.getPath(Application::Home));
        IDEAL_SDEBUG("*** Going to stat " << app.getPath(Application::Home) + "/.bashrc");
        protocolHandler->stat(app.getPath(Application::Home) + "/.bashrc");
        IDEAL_SDEBUG("*** Going to stat " << app.getPath(Application::Home) + "/.nonexistantfile");
        protocolHandler->stat(app.getPath(Application::Home) + "/.nonexistantfile");
    }

    delete protocolHandler;

    return 0;
}
