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
#include <core/ideal_locale.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    Application app(argc, argv);
    IDEAL_SDEBUG("Codeset: " << app.locale().codeSet());
    IDEAL_SDEBUG("Monday: " << app.locale().weekDay(Locale::Monday));
    IDEAL_SDEBUG("Abbr. Monday: " << app.locale().weekDay(Locale::Monday, Locale::Abbreviated));
    return 0;
}