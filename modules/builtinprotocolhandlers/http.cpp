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

#include "http.h"

#include <core/file.h>
#include "../../core/uri.h"
#include "../../core/ideal_string.h"

namespace IdealCore {

class BuiltinProtocolHandlersHttp::Private
{
public:
    Private(BuiltinProtocolHandlersHttp *q)
        : q(q)
    {
    }

    ~Private()
    {
    }

    Uri                          m_uri;
    BuiltinProtocolHandlersHttp *q;
};

BuiltinProtocolHandlersHttp::BuiltinProtocolHandlersHttp()
    : d(new Private(this))
{
}

BuiltinProtocolHandlersHttp::~BuiltinProtocolHandlersHttp()
{
    delete d;
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersHttp::open(const Uri &uri, int openMode)
{
    if (!uri.isValid()) {
        return InvalidURI;
    }
    d->m_uri = uri;
    return NoError;
}

ByteStream BuiltinProtocolHandlersHttp::read(unsigned int nbytes)
{
    return ByteStream();
}

unsigned int BuiltinProtocolHandlersHttp::write(const ByteStream &byteStream)
{
    return 0;
}

void BuiltinProtocolHandlersHttp::close()
{
}

List<Uri> BuiltinProtocolHandlersHttp::listDir(const Uri &uri)
{
    return List<Uri>();
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersHttp::mkdir(const Uri &uri,
                                                                Permissions permissions)
{
    return UnknownError;
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersHttp::rm(const Uri &uri)
{
    return UnknownError;
}

ProtocolHandler::StatResult BuiltinProtocolHandlersHttp::stat(const Uri &uri)
{
    d->m_uri = uri;
    return StatResult();
}

bool BuiltinProtocolHandlersHttp::canBeReusedWith(const Uri &uri) const
{
    return d->m_uri.scheme() == uri.scheme() &&
           d->m_uri.host() == uri.host() &&
           (uri.username().empty() || d->m_uri.username() == uri.username()) &&
           (uri.password().empty() || d->m_uri.password() == uri.password()) &&
           (uri.port() == -1 || d->m_uri.port() == uri.port());
}

String BuiltinProtocolHandlersHttp::name() const
{
    return "Builtin remote protocol handler set";
}

String BuiltinProtocolHandlersHttp::description() const
{
    return "Contains support for http, https, ftp and sftp protocols";
}

String BuiltinProtocolHandlersHttp::author() const
{
    return "Rafael Fernández López";
}

String BuiltinProtocolHandlersHttp::version() const
{
    return IDEALLIBRARY_VERSION;
}

}

EXTENSION_ENTRY_POINT(builtinProtocolHandlersHttp, IdealCore::BuiltinProtocolHandlersHttp)
