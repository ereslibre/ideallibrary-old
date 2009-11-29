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

using namespace IdealCore;

static void eventReceived(const File::EventNotify &eventNotify)
{
    IDEAL_SDEBUG("Received event \"" << eventNotify.event << "\" for URI \"" << eventNotify.uri.uri() << "\"");
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    File f(app.getPath(Application::Home), &app);
    Object::connectStatic(f.event, eventReceived);
    f.setTrackEvents(File::AllEvent);

    return app.exec();
}
