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

#ifndef REMOTE_H
#define REMOTE_H

#include <core/interfaces/protocol_handler.h>

namespace IdealCore {

class BuiltinProtocolHandlersHttp
    : public ProtocolHandler
{
public:
    BuiltinProtocolHandlersHttp();
    virtual ~BuiltinProtocolHandlersHttp();

    virtual ErrorCode open(const Uri &uri, iint32 openMode);
    virtual ByteStream read(size_t nbytes);
    virtual size_t write(const ByteStream &byteStream);
    virtual void close();
    virtual List<Uri> listDir(const Uri &uri);
    virtual ErrorCode mkdir(const Uri &uri, Permissions permissions);
    virtual ErrorCode rm(const Uri &uri);
    virtual StatResult stat(const Uri &uri);
    virtual bool canBeReusedWith(const Uri &uri) const;

    virtual String name() const;
    virtual String description() const;
    virtual String author() const;
    virtual String version() const;

private:
    class Private;
    Private *const d;
};

}

#endif //REMOTE_H
