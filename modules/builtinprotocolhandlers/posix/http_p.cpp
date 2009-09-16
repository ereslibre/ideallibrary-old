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

#include "../http.h"

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

    ~Private()
    {
    }

    Uri                          m_uri;
    int                          m_sockfd;
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
        const char *commandLength = "GET \r\n";
        const int commandSize = strlen(commandLength) + uri.path().size();
        char *command = new char[commandSize];
        bzero(command, commandSize);
        sprintf(command, "GET %s\r\n", uri.path().data());
        const int bytesSent = send(d->m_sockfd, command, commandSize, 0);
        return bytesSent == commandSize ? NoError : UnknownError;
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

}
