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

#include <core/object.h>
#include <core/application.h>
#include <core/ideal_string.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    Application app(argc, argv);
    {
        Char c(L'𝛏');
        IDEAL_SDEBUG("Character is: " << c);
    }
    {
        String str("This is a tést with spécial cháracters");
        Char c(L'é');
        IDEAL_SDEBUG("String contains 'é'? " << (str.contains(c) ? "yes" : "no"));
    }
    {
        String str("What special char ?ñ");
        IDEAL_SDEBUG("String contains 'ñ'? " << (str.contains(L'ñ') ? "yes" : "no"));
    }
    {
        String str("What special char ?");
        IDEAL_SDEBUG("String contains 'ñ'? " << (str.contains(L'ñ') ? "yes" : "no"));
    }
    {
        Char c(L'𝛏');
        String str(c);
        IDEAL_SDEBUG("String with a char (𝛏) contains: " << str);
    }
    {
        String str("Hi, how are you?", 7);
        IDEAL_SDEBUG("Should print \"Hi, how\": " << str);
    }
    {
        String str("Hí, hów are you?", 7);
        IDEAL_SDEBUG("Should print \"Hí, hów\": " << str);
    }
    {
        String str("𝛏𝛏𝛏𝛏𝛏", 3);
        IDEAL_SDEBUG("Should print \"𝛏𝛏𝛏\": " << str);
    }
    {
        String str("𝛏𝛏𝛏𝛏𝛏", 10);
        IDEAL_SDEBUG("Should print \"𝛏𝛏𝛏𝛏𝛏\": " << str);
    }
    {
        String str("one𝛏two𝛏three𝛏four𝛏five𝛏spécial𝛏ñññTestñññ𝛏𝚿𝛏");
        List<String> split = str.split(L'𝛏');
        List<String>::iterator it;
        for (it = split.begin(); it != split.end(); ++it) {
            String str = *it;
            IDEAL_SDEBUG("element: " << str);
        }
    }
    {
        String str1("First part");
        String str2("; Second part");
        str1 += str2;
        IDEAL_SDEBUG("\"" << str1 << "\"");
    }
    {
        String str1("First part");
        const ichar *str2 = "; Second part";
        str1 += str2;
        IDEAL_SDEBUG("\"" << str1 << "\"");
    }
    {
        String str1("First part; ");
        Char c(L'𝛏');
        str1 += c;
        IDEAL_SDEBUG("\"" << str1 << "\"");
    }
    {
        String str1("Hello world !");
        IDEAL_SDEBUG("\"" << str1.substr(6, 5) << "\"");
    }
    {
        String str1("Hello world !");
        IDEAL_SDEBUG("\"" << str1.substr(0, str1.size()) << "\"");
    }

    return 0;
}
