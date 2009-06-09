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

using namespace IdealCore;
using namespace std;

int main(int argc, char **argv)
{
    Application app(argc, argv);

    IDEAL_SDEBUG("*** Prefix path is:\t" << app.getPath(Application::Prefix));
    IDEAL_SDEBUG("*** Path is:\t\t" << app.getPath(Application::Global));
    IDEAL_SDEBUG("*** Library path is:\t" << app.getPath(Application::Library));
    IDEAL_SDEBUG("*** pkg-config path is:\t" << app.getPath(Application::PkgConfig));
    IDEAL_SDEBUG("*** Home path is:\t" << app.getPath(Application::Home));
    IDEAL_SDEBUG("*** Module path is:\t" << app.getPath(Application::Module));

    return app.quit();
}
