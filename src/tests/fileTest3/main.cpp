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

#include <core/application.h>
#include <core/file.h>
#include <core/async_result.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    Application app(argc, argv);

    AsyncResult result(&app);

    {
        IDEAL_SDEBUG("*** Going to stat " << app.getPath(Application::Home));

        File f(app.getPath(Application::Home), &app);
        Object::connect(f.statResult, &result, &AsyncResult::set<ProtocolHandler::StatResult>);
        f.stat(Thread::Joinable)->execAndJoin();

        // it is possible that statResult wasn't emitted, (e.g. when not enough permissions to
        // access the file)
        if (!result.resultReceived()) {
            IDEAL_DEBUG_WARNING("result is empty");
            return 0;
        }

        ProtocolHandler::StatResult statResult = result.get<ProtocolHandler::StatResult>(0);

        if (statResult.errorCode != ProtocolHandler::NoError) {
            if (statResult.errorCode == ProtocolHandler::InsufficientPermissions) {
                IDEAL_SDEBUG("not enough permissions to stat " << f.uri().uri());
            } else {
                IDEAL_SDEBUG("unknown error while stating " << f.uri().uri() << ": " << statResult.errorCode);
            }
        } else {
            IDEAL_SDEBUG("type is " << statResult.type);
            IDEAL_SDEBUG("owner user is " << statResult.ownerUser);
            IDEAL_SDEBUG("owner group is " << statResult.ownerGroup);
            IDEAL_SDEBUG("permissions is " << statResult.permissions);
            IDEAL_SDEBUG("size is " << statResult.size);
            IDEAL_SDEBUG("last accessed is " << statResult.lastAccessed);
            IDEAL_SDEBUG("last modified is " << statResult.lastModified);
            IDEAL_SDEBUG("content type is " << statResult.contentType);
            IDEAL_SDEBUG("uri is " << statResult.uri.uri());
        }
    }

    {
        result.clear();

        IDEAL_SDEBUG("");
        IDEAL_SDEBUG("*** Going to stat /root/foo");

        File f("/root/foo", &app);
        Object::connect(f.statResult, &result, &AsyncResult::set<ProtocolHandler::StatResult>);
        f.stat(Thread::Joinable)->execAndJoin();

        const ProtocolHandler::StatResult statResult = result.get<ProtocolHandler::StatResult>(0);

        if (statResult.errorCode != ProtocolHandler::NoError) {
            if (statResult.errorCode == ProtocolHandler::InsufficientPermissions) {
                IDEAL_SDEBUG("not enough permissions to stat " << f.uri().uri());
            } else {
                IDEAL_SDEBUG("unknown error while stating " << f.uri().uri() << ": " << statResult.errorCode);
            }
        } else {
            IDEAL_SDEBUG("type is " << statResult.type);
            IDEAL_SDEBUG("owner user is " << statResult.ownerUser);
            IDEAL_SDEBUG("owner group is " << statResult.ownerGroup);
            IDEAL_SDEBUG("permissions is " << statResult.permissions);
            IDEAL_SDEBUG("size is " << statResult.size);
            IDEAL_SDEBUG("last accessed is " << statResult.lastAccessed);
            IDEAL_SDEBUG("last modified is " << statResult.lastModified);
            IDEAL_SDEBUG("content type is " << statResult.contentType);
            IDEAL_SDEBUG("uri is " << statResult.uri.uri());
        }
    }

    {
        result.clear();

        IDEAL_SDEBUG("");
        IDEAL_SDEBUG("*** Going to stat ftp://ftp.gnu.org");

        File f("ftp://ftp.gnu.org", &app);
        Object::connect(f.statResult, &result, &AsyncResult::set<ProtocolHandler::StatResult>);
        f.stat(Thread::Joinable)->execAndJoin();

        ProtocolHandler::StatResult statResult = result.get<ProtocolHandler::StatResult>(0);

        if (statResult.errorCode != ProtocolHandler::NoError) {
            if (statResult.errorCode == ProtocolHandler::InsufficientPermissions) {
                IDEAL_SDEBUG("not enough permissions to stat " << f.uri().uri());
            } else {
                IDEAL_SDEBUG("unknown error while stating " << f.uri().uri() << ": " << statResult.errorCode);
            }
        } else {
            IDEAL_SDEBUG("type is " << statResult.type);
            IDEAL_SDEBUG("owner user is " << statResult.ownerUser);
            IDEAL_SDEBUG("owner group is " << statResult.ownerGroup);
            IDEAL_SDEBUG("permissions is " << statResult.permissions);
            IDEAL_SDEBUG("size is " << statResult.size);
            IDEAL_SDEBUG("last accessed is " << statResult.lastAccessed);
            IDEAL_SDEBUG("last modified is " << statResult.lastModified);
            IDEAL_SDEBUG("content type is " << statResult.contentType);
            IDEAL_SDEBUG("uri is " << statResult.uri.uri());
        }
    }

    return 0;
}
