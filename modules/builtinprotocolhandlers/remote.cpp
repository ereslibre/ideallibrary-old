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
        , m_maxBytes(NoMaxBytes)
        , m_currentSize(0)
        , q(q)
    {
    }

    ~Private()
    {
        if (m_curl) {
            curl_easy_cleanup(m_curl);
        }
    }

    static size_t processAndNotifyOutput(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t processAndDiscardOutput(void*, size_t size, size_t nmemb, void*);
    StatResult stat(const Uri &uri);

    CURL                          *m_curl;
    Uri                            m_uri;
    bool                           m_success;
    double                         m_maxBytes;
    double                         m_currentSize;
    BuiltinProtocolHandlersRemote *q;
};

size_t BuiltinProtocolHandlersRemote::Private::processAndNotifyOutput(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ByteStream res(static_cast<char*>(ptr));
    BuiltinProtocolHandlersRemote *const protocolHandler = static_cast<BuiltinProtocolHandlersRemote*>(stream);
    protocolHandler->emit(protocolHandler->dataRead, res);
    const size_t receivedBytes = size * nmemb;
    protocolHandler->d->m_currentSize += receivedBytes;
    if (protocolHandler->d->m_maxBytes && (protocolHandler->d->m_currentSize >= protocolHandler->d->m_maxBytes)) {
        return 0;
    }
    return receivedBytes;
}

size_t BuiltinProtocolHandlersRemote::Private::processAndDiscardOutput(void*, size_t size, size_t nmemb, void*)
{
    // TODO: process timestamp
    return size * nmemb;
}

ProtocolHandler::StatResult BuiltinProtocolHandlersRemote::Private::stat(const Uri &uri)
{
    m_success = false;
    if (!m_curl) {
        m_curl = curl_easy_init();
        curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 0L);
    }
    curl_easy_setopt(m_curl, CURLOPT_URL, uri.uri().data());
    curl_easy_setopt(m_curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(m_curl, CURLOPT_FILETIME, 1L);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, processAndDiscardOutput);
    StatResult statResult;
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
                m_success = true;
            } else {
                emit(q->error, FileNotFound);
            }
        }
            break;
        case CURLE_FTP_COULDNT_RETR_FILE:
        case CURLE_REMOTE_FILE_NOT_FOUND:
            emit(q->error, FileNotFound);
            break;
        case CURLE_LOGIN_DENIED:
            emit(q->error, LoginFailed);
            break;
        case CURLE_COULDNT_RESOLVE_HOST:
            emit(q->error, CouldNotResolveHost);
            break;
        case CURLE_COULDNT_CONNECT:
            emit(q->error, CouldNotConnect);
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown error code: " << retCode);
            break;
        }
    return statResult;
}

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
    const StatResult statRes = d->stat(uri);
    if (d->m_success) {
        emit(statResult, statRes);
    }
}

void BuiltinProtocolHandlersRemote::get(const Uri &uri, double maxBytes)
{
    d->m_uri = uri;
    d->m_maxBytes = maxBytes;
    d->m_currentSize = 0;
    if (!d->m_curl) {
        d->m_curl = curl_easy_init();
        curl_easy_setopt(d->m_curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(d->m_curl, CURLOPT_VERBOSE, 0L);
    }
    curl_easy_setopt(d->m_curl, CURLOPT_URL, uri.uri().data());
    curl_easy_setopt(d->m_curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(d->m_curl, CURLOPT_FILETIME, 0L);
    curl_easy_setopt(d->m_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(d->m_curl, CURLOPT_WRITEFUNCTION, Private::processAndNotifyOutput);
    // TODO: handle return value
    curl_easy_perform(d->m_curl);
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
