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

#define BUFFER_SIZE 1

using namespace IdealCore;

int main(int argc, char **argv)
{
    Application app(argc, argv);

    const File f(Uri(app.getPath(Application::Home), ".bashrc"), &app);
    ProtocolHandler *const protocolHandler = f.protocolHandler();
    IDEAL_SDEBUG("*** Contents following: ");
    protocolHandler->open(f.uri());
    ByteStream read;
    int i = 0;
    while ((read = protocolHandler->read(BUFFER_SIZE)).size()) {
        std::cout << read.data();
        ++i;
    }
    protocolHandler->close();
    IDEAL_SDEBUG("*** I had to read " << i << " times");

    return 0;
}
