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

#include <core/object.h>
#include <core/ideal_string.h>
#include <locale.h>
#include <iostream>

using namespace IdealCore;

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    {
        Char c(L'𝛏');
        std::cout << "Character is: " << c << std::endl;
    }
    {
        String str("This is a tést with spécial cháracters");
        Char c(L'é');
        std::cout << "String contains 'é'? " << (str.contains(c) ? "yes" : "no") << std::endl;
    }
    {
        String str("What special char ?ñ");
        std::cout << "String contains 'ñ'? " << (str.contains(L'ñ') ? "yes" : "no") << std::endl;
    }
    {
        String str("What special char ?");
        std::cout << "String contains 'ñ'? " << (str.contains(L'ñ') ? "yes" : "no") << std::endl;
    }
    {
        Char c(L'𝛏');
        String str(c);
        std::cout << "String with a char (𝛏) contains: " << str << std::endl;
    }
    {
        String str("Hi, how are you?", 7);
        std::cout << "Should print \"Hi, how\": " << str << std::endl;
    }
    {
        String str("Hí, hów are you?", 7);
        std::cout << "Should print \"Hí, hów\": " << str << std::endl;
    }
    {
        String str("𝛏𝛏𝛏𝛏𝛏", 3);
        std::cout << "Should print \"𝛏𝛏𝛏\": " << str << std::endl;
    }
    {
        String str("𝛏𝛏𝛏𝛏𝛏", 10);
        std::cout << "Should print \"𝛏𝛏𝛏𝛏𝛏\": " << str << std::endl;
    }


    return 0;
}
