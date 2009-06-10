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

#ifndef FILE_P_H
#define FILE_P_H

#include <core/file.h>
#include <core/concurrent.h>
#include <core/interfaces/protocol_handler.h>

namespace IdealCore {

class File::Private
{
public:
    Private(File *q);

    ProtocolHandler::ErrorCode m_errorCode;
    bool                       m_stated;
    bool                       m_exists;
    File::Type                 m_type;
    String                     m_ownerUser;
    String                     m_ownerGroup;
    File::Permissions          m_permissions;
    double                     m_size;
    String                     m_contentType;
    Uri                        m_uri;
    File               * const q;

    class Job;
};

class File::Private::Job
    : public Concurrent
{
public:
    enum Operation {
        FileExists = 0,
        FileType,
        FileOwnerUser,
        FileOwnerGroup,
        FilePermissions,
        FileSize,
        FileContentType
    };

    Job(File *file, Type type);

    ProtocolHandler *findProtocolHandler();
    void cacheOrDiscard(ProtocolHandler *protocolHandler);

    void fetchInfo();

    struct LessThanProtocolHandler
    {
        bool operator()(ProtocolHandler *&left, ProtocolHandler *&right);
    };

    File            *m_file;
    Operation        m_operation;
    ProtocolHandler *m_protocolHandler;

protected:
    virtual void run();
};

}

#endif //FILE_P_H
