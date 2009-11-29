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

#include <core/reg_exp.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    {
        RegExp regExp("^abc$");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        IDEAL_SDEBUG("abc matches? " << (regExp.match("abc") ? "yes" : "no"));
        IDEAL_SDEBUG("aabc matches? " << (regExp.match("aabc") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("^(á)*$");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        IDEAL_SDEBUG("á matches? " << (regExp.match("á") ? "yes" : "no"));
        IDEAL_SDEBUG("áá matches? " << (regExp.match("áá") ? "yes" : "no"));
        IDEAL_SDEBUG("ááb matches? " << (regExp.match("ááb") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("^á*$");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        IDEAL_SDEBUG("á matches? " << (regExp.match("á") ? "yes" : "no"));
        IDEAL_SDEBUG("áá matches? " << (regExp.match("áá") ? "yes" : "no"));
        IDEAL_SDEBUG("ááb matches? " << (regExp.match("ááb") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("(á)*(ñ)*b*");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        IDEAL_SDEBUG("áb matches? " << (regExp.match("áb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñb matches? " << (regExp.match("áñb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñ matches? " << (regExp.match("áñ") ? "yes" : "no"));
        IDEAL_SDEBUG("ñb matches? " << (regExp.match("ñb") ? "yes" : "no"));
        IDEAL_SDEBUG("á matches? " << (regExp.match("á") ? "yes" : "no"));
        IDEAL_SDEBUG("ñ matches? " << (regExp.match("ñ") ? "yes" : "no"));
        IDEAL_SDEBUG("b matches? " << (regExp.match("b") ? "yes" : "no"));
        IDEAL_SDEBUG("ááááá matches? " << (regExp.match("ááááá") ? "yes" : "no"));
        IDEAL_SDEBUG("ñññññ matches? " << (regExp.match("ñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññ matches? " << (regExp.match("áááááñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññbbbbb matches? " << (regExp.match("áááááñññññbbbbb") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("á*ñ*b*");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        IDEAL_SDEBUG("áb matches? " << (regExp.match("áb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñb matches? " << (regExp.match("áñb") ? "yes" : "no"));
        IDEAL_SDEBUG("áñ matches? " << (regExp.match("áñ") ? "yes" : "no"));
        IDEAL_SDEBUG("ñb matches? " << (regExp.match("ñb") ? "yes" : "no"));
        IDEAL_SDEBUG("á matches? " << (regExp.match("á") ? "yes" : "no"));
        IDEAL_SDEBUG("ñ matches? " << (regExp.match("ñ") ? "yes" : "no"));
        IDEAL_SDEBUG("b matches? " << (regExp.match("b") ? "yes" : "no"));
        IDEAL_SDEBUG("ááááá matches? " << (regExp.match("ááááá") ? "yes" : "no"));
        IDEAL_SDEBUG("ñññññ matches? " << (regExp.match("ñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññ matches? " << (regExp.match("áááááñññññ") ? "yes" : "no"));
        IDEAL_SDEBUG("áááááñññññbbbbb matches? " << (regExp.match("áááááñññññbbbbb") ? "yes" : "no"));
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("(\\w*):(\\d*)");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        const bool matches = regExp.match("test:1234");
        IDEAL_SDEBUG("test:1234 matches ? " << (matches ? "yes" : "no"));
        if (matches) {
            IDEAL_SDEBUG("text is: " << regExp.getCapture(0));
            IDEAL_SDEBUG("number is: " << regExp.getCapture(1));
        }
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("(\\w*):(\\d*)");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        const bool matches = regExp.match("test:");
        IDEAL_SDEBUG("test: matches ? " << (matches ? "yes" : "no"));
        if (matches) {
            IDEAL_SDEBUG("text is: " << regExp.getCapture(0));
            IDEAL_SDEBUG("number is: " << regExp.getCapture(1));
        }
    }
    IDEAL_SDEBUG("");
    {
        RegExp regExp("(\\w*):(\\d*)");
        IDEAL_SDEBUG("regexp is: " << regExp.regExp());
        const bool matches = regExp.match(":1234");
        IDEAL_SDEBUG(":1234 matches ? " << (matches ? "yes" : "no"));
        if (matches) {
            IDEAL_SDEBUG("text is: " << regExp.getCapture(0));
            IDEAL_SDEBUG("number is: " << regExp.getCapture(1));
        }
    }
    return 0;
}
