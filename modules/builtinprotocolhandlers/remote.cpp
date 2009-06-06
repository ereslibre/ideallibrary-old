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

static size_t discardOutput(void*, size_t size, size_t nmemb, void*)
{
    return size * nmemb;
}

class BuiltinProtocolHandlersRemote::Private
{
public:
    Private(BuiltinProtocolHandlersRemote *q)
        : m_curl(0)
        , q(q)
    {
    }

    ~Private()
    {
        if (m_curl) {
            curl_easy_cleanup(m_curl);
        }
    }

    StatResult stat(const Uri &uri)
    {
        if (!m_curl) {
            m_curl = curl_easy_init();
            curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1L);
            curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 0L);
            curl_easy_setopt(m_curl, CURLOPT_NOBODY, 1L);
            {
                //### WORKAROUND. NOT REALLY.
                //    As described in http://curl.haxx.se/mail/lib-2008-09/0296.html, when a file
                //    does not exist in FTP and CURLOPT_NOBODY flag is set, CURLE_OK is returned by
                //    curl_easy_perform() instead of CURLE_FTP_COULDNT_RETR_FILE. When
                //    CURLOPT_FILETIME flag is set, CURLE_FTP_COULDNT_RETR_FILE is returned in this
                //    case, what is the expected result.
                //
                //    Since we are stating, we actually want this flag to be set, but still, we
                //    should keep an eye on this, for libcurl correctness.
                curl_easy_setopt(m_curl, CURLOPT_FILETIME, 1L);
            }
            curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, discardOutput);
        }
        curl_easy_setopt(m_curl, CURLOPT_URL, uri.uri().data());
        StatResult statResult;
        statResult.errorCode = ProtocolHandler::Unknown;
        statResult.exists = false;
        statResult.type = File::UnknownType;
        statResult.ownerUser = String();
        statResult.ownerGroup = String();
        statResult.permissions = File::UnknownPermissions;
        statResult.size = 0;
        statResult.lastAccessed = -1;
        statResult.lastModified = -1;
        statResult.contentType = String();
        statResult.uri = uri;
        const CURLcode retCode = curl_easy_perform(m_curl);
        switch (retCode) {
            case CURLE_OK: {
                    statResult.errorCode = ProtocolHandler::NoError;
                    long serverResponse;
                    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &serverResponse);
                    if (serverResponse >= 200 && serverResponse < 400) {
                        statResult.exists = true;
                        curl_easy_getinfo(m_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &statResult.size);
                        curl_easy_getinfo(m_curl, CURLINFO_FILETIME, &statResult.lastModified);
                        char *contentType = 0;
                        curl_easy_getinfo(m_curl, CURLINFO_CONTENT_TYPE, &contentType);
                        if (contentType) {
                            statResult.contentType = contentType;
                        }
                    } else {
                        statResult.errorCode = ProtocolHandler::FileNotFound;
                    }
                }
                break;
            case CURLE_FTP_COULDNT_RETR_FILE:
            case CURLE_REMOTE_FILE_NOT_FOUND:
                statResult.errorCode = ProtocolHandler::FileNotFound;
                break;
            case CURLE_LOGIN_DENIED:
                statResult.errorCode = ProtocolHandler::LoginFailed;
                break;
            case CURLE_COULDNT_RESOLVE_HOST:
                statResult.errorCode = ProtocolHandler::CouldNotResolveHost;
                break;
            case CURLE_COULDNT_CONNECT:
                statResult.errorCode = ProtocolHandler::CouldNotConnect;
                break;
            default:
                IDEAL_DEBUG_WARNING("unknown error code: " << retCode);
                break;
        }
        return statResult;
    }

    CURL                          *m_curl;
    Uri                            m_uri;
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

void BuiltinProtocolHandlersRemote::cd(const Uri &uri)
{
}

void BuiltinProtocolHandlersRemote::mkdir(const Uri &uri)
{
}

void BuiltinProtocolHandlersRemote::rm(const Uri &uri)
{
}

void BuiltinProtocolHandlersRemote::stat(const Uri &uri)
{
    d->m_uri = uri;
    emit(statResult, d->stat(uri));
}

bool BuiltinProtocolHandlersRemote::canBeReusedWith(const Uri &uri) const
{
    return d->m_uri.scheme() == uri.scheme() &&
           d->m_uri.host() == uri.host() &&
           (uri.username().empty() || d->m_uri.username() == uri.username()) &&
           (uri.password().empty() || d->m_uri.password() == uri.password()) &&
           (uri.port() == -1 || d->m_uri.port() == uri.port());
}

}

EXTENSION_ENTRY_POINT(builtinProtocolHandlersRemote, IdealCore::BuiltinProtocolHandlersRemote)
