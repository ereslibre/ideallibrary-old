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

#ifndef FILE_H
#define FILE_H

#include <ideal_export.h>
#include <core/uri.h>
#include <core/object.h>
#include <core/concurrent.h>
#include <core/interfaces/protocol_handler.h>

namespace IdealCore {

class Concurrent;

/**
  * @class File file.h core/file.h
  *
  * This class represents a file in a local or remote filesystem. If the file is located on the
  * local file system, all the information will be retrieved through system calls. On the other hand
  * if the file is living on a remote filesystem, we will try to create a correct protocol handler,
  * which will get the file information.
  *
  * @note The API for retrieving information about files is asynchronous, but it is fairly easy to
  *       retrieve the contents of several files reusing code, using Object::connectMulti or
  *       Object::connectStaticMulti. For example, say we want to find out if a list of files
  *       exist. We could do:
  *
  * @code
  * List<File*>::iterator it;
  * for (it = fileList.begin(); it != fileList.end(); ++it) {
  *     File *const file = *it;
  *     connectMulti(file->existsResult, this, &MyObject::existsResultSlot);
  *     Concurrent *const size = file->size();
  *     size->exec();
  * }
  *
  * void MyObject::existsResultSlot(Object *sender, bool exists)
  * {
  *     File *const sdr = static_cast<File*>(sender);
  *     IDEAL_SDEBUG("*** File " << sdr->uri().uri() << " exists? " << (exists ? "yes" : "no"));
  *     // or do something interesting using 'sender' (or 'sdr') and 'exists'
  * }
  * @endcode
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT File
    : public Object
{
public:
    File(const Uri &uri, Object *parent);
    virtual ~File();

    enum Permissions {
        NoPermissions      = 0,   ///< no permissions of any kind
        OwnerCanRead       = 1,   ///< the owner of this file can read it
        OwnerCanWrite      = 2,   ///< the owner of this file can write on it
        OwnerCanExecute    = 4,   ///< the owner of this file can execute it
        OwnerMask          = 7,   ///< masks the Permissions flags on the owner only
        GroupCanRead       = 8,   ///< the group of this file can read it
        GroupCanWrite      = 16,  ///< the group of this file can write on it
        GroupCanExecute    = 32,  ///< the group of this file can execute it
        GroupMask          = 56,  ///< masks the Permissions flags on the group only
        OthersCanRead      = 64,  ///< others can read this file
        OthersCanWrite     = 128, ///< others can write on this file
        OthersCanExecute   = 256, ///< others can execute this file
        OthersMask         = 448, ///< masks the Permissions flags on others only
        UnknownPermissions = 512  ///< unknown permissions
    };

    enum Type {
        UnknownType     = 0,  ///< this is an unknown type of file
        RegularFile     = 1,  ///< this is a regular file
        Directory       = 2,  ///< this is a directory
        CharacterDevice = 4,  ///< this is a character device
        BlockDevice     = 8,  ///< this is a block device
        Device          = 12, ///< this is a device, character or block one
        SymbolicLink    = 16, ///< this is a symbolic link
        Socket          = 32, ///< this is a socket
        Pipe            = 64  ///< this is a pipe
    };

    /**
      * @return A constructed concurrent object that will be able to run asynchronously, signaling
      *         existsResult, or error, depending if the operation could be executed successfully.
      *
      * Example:
      *
      * @code
      *      File f("ftp://ftp.myserver.com/path/to/my/file.txt", parent);
      *      connectMulti(f.existsResult, myObject, &MyObject::existsResult);
      *      connectMulti(f.error, myObject, &MyObject::error);
      *      Concurrent *exists = f.exists();
      *      exists->exec();
      * @endcode
      *
      * If you want this same task to be executed synchronously (what is strongly discouraged in
      * general):
      *
      * @code
      *      File f("ftp://ftp.myserver.com/path/to/my/file.txt", parent);
      *      connectMulti(f.existsResult, myObject, &MyObject::existsResult);
      *      connectMulti(f.error, myObject, &MyObject::error);
      *      Concurrent *exists = f.exists();
      *      exists->exec();
      *      exists->join();
      * @endcode
      */
    Concurrent *exists(Concurrent::Type type = Concurrent::Joinable) const;

    /**
      * @return A constructed concurrent object that will be able to run asynchronously, signaling
      *         typeResult, or error, depending if the operation could be executed successfully.
      */
    Concurrent *type(Concurrent::Type type = Concurrent::Joinable) const;

    /**
      * @return A constructed concurrent object that will be able to run asynchronously, signaling
      *         ownerUserResult, or error, depending if the operation could be executed successfully.
      */
    Concurrent *ownerUser(Concurrent::Type type = Concurrent::Joinable) const;

    /**
      * @return A constructed concurrent object that will be able to run asynchronously, signaling
      *         ownerGroupResult, or error, depending if the operation could be executed successfully.
      */
    Concurrent *ownerGroup(Concurrent::Type type = Concurrent::Joinable) const;

    /**
      * @return A constructed concurrent object that will be able to run asynchronously, signaling
      *         permissionsResult, or error, depending if the operation could be executed successfully.
      */
    Concurrent *permissions(Concurrent::Type type = Concurrent::Joinable) const;

    /**
      * @return A constructed concurrent object that will be able to run asynchronously, signaling
      *         sizeResult, or error, depending if the operation could be executed successfully.
      */
    Concurrent *size(Concurrent::Type type = Concurrent::Joinable) const;

    /**
      * @return A constructed concurrent object that will be able to run asynchronously, signaling
      *         contentTypeResult, or error, depending if the operation could be executed successfully.
      */
    Concurrent *contentType(Concurrent::Type type = Concurrent::Joinable) const;

    /**
      * @return The URI of the file object.
      */
    Uri uri() const;

public:
    /**
      * Whether this file exists or not.
      */
    IDEAL_SIGNAL(existsResult, bool);

    /**
      * The type of this file.
      *
      * @see Type
      */
    IDEAL_SIGNAL(typeResult, Type);

    /**
      * The owner user of this file.
      */
    IDEAL_SIGNAL(ownerUserResult, String);

    /**
      * The owner group of this file.
      */
    IDEAL_SIGNAL(ownerGroupResult, String);

    /**
      * Permissions of this file.
      *
      * @see Permissions
      */
    IDEAL_SIGNAL(permissionsResult, Permissions);

    /**
      * Size of this file.
      */
    IDEAL_SIGNAL(sizeResult, double);

    /**
      * The content type of this file.
      */
    IDEAL_SIGNAL(contentTypeResult, String);

    /**
      * If for any reason, the request could not finish correctly, this signal will be emitted with
      * the proper error code set.
      *
      * @see ProtocolHandler::ErrorCode
      */
    IDEAL_SIGNAL(error, ProtocolHandler::ErrorCode);

private:
    void statResultSlot(ProtocolHandler::StatResult statResult);

    class Private;
    Private *const d;
};

}

#endif //FILE_H
