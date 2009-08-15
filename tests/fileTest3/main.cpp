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

#include <core/application.h>
#include <core/file.h>
#include <core/async_result.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    Application app(argc, argv);

    File f(app.getPath(Application::Home), &app);
    AsyncResult result(&app);
    Object::connect(f.statResult, &result, &AsyncResult::set<ProtocolHandler::StatResult>);
    Thread *stat = f.stat(Thread::Joinable);
    stat->exec();
    stat->join();
    ProtocolHandler::StatResult statResult = result.get<ProtocolHandler::StatResult>(0);

    IDEAL_SDEBUG("uri is " << statResult.uri.uri());
    IDEAL_SDEBUG("owner is " << statResult.ownerUser);

    return 0;
}
