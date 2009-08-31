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

#include "remote.h"

#include <curl/curl.h>

#include <core/file.h>

namespace IdealCore {

class BuiltinProtocolHandlersRemote::Private
{
public:
    Private(BuiltinProtocolHandlersRemote *q)
        : m_curl(0)
        , m_maxBytes(File::NoMaxBytes)
        , m_currentSize(0)
        , m_isDir(false)
        , q(q)
    {
    }

    ~Private()
    {
        if (m_curl) {
            curl_easy_cleanup(m_curl);
        }
    }

    enum State {
        Receiving = 0,
        Sending
    };

    CURL                          *m_curl;
    Uri                            m_uri;
    bool                           m_success;
    double                         m_maxBytes;
    double                         m_currentSize;
    bool                           m_isDir;
    BuiltinProtocolHandlersRemote *q;
};

BuiltinProtocolHandlersRemote::BuiltinProtocolHandlersRemote()
    : d(new Private(this))
{
}

BuiltinProtocolHandlersRemote::~BuiltinProtocolHandlersRemote()
{
    delete d;
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersRemote::open(const Uri &uri, int openMode)
{
    return Unknown;
}

ByteStream BuiltinProtocolHandlersRemote::read(unsigned int nbytes)
{
    return ByteStream();
}

unsigned int BuiltinProtocolHandlersRemote::write(const ByteStream &byteStream)
{
    return 0;
}

void BuiltinProtocolHandlersRemote::close()
{
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersRemote::mkdir(const Uri &uri, Permissions permissions)
{
    return ProtocolHandler::Unknown;
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersRemote::rm(const Uri &uri)
{
    return ProtocolHandler::Unknown;
}

ProtocolHandler::StatResult BuiltinProtocolHandlersRemote::stat(const Uri &uri)
{
    d->m_uri = uri;
    d->m_success = false;
    if (!d->m_curl) {
        d->m_curl = curl_easy_init();
        curl_easy_setopt(d->m_curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(d->m_curl, CURLOPT_VERBOSE, 0L);
    }
    curl_easy_setopt(d->m_curl, CURLOPT_URL, uri.uri().data());
    curl_easy_setopt(d->m_curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(d->m_curl, CURLOPT_CONNECT_ONLY, 1L);
    curl_easy_setopt(d->m_curl, CURLOPT_WRITEFUNCTION, 0L);
    StatResult statResult;
    statResult.uri = uri;
    if (curl_easy_perform(d->m_curl) == CURLE_OK) {
        d->m_success = true;
        // wait on socket
        if (!uri.scheme().compare("ftp")) {
            // TODO
        } else {
            // TODO
        }
    } else {
        // TODO: handle different error cases
    }
    return statResult;
}

bool BuiltinProtocolHandlersRemote::canBeReusedWith(const Uri &uri) const
{
    return d->m_uri.scheme() == uri.scheme() &&
           d->m_uri.host() == uri.host() &&
           (uri.username().empty() || d->m_uri.username() == uri.username()) &&
           (uri.password().empty() || d->m_uri.password() == uri.password()) &&
           (uri.port() == -1 || d->m_uri.port() == uri.port());
}

String BuiltinProtocolHandlersRemote::name() const
{
    return "Builtin remote protocol handler set";
}

String BuiltinProtocolHandlersRemote::description() const
{
    return "Contains support for http, https, ftp and sftp protocols";
}

String BuiltinProtocolHandlersRemote::author() const
{
    return "Rafael Fernández López";
}

String BuiltinProtocolHandlersRemote::version() const
{
    return IDEALLIBRARY_VERSION;
}

}

EXTENSION_ENTRY_POINT(builtinProtocolHandlersRemote, IdealCore::BuiltinProtocolHandlersRemote)
