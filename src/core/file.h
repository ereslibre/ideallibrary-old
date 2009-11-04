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
  *     connectMulti(file->statResult, this, &MyObject::statResultSlot);
  *     Thread *const stat = file->stat();
  *     stat->exec();
  * }
  *
  * void MyObject::statResultSlot(Object *sender, const ProtocolHandler::StatResult &statResult)
  * {
  *     File *const sdr = static_cast<File*>(sender);
  *     IDEAL_SDEBUG("*** File " << sdr->uri().uri() << " exists? " << (statResult.errorCode == ProtocolHandler::NoError ? "yes" : "no"));
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

    static const iuint64 NoMaxBytes = 0;

    enum Event {
        NoEvent              = 0,   ///< Do not track events for this file
        AccessEvent          = 1,   ///< Track access events for this file
        AttributeChangeEvent = 2,   ///< Track attribute change events for this file
        CloseEvent           = 4,   ///< Track close events for this file
        CreateEvent          = 8,   ///< Track create events for this file
        DeleteEvent          = 16,  ///< Track delete events for this file
        ModifyEvent          = 32,  ///< Track modify events for this file
        MoveEvent            = 64,  ///< Track move events for this file
        OpenEvent            = 128, ///< Track open events for this file
        AllEvent             = AccessEvent | AttributeChangeEvent | CloseEvent | CreateEvent |
                               DeleteEvent | ModifyEvent | MoveEvent | OpenEvent ///< Track all events
                                                                                 ///< for this file
    };

    struct EventNotify {
        iint32 event; ///< An OR bitfield with the events received. @see File::Event
        Uri    uri;   ///< The full URI of the file that did notify the event. Note that if you are tracking
                      ///< a directory, all files under the directory are being tracked also, and this uri
                      ///< would contain the full URI of the file or folder that notified the event.
    };

    /**
      * Track events @p events on this file or directory.
      *
      * When an event has happened on this file, or on any file below this directory (if this file
      * is a directory), the event signal will be emitted.
      */
    void setTrackEvents(Event events);

    /**
      * @return The events being tracked for this file or directory.
      */
    Event trackEvents() const;

    /**
      * @return A constructed thread object that will be able to run asynchronously, signaling statResult
      *         when the operation has finished.
      *
      * @note If something goes wrong, error signal will not be emitted. statResult signal will be emitted
      *       with the corresponding errorCode properly set.
      *
      * @see ProtocolHandler::stat()
      */
    Thread *stat(Thread::Type type = Thread::NoJoinable) const;

    /**
      * @param maxBytes If set to NoMaxBytes, it will be attempted to retrieve the full file. Otherwise when
      *        @p maxBytes bytes are reached, no more chunks will be requested.
      *
      * @return A constructed thread object that will be able to run asynchronously, signaling dataRead
      *         several times until the file has been reached EOF or it has been cancelled, or maxBytes
      *         has been reached.
      *
      * @see ProtocolHandler::get()
      */
    Thread *get(iuint64 maxBytes = NoMaxBytes, Thread::Type type = Thread::NoJoinable) const;

    /**
      * @return A constructed thread object that will be able to run asynchronously.
      *
      * @see ProtocolHandler::mkdir()
      */
    Thread *mkdir(ProtocolHandler::Permissions permissions = ProtocolHandler::SystemDefault, Thread::Type type = Thread::NoJoinable) const;

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

    /**
      * An event was notified.
      */
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
