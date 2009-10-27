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

#include <core/reg_exp.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    {
        RegExp regExp("^abc$");
        IDEAL_SDEBUG("regexp is: ^abc$");
        IDEAL_SDEBUG("abc matches? " << (regExp.fullMatch("abc") ? "yes" : "no"));
        IDEAL_SDEBUG("aabc matches? " << (regExp.fullMatch("aabc") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("^(á)*$");
        IDEAL_SDEBUG("regexp is: ^(á)*$");
        IDEAL_SDEBUG("á matches? " << (regExp.fullMatch("á") ? "yes" : "no"));
        IDEAL_SDEBUG("áá matches? " << (regExp.fullMatch("áá") ? "yes" : "no"));
        IDEAL_SDEBUG("ááb matches? " << (regExp.fullMatch("ááb") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("^á*$");
        IDEAL_SDEBUG("regexp is: ^á*$");
        IDEAL_SDEBUG("á matches? " << (regExp.fullMatch("á") ? "yes" : "no"));
        IDEAL_SDEBUG("áá matches? " << (regExp.fullMatch("áá") ? "yes" : "no"));
        IDEAL_SDEBUG("ááb matches? " << (regExp.fullMatch("ááb") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("(á)*(ñ)*b*");
        IDEAL_SDEBUG("regexp is: (á)*(ñ)*b*");
        IDEAL_SDEBUG("áb matches? " << (regExp.fullMatch("áb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñb matches? " << (regExp.fullMatch("áñb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñ matches? " << (regExp.fullMatch("áñ") ? "yes" : "no"));
        IDEAL_SDEBUG("ñb matches? " << (regExp.fullMatch("ñb") ? "yes" : "no"));
        IDEAL_SDEBUG("á matches? " << (regExp.fullMatch("á") ? "yes" : "no"));
        IDEAL_SDEBUG("ñ matches? " << (regExp.fullMatch("ñ") ? "yes" : "no"));
        IDEAL_SDEBUG("b matches? " << (regExp.fullMatch("b") ? "yes" : "no"));
        IDEAL_SDEBUG("ááááá matches? " << (regExp.fullMatch("ááááá") ? "yes" : "no"));
        IDEAL_SDEBUG("ñññññ matches? " << (regExp.fullMatch("ñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññ matches? " << (regExp.fullMatch("áááááñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññbbbbb matches? " << (regExp.fullMatch("áááááñññññbbbbb") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("á*ñ*b*");
        IDEAL_SDEBUG("regexp is: á*ñ*b*");
        IDEAL_SDEBUG("áb matches? " << (regExp.fullMatch("áb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñb matches? " << (regExp.fullMatch("áñb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñ matches? " << (regExp.fullMatch("áñ") ? "yes" : "no"));
        IDEAL_SDEBUG("ñb matches? " << (regExp.fullMatch("ñb") ? "yes" : "no"));
        IDEAL_SDEBUG("á matches? " << (regExp.fullMatch("á") ? "yes" : "no"));
        IDEAL_SDEBUG("ñ matches? " << (regExp.fullMatch("ñ") ? "yes" : "no"));
        IDEAL_SDEBUG("b matches? " << (regExp.fullMatch("b") ? "yes" : "no"));
        IDEAL_SDEBUG("ááááá matches? " << (regExp.fullMatch("ááááá") ? "yes" : "no"));
        IDEAL_SDEBUG("ñññññ matches? " << (regExp.fullMatch("ñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññ matches? " << (regExp.fullMatch("áááááñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññbbbbb matches? " << (regExp.fullMatch("áááááñññññbbbbb") ? "yes" : "no"));
    }
    return 0;
}
