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

#include <core/file.h>
#include <core/application.h>
#include <core/extension_loader.h>
#include <core/interfaces/extension_load_decider.h>
#include <core/interfaces/protocol_handler.h>

#define BUFFER_SIZE (1024 * 32)

using namespace IdealCore;

static void statResult(ProtocolHandler::StatResult statResult)
{
    IDEAL_SDEBUG("\t*** Got a stat result");
    IDEAL_SDEBUG("\t\t*** Is Valid?\t\t" << (statResult.errorCode == ProtocolHandler::NoError ? "yes" : "no"));
    if (statResult.errorCode != ProtocolHandler::NoError) {
        return;
    }
    IDEAL_SDEBUG("\t\t*** Is directory?\t" << ((statResult.type & ProtocolHandler::Directory) ? "yes" : "no"));
}

struct ExtensionPHLoadDecider
    : public ExtensionLoadDecider
{
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

    ProtocolHandler *protocolHandler = ExtensionLoader::findFirstExtension<ProtocolHandler>(new ExtensionPHLoadDecider, &app);

    if (protocolHandler) {
        IDEAL_SDEBUG("*** Going to read /usr/include/stdio.h");
        const ProtocolHandler::ErrorCode errorCode = protocolHandler->open("/usr/include/stdio.h", ProtocolHandler::Read);
        if (errorCode == ProtocolHandler::NoError) {
            ByteStream buffer;
            while ((buffer = protocolHandler->read(BUFFER_SIZE)).size()) {
                std::cout << buffer;
            }
            std::cout << std::endl;
            protocolHandler->close();
        } else {
            IDEAL_SDEBUG("An error happened with code " << errorCode);
        }
        IDEAL_SDEBUG("*** Going to stat " << app.getPath(Application::Home));
        statResult(protocolHandler->stat(app.getPath(Application::Home)));
        IDEAL_SDEBUG("*** Going to stat " << app.getPath(Application::Home) + "/.bashrc");
        statResult(protocolHandler->stat(app.getPath(Application::Home) + "/.bashrc"));
        IDEAL_SDEBUG("*** Going to stat " << app.getPath(Application::Home) + "/.nonexistantfile");
        statResult(protocolHandler->stat(app.getPath(Application::Home) + "/.nonexistantfile"));
        IDEAL_SDEBUG("*** Going to stat /root/foo");
        statResult(protocolHandler->stat("/root/foo"));
    }

    return 0;
}
