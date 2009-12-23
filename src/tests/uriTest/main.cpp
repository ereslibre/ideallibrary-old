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

#include <core/uri.h>

using namespace IdealCore;

int main(int argc, char **argv)
{
    {
        Uri uri("/non/existant/path/nor/file.txt");
        IDEAL_SDEBUG("Uri is: " << uri.uri());
        IDEAL_SDEBUG("Path is: " << uri.path());
        IDEAL_SDEBUG("Valid: " << (uri.isValid() ? "yes" : "no"));
    }
    {
        Uri uri("http://www.google.com");
        IDEAL_SDEBUG("Uri is: " << uri.uri());
        IDEAL_SDEBUG("Scheme is: " << uri.scheme());
        IDEAL_SDEBUG("Host is: " << uri.host());
        IDEAL_SDEBUG("Valid: " << (uri.isValid() ? "yes" : "no"));
    }
    {
        Uri uri("file:///home/user/file.txt");
        IDEAL_SDEBUG("Uri is: " << uri.uri());
        IDEAL_SDEBUG("Scheme is: " << uri.scheme());
        IDEAL_SDEBUG("Path is: " << uri.path());
        IDEAL_SDEBUG("Valid: " << (uri.isValid() ? "yes" : "no"));
    }
    {
        Uri uri("file:///");
        IDEAL_SDEBUG("Uri is: " << uri.uri());
        IDEAL_SDEBUG("Scheme is: " << uri.scheme());
        IDEAL_SDEBUG("Path is: " << uri.path());
        IDEAL_SDEBUG("Valid: " << (uri.isValid() ? "yes" : "no"));
    }
    {
        Uri uri("file://");
        IDEAL_SDEBUG("Uri is: " << uri.uri());
        IDEAL_SDEBUG("Scheme is: " << uri.scheme());
        IDEAL_SDEBUG("Path is: " << uri.path());
        IDEAL_SDEBUG("Valid: " << (uri.isValid() ? "yes" : "no"));
    }
    {
        Uri uri("svn+ssh://svn.host.com/test/testing");
        IDEAL_SDEBUG("Uri is: " << uri.uri());
        IDEAL_SDEBUG("Scheme is: " << uri.scheme());
        IDEAL_SDEBUG("Host is: " << uri.host());
        IDEAL_SDEBUG("Path is: " << uri.path());
        IDEAL_SDEBUG("Valid: " << (uri.isValid() ? "yes" : "no"));
    }
    {
        Uri uri("ldap://[2001:db8::7]");
        IDEAL_SDEBUG("Uri is: " << uri.uri());
        IDEAL_SDEBUG("Scheme is: " << uri.scheme());
        IDEAL_SDEBUG("Host is: " << uri.host());
        IDEAL_SDEBUG("Valid: " << (uri.isValid() ? "yes" : "no"));
    }
    return 0;
}
