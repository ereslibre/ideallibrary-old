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

#include <core/any.h>
#include <core/uri.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    {
        int a = 100;
        Any b(a);
        IDEAL_SDEBUG("Number is: " << b.get<int>());
    }
    {
        int *a = new int;
        *a = 100;
        Any b(a);
        IDEAL_SDEBUG("Number is: " << *b.get<int*>());
        delete a;
    }
    {
        String a("This is a test");
        Any b(a);
        IDEAL_SDEBUG("String is: " << b.get<String>());
    }
    {
        Uri a("http://www.google.com");
        Any b(a);
        IDEAL_SDEBUG("Host is: " << b.get<Uri>().host());
    }
    return 0;
}
