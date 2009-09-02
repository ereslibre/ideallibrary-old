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

#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include <ideal_export.h>
#include <core/extension.h>
#include <core/byte_stream.h>

namespace IdealCore {

/**
  * @class ProtocolHandler protocol_handler.h core/interfaces/protocol_handler.h
  *
  * This interface is a generic way of doing certain operations over files by hiding the protocol
  * internal operations and implementation details.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT ProtocolHandler
    : public Extension
{
    friend class File;

public:
    /**
      * @struct AdditionalInfo protocol_handler.h core/interfaces/protocol_handler.h
      *
      * This additional info can be set on the extension, so the component loading the extension
      * can decide whether to load it or not depending on this information
      */
    struct AdditionalInfo {
        List<String> handlesProtocols;
    };

    enum OpenMode {
        Read  = 1, ///< The file is opened for reading
        Write = 2  ///< The file is opened for writing
    };

    enum Permissions {
        NoPermissions      = 0,   ///< No permissions of any kind
        OwnerCanRead       = 1,   ///< The owner of this file can read it
        OwnerCanWrite      = 2,   ///< The owner of this file can write on it
        OwnerCanExecute    = 4,   ///< The owner of this file can execute it
        OwnerMask          = 7,   ///< Masks the Permissions flags on the owner only
        GroupCanRead       = 8,   ///< The group of this file can read it
        GroupCanWrite      = 16,  ///< The group of this file can write on it
        GroupCanExecute    = 32,  ///< The group of this file can execute it
        GroupMask          = 56,  ///< Masks the Permissions flags on the group only
        OthersCanRead      = 64,  ///< Others can read this file
        OthersCanWrite     = 128, ///< Others can write on this file
        OthersCanExecute   = 256, ///< Others can execute this file
        OthersMask         = 448, ///< Masks the Permissions flags on others only
        ReadMask           = 73,  ///< Everybody (owner, group and others) can read
        WriteMask          = 146, ///< Everybody (owner, group and others) can write
        ExecuteMask        = 292, ///< Everybody (owner, group and others) can execute
        UnknownPermissions = 512, ///< Unknown permissions
        SystemDefault      = 1024 ///< Valid only for file/directory creation. Applies the system
                                  ///< default mask.
    };

    enum Type {
        UnknownType     = 0,  ///< This is an unknown type of file
        RegularFile     = 1,  ///< This is a regular file
        Directory       = 2,  ///< This is a directory
        CharacterDevice = 4,  ///< This is a character device
        BlockDevice     = 8,  ///< This is a block device
        Device          = 12, ///< This is a device, character or block one
        SymbolicLink    = 16, ///< This is a symbolic link
        Socket          = 32, ///< This is a socket
        Pipe            = 64  ///< This is a pipe
    };

    enum ErrorCode {
        NoError = 0,             ///< The operation finished successfully.
        FileNotFound,            ///< The stat of the file failed because the file does not exist.
        FileAlreadyExists,       ///< The target URI of the operation already exists.
        InsufficientPermissions, ///< Not enough permissions to stat the file.
        CouldNotResolveHost,     ///< Host could not be resolved.
        CouldNotConnect,         ///< Could not connect.
        LoginFailed,             ///< Login failed.
        Unknown,                 ///< An unknown error happened.
    };

    struct StatResult {
        StatResult();

        ErrorCode errorCode; ///< The error code if an error happened. The stat result is only valid if this
                             ///< field has the NoError value.
        bool exists;         ///< Whether this file exists or not.
        int type;            ///< The type of this file. Matches ProtocolHandler::Type.
        String ownerUser;    ///< The user owner of this file.
        String ownerGroup;   ///< The group owner of this file.
        int permissions;     ///< The permissions of this file. Matches ProtocolHandler::Permissions.
        double size;         ///< The size of this file.
        long lastAccessed;   ///< The last access on this file.
        long lastModified;   ///< When was this file last modified.
        String contentType;  ///< The content type of this file.
        Uri uri;             ///< The uri of this file.
    };

    enum Behavior {
        DoNotOverwriteTarget = 0, ///< Do not overwrite target if it already exists. Emit error signal instead.
        OverwriteTarget           ///< Overwrite target silently if it already exists.
    };

    virtual ErrorCode open(const Uri &uri, int openMode = Read | Write) = 0;

    virtual ByteStream read(unsigned int nbytes) = 0;

    virtual unsigned int write(const ByteStream &byteStream) = 0;

    virtual void close() = 0;

    /**
      * Creates the directory @p uri.
      *
      * @param uri A full URI with absolute path
      */
    virtual ErrorCode mkdir(const Uri &uri, Permissions permissions = SystemDefault) = 0;

    /**
      * Deletes @p uri.
      *
      * @param uri A full URI with absolute path
      */
    virtual ErrorCode rm(const Uri &uri) = 0;

    /**
      * Stats path @p uri.
      *
      * @param uri A full URI with absolute path
      *
      * @return The stat result of the stat operation.
      */
    virtual StatResult stat(const Uri &uri) = 0;

    /**
      * @return Whether this protocol handler can be reused with @p uri. For example, if you have
      *         an already opened connection with a ftp host and visit the same host again while
      *         this protocol handler is cached, it will be reused so it is faster because it does
      *         not have to stablish the connection again (it was kept alive).
      */
    virtual bool canBeReusedWith(const Uri &uri) const = 0;

public:
    ProtocolHandler();

public:
    IDEAL_SIGNAL(dataRead, ByteStream);
    IDEAL_SIGNAL(dirRead, List<Uri>);

private:
    unsigned int m_weight;
};

}

#endif //PROTOCOL_HANDLER_H
