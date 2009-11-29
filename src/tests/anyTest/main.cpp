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

#include <core/any.h>
#include <core/uri.h>

using namespace IdealCore;

struct MyStruct
{
    bool operator==(const MyStruct &myStruct) const
    {
        return a == myStruct.a && b == myStruct.b && c == myStruct.c;
    }

    iint32 a;
    iint32 b;
    ireal  c;
};

int main(int argc, char **argv)
{
    {
        iint32 a = 100;
        Any b(a);
        IDEAL_SDEBUG("Number is: " << b.get<int>());
        IDEAL_SDEBUG("Type is: " << b.typeName());
    }
    {
        iint32 *a = new iint32;
        *a = 100;
        Any b(a);
        IDEAL_SDEBUG("Number is: " << *b.get<int*>());
        IDEAL_SDEBUG("Type is: " << b.typeName());
        delete a;
    }
    {
        String a("This is a test");
        String other("This is a test");
        Any b(a);
        Any otherb(other);
        IDEAL_SDEBUG("String is: " << b.get<String>());
        IDEAL_SDEBUG("Are they the same ? " << (b == otherb ? "yes" : "no"));
        IDEAL_SDEBUG("Type is: " << b.typeName());
    }
    {
        Uri a("http://www.google.com");
        Any b(a);
        IDEAL_SDEBUG("Host is: " << b.get<Uri>().host());
        IDEAL_SDEBUG("Type is: " << b.typeName());
    }
    IDEAL_SDEBUG("*** Going to do something illegal");
    {
        MyStruct a;
        Any b(a);
        const String myString = b.get<String>();
        IDEAL_SDEBUG("myString is: \"" << myString << "\"");
    }
    IDEAL_SDEBUG("*** Using the static methods");
    {
        String a("This is a test");
        IDEAL_SDEBUG("Type is: " << Any::typeName(a));
    }
    {
        Uri a("http://www.google.com");
        IDEAL_SDEBUG("Type is: " << Any::typeName(a));
    }
    {
        String *a = 0;
        IDEAL_SDEBUG("Type is: " << Any::typeName(a));
    }
    {
        List<List<Uri*>*> a;
        IDEAL_SDEBUG("Type is: " << Any::typeName(a));
    }
    {
        MyStruct a;
        IDEAL_SDEBUG("Type is: " << Any::typeName(a));
    }
    return 0;
}
