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

    enum ErrorCode {
        NoError = 0,             ///< The operation finished successfully.
        FileNotFound,            ///< The stat of the file failed because the file does not exist.
        InsufficientPermissions, ///< Not enough permissions to stat the file.
        CouldNotResolveHost,     ///< Host could not be resolved.
        CouldNotConnect,         ///< Could not connect.
        LoginFailed,             ///< Login failed.
        Unknown,                 ///< An unknown error happened.
    };

    struct StatResult {
        bool exists;         ///< Whether this file exists or not.
        int type;            ///< The type of this file. Matches File::Type.
        String ownerUser;    ///< The user owner of this file.
        String ownerGroup;   ///< The group owner of this file.
        int permissions;     ///< The permissions of this file. Matches File::Permissions.
        double size;         ///< The size of this file.
        long lastAccessed;   ///< The last access on this file.
        long lastModified;   ///< When was this file last modified.
        String contentType;  ///< The content type of this file.
        Uri uri;             ///< The uri of this file.
    };

    static const double NoMaxBytes = 0;

    /**
      * Sets the working directory at @p uri.
      */
    virtual void cd(const Uri &uri) = 0;

    /**
      * Creates the directory @p uri. @p uri can be either an absolute or a relative path. It
      * will success if the last directory before the one that you are creating existed before.
      * Examples:
      *
      * @code
      * // example of a relative path
      * protocolHandler->cd("/home/testuser");
      * protocolHandler->mkdir("test");
      *
      * // example of an absolute path (fail)
      * protocolHandler->mkdir("/home/testuser/test/foo"); // will fail because "test" does not exist
      *
      * // example of an absolute path (success)
      * protocolHandler->mkdir("/home/testuser/test"); // will succeed if "/home/testuser" does exist
      * @endcode
      */
    virtual void mkdir(const Uri &uri) = 0;

    /**
      * Deletes recursively @p uri.
      */
    virtual void rm(const Uri &uri) = 0;

    /**
      * Stats path @p uri.
      */
    virtual void stat(const Uri &uri) = 0;

    /**
      * Retrieves the file at @p uri. If @p maxBytes is NoMaxBytes, it will be attempted to retrieve the
      * whole file. If @p maxBytes is greater than 0, then a maximum of @p maxBytes will be retrieved.
      *
      * @note Please note that since data will usually come in data packets the implementation should
      *       stop when @p maxBytes has been reached. This does not mean that exactly @p maxBytes will
      *       be read. It is possible that a situation like the next happens (and actually, will be the
      *       normal case):
      *       - @p maxBytes has been set to 1 KB (1024 bytes).
      *       - We have already read 900 bytes of a file.
      *       - A data packet of 300 bytes is coming (dataRead is emitted).
      *       - You end up with 1200 bytes, what is more than 1 KB, but the transmission is stopped,
      *         so dataRead will not be emitted again for this transmission.
      */
    virtual void get(const Uri &uri, double maxBytes = NoMaxBytes) = 0;

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
    IDEAL_SIGNAL(statResult, StatResult);
    IDEAL_SIGNAL(dataRead, ByteStream);
    IDEAL_SIGNAL(error, ErrorCode);

private:
    unsigned int m_weight;
};

}

#endif //PROTOCOL_HANDLER_H
