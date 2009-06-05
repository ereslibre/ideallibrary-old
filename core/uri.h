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

#ifndef URI_H
#define URI_H

#include <ideal_export.h>
#include <core/ideal_string.h>

namespace IdealCore {

/**
  * @class Uri uri.h core/uri.h
  *
  * This class represents a URI as defined on RFC 3986.
  * http://www.ietf.org/rfc/rfc3986.txt
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Uri
{
public:
    Uri();
    Uri(const String &uri);
    Uri(const char *uri);
    Uri(const Uri &uri);
    virtual ~Uri();

    /**
      * @return The scheme specified on the URI. An empty string if no scheme specified.
      */
    String scheme() const;

    /**
      * @return The username specified on the URI. An empty string if no username specified.
      */
    String username() const;

    /**
      * @return The password specified on the URI. An empty string if no password specified.
      */
    String password() const;

    /**
      * @return The host specified on the URI. An empty string if no host specified.
      */
    String host() const;

    /**
      * @return The port specified on the URI. -1 if no port specified.
      */
    int port() const;

    /**
      * @return The path specified on the URI. An empty string if no path specified.
      */
    String path() const;

    /**
     * @return The path specified on the URI without the filename. An empty string if no path specified.
     */
    String filenameLessPath() const;

    /**
      * @return The filename specified on the URI. An empty string if no filename specified.
      */
    String filename() const;

    /**
      * @return The full URI
      */
    String uri() const;

    /**
      * @return Whether the URI could be correctly parsed or not.
      */
    bool isValid() const;

    Uri &operator=(const Uri &uri);
    bool operator==(const Uri &uri);
    bool operator!=(const Uri &uri);

private:
    class Private;
    Private *const d;
};

}

#endif //URI_H
