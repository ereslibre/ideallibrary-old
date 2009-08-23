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

    static size_t processAndNotifyOutput(void *ptr, size_t size, size_t nmemb, void *stream);

    enum State {
        Receiving = 0,
        Sending
    };

    int waitOnSocket(State state);

    void statFtp();

    CURL                          *m_curl;
    Uri                            m_uri;
    bool                           m_success;
    double                         m_maxBytes;
    double                         m_currentSize;
    bool                           m_isDir;
    BuiltinProtocolHandlersRemote *q;
};

size_t BuiltinProtocolHandlersRemote::Private::processAndNotifyOutput(void *ptr, size_t size, size_t nmemb, void *stream)
{
    ByteStream res(static_cast<char*>(ptr));
    BuiltinProtocolHandlersRemote *const protocolHandler = static_cast<BuiltinProtocolHandlersRemote*>(stream);
    const size_t receivedBytes = size * nmemb;
    if (protocolHandler->d->m_isDir) {
        // TODO: parse output
        IDEAL_SDEBUG("I should be parsing output here :)");
    } else {
        protocolHandler->emit(protocolHandler->dataRead, res);
        protocolHandler->d->m_currentSize += receivedBytes;
        if (protocolHandler->d->m_maxBytes && (protocolHandler->d->m_currentSize >= protocolHandler->d->m_maxBytes)) {
            return 0;
        }
    }
    return receivedBytes;

}

int BuiltinProtocolHandlersRemote::Private::waitOnSocket(State state)
{
    int socket;
    struct timeval tv;
    fd_set infd;
    fd_set outfd;
    fd_set errfd;

    tv.tv_sec = 60;
    tv.tv_usec= 0;

    FD_ZERO(&infd);
    FD_ZERO(&outfd);
    FD_ZERO(&errfd);

    curl_easy_getinfo(m_curl, CURLINFO_LASTSOCKET, &socket);

    FD_SET(socket, &errfd);

    if (state == Receiving) {
        FD_SET(socket, &infd);
    } else {
        FD_SET(socket, &outfd);
    }

    return select(socket + 1, &infd, &outfd, &errfd, &tv);
}

void BuiltinProtocolHandlersRemote::Private::statFtp()
{
    CURLcode res;
    size_t n;
    const char *lsCommand = "LIST\n";
    curl_easy_send(m_curl, lsCommand, strlen(lsCommand), &n);
    do {
        char buf[1024];
        bzero(buf, 1024);
        waitOnSocket(Receiving);
        res = curl_easy_recv(m_curl, buf, 1024, &n);
        IDEAL_SDEBUG("have read " << buf);
    } while (res != CURLE_OK);
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

void BuiltinProtocolHandlersRemote::cp(const Uri &source, const Uri &target)
{
}

void BuiltinProtocolHandlersRemote::mv(const Uri &source, const Uri &target)
{
}

void BuiltinProtocolHandlersRemote::rm(const Uri &uri)
{
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
        d->waitOnSocket(Private::Sending);
        if (!uri.scheme().compare("ftp")) {
            d->statFtp();
        } else {
            // TODO
        }
    } else {
        // TODO: handle different error cases
    }
    return statResult;
}

void BuiltinProtocolHandlersRemote::get(const Uri &uri, double maxBytes)
{
    d->m_maxBytes = maxBytes;
    d->m_currentSize = 0;
    const StatResult statRes = stat(uri);
    d->m_isDir = statRes.exists && statRes.type == ProtocolHandler::Directory;
    if (!d->m_curl) {
        d->m_curl = curl_easy_init();
        curl_easy_setopt(d->m_curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(d->m_curl, CURLOPT_VERBOSE, 0L);
    }
    curl_easy_setopt(d->m_curl, CURLOPT_URL, uri.uri().data());
    curl_easy_setopt(d->m_curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(d->m_curl, CURLOPT_CONNECT_ONLY, 0L);
    curl_easy_setopt(d->m_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(d->m_curl, CURLOPT_WRITEFUNCTION, Private::processAndNotifyOutput);
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
