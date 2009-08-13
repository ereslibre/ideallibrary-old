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
#include <core/thread.h>
#include <core/interfaces/protocol_handler.h>

namespace IdealCore {

class Thread;

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
  *     Thread *const size = file->size();
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
        UnknownPermissions = 512  ///< Unknown permissions
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

    enum Event {
        NoEvent              = 0,
        AccessEvent          = 1,
        AttributeChangeEvent = 2,
        CloseEvent           = 4,
        CreateEvent          = 8,
        DeleteEvent          = 16,
        ModifyEvent          = 32,
        MoveEvent            = 64,
        OpenEvent            = 128,
        AllEvent             = AccessEvent | AttributeChangeEvent | CloseEvent | CreateEvent |
                               DeleteEvent | ModifyEvent | MoveEvent | OpenEvent
    };

    struct EventNotify {
        int event;
        Uri uri;
    };

    /**
      * Track events @p events on this file.
      */
    void setTrackEvents(Event events);

    Event trackEvents() const;

    Thread *stat(Thread::Type type = Thread::NoJoinable) const;

    /**
      * @param maxBytes If set to ProtocolHandler::NoMaxBytes, it will be attempted to retrieve the full
      *        file. Otherwise when @p maxBytes bytes are reached, no more chunks will be requested.
      *
      * @return A constructed thread object that will be able to run asynchronously, signaling dataRead
      *         several times until the file has been reached EOF or it has been cancelled, or maxBytes
      *         has been reached.
      *
      * @see ProtocolHandler::get()
      */
    Thread *get(double maxBytes = ProtocolHandler::NoMaxBytes, Thread::Type type = Thread::NoJoinable) const;

    /**
      * @return The URI of the file object.
      */
    Uri uri() const;

public:
    /**
      * The stat result.
      */
    IDEAL_SIGNAL(statResult, ProtocolHandler::StatResult);

    /**
      * The data ready to be read.
      */
    IDEAL_SIGNAL(dataRead, ByteStream);

    /**
      * The contents of a directory.
      */
    IDEAL_SIGNAL(dirRead, List<Uri>);

    IDEAL_SIGNAL(event, EventNotify);

    /**
      * If for any reason, the request could not finish correctly, this signal will be emitted with
      * the proper error code set.
      *
      * @see ProtocolHandler::ErrorCode
      */
    IDEAL_SIGNAL(error, ProtocolHandler::ErrorCode);

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //FILE_H
