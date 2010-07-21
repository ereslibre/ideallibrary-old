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

#include "../http.h"

#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

namespace IdealCore {

class BuiltinProtocolHandlersHttp::Private
{
public:
    Private(BuiltinProtocolHandlersHttp *q)
        : m_sockfd(-1)
        , q(q)
    {
    }

    virtual ~Private()
    {
    }

    enum CommandType {
        Get = 0,
        Head
    };

    bool sendCommand(CommandType commandType, const Uri &uri);

    Uri                          m_uri;
    iint32                       m_sockfd;
    static const ichar          *m_commandGet;
    static const ichar          *m_commandHead;
    static const iint32          m_bufferSize;
    BuiltinProtocolHandlersHttp *q;
};

const ichar *BuiltinProtocolHandlersHttp::Private::m_commandGet  = "GET  HTTP/1.1\r\nHost: \r\n\r\n";
const ichar *BuiltinProtocolHandlersHttp::Private::m_commandHead = "HEAD  HTTP/1.1\r\nHost: \r\n\r\n";
const iint32 BuiltinProtocolHandlersHttp::Private::m_bufferSize  = 1024 * 32;

bool BuiltinProtocolHandlersHttp::Private::sendCommand(CommandType commandType, const Uri &uri)
{
    iint32 commandSize;
    switch (commandType) {
        case Get:
            commandSize = strlen(m_commandGet);
            break;
        case Head:
            commandSize = strlen(m_commandHead);
            break;
    }
    commandSize += uri.host().size() + uri.path().size();
    ichar *command = (ichar*) calloc(commandSize + 1, sizeof(ichar));
    switch (commandType) {
        case Get:
            sprintf(command, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", uri.path().data(), uri.host().data());
            break;
        case Head:
            sprintf(command, "HEAD %s HTTP/1.1\r\nHost: %s\r\n\r\n", uri.path().data(), uri.host().data());
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown command type");
            break;
    }
    const iint32 bytesSent = send(m_sockfd, command, commandSize, 0);
    free(command);
    return bytesSent == commandSize;
}

BuiltinProtocolHandlersHttp::BuiltinProtocolHandlersHttp()
    : d(new Private(this))
{
}

BuiltinProtocolHandlersHttp::~BuiltinProtocolHandlersHttp()
{
    delete d;
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersHttp::open(const Uri &uri, iint32 openMode)
{
    if (!uri.isValid()) {
        return InvalidURI;
    }
    d->m_uri = uri;
    d->m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (d->m_sockfd == -1) {
        switch(errno) {
            case EACCES:
                return InsufficientPermissions;
                break;
            default:
                return UnknownError;
                break;
        }
    }
    struct hostent *const host = ::gethostbyname(uri.host().data());
    if (!host) {
        switch (h_errno) {
            case HOST_NOT_FOUND:
                return CouldNotResolveHost;
                break;
            default:
                return UnknownError;
                break;
        }
    }
    struct sockaddr_in destAddr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(uri.port() == -1 ? 80 : uri.port());
    destAddr.sin_addr.s_addr = ::inet_addr(inet_ntoa(*((struct in_addr*) host->h_addr)));
    memset(&(destAddr.sin_zero), '\0', 8);
    if (!::connect(d->m_sockfd, (struct sockaddr*) &destAddr, sizeof(struct sockaddr))) {
        d->sendCommand(Private::Get, uri);
    }
    switch(errno) {
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
        case ETIMEDOUT:
            return CouldNotConnect;
            break;
        default:
            return UnknownError;
            break;
    }
}

ByteStream BuiltinProtocolHandlersHttp::read(size_t nbytes)
{
    if (d->m_sockfd == -1) {
        return ByteStream();
    }
    ichar *buf = (ichar*) calloc(d->m_bufferSize + 1, sizeof(ichar));
    const ssize_t bytesRead = recv(d->m_sockfd, buf, d->m_bufferSize, 0);
    if (bytesRead > 0) {
        ByteStream res(buf, bytesRead);
        free(buf);
        return res;
    }
    free(buf);
    return ByteStream();
}

size_t BuiltinProtocolHandlersHttp::write(const ByteStream &byteStream)
{
    return 0;
}

void BuiltinProtocolHandlersHttp::close()
{
    if (d->m_sockfd > 0) {
        ::close(d->m_sockfd);
        d->m_sockfd = -1;
    }
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
    StatResult statRes;
    statRes.errorCode = NoError;
    statRes.type = RegularFile;
    statRes.uri = uri;
    return statRes;
}

bool BuiltinProtocolHandlersHttp::canBeReusedWith(const Uri &uri) const
{
    return d->m_uri.scheme() == uri.scheme() &&
           d->m_uri.host() == uri.host() &&
           (uri.userInfo().empty() || d->m_uri.userInfo() == uri.userInfo()) &&
           (uri.port() == -1 || d->m_uri.port() == uri.port());
}

}
