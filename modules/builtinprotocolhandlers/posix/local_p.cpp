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

#include "../local.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include <core/file.h>

namespace IdealCore {

class BuiltinProtocolHandlersLocal::Private
{
public:
    Private(BuiltinProtocolHandlersLocal *q)
        : m_fd(-1)
        , q(q)
    {
    }

    bool                          m_success;
    Uri                           m_opened;
    int                           m_fd;
    BuiltinProtocolHandlersLocal *q;
};

BuiltinProtocolHandlersLocal::BuiltinProtocolHandlersLocal()
    : d(new Private(this))
{
}

BuiltinProtocolHandlersLocal::~BuiltinProtocolHandlersLocal()
{
    delete d;
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersLocal::open(const Uri &uri, int openMode)
{
    if (d->m_opened.isValid()) {
        IDEAL_DEBUG_WARNING("the uri " << d->m_opened.uri() << " was opened. Closing");
        close();
    }
    d->m_opened = uri;
    int oflag = 0;
    if ((openMode & Read) && (openMode & Write)) {
        oflag = O_RDWR;
    } else if (openMode & Read) {
        oflag = O_RDONLY;
    } else {
        oflag = O_WRONLY;
    }
    d->m_fd = ::open(uri.path().data(), oflag);
    if (d->m_fd > -1) {
        return NoError;
    }
    switch (errno) {
        case EACCES:
            return InsufficientPermissions;
            break;
        default:
            return UnknownError;
            break;
    }
}

ByteStream BuiltinProtocolHandlersLocal::read(unsigned int nbytes)
{
    if (d->m_fd == -1) {
        return ByteStream();
    }
    char resBuffer[nbytes];
    const size_t bytesRead = ::read(d->m_fd, resBuffer, nbytes);
    if (bytesRead) {
        return ByteStream(resBuffer, bytesRead);
    }
    return ByteStream();
}

unsigned int BuiltinProtocolHandlersLocal::write(const ByteStream &byteStream)
{
    if (d->m_fd == -1) {
        return 0;
    }
    return ::write(d->m_fd, byteStream.data(), byteStream.size());
}

void BuiltinProtocolHandlersLocal::close()
{
    if (d->m_fd > 0) {
        ::close(d->m_fd);
        d->m_fd = -1;
    }
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersLocal::mkdir(const Uri &uri, Permissions permissions)
{
    mode_t mode = 0;
    if (permissions == SystemDefault) {
        const mode_t mask = umask(0);
        umask(mask);
        mode = 0777 & ~mask;
    } else {
        if (permissions & OwnerCanRead) {
            mode |= 0400;
        }
        if (permissions & OwnerCanWrite) {
            mode |= 0200;
        }
        if (permissions & OwnerCanExecute) {
            mode |= 0100;
        }
        if (permissions & GroupCanRead) {
            mode |= 0040;
        }
        if (permissions & GroupCanWrite) {
            mode |= 0020;
        }
        if (permissions & GroupCanExecute) {
            mode |= 0010;
        }
        if (permissions & OthersCanRead) {
            mode |= 0004;
        }
        if (permissions & OthersCanWrite) {
            mode |= 0002;
        }
        if (permissions & OthersCanExecute) {
            mode |= 0001;
        }
    }
    if (::mkdir(uri.path().data(), mode)) {
        switch (errno) {
            case EACCES:
                return InsufficientPermissions;
                break;
            default:
                return UnknownError;
                break;
        }
    }
    return NoError;
}

ProtocolHandler::ErrorCode BuiltinProtocolHandlersLocal::rm(const Uri &uri)
{
    if (::unlink(uri.path().data())) {
        switch (errno) {
            case EACCES:
                return InsufficientPermissions;
                break;
            default:
                return UnknownError;
                break;
        }
    }
    return NoError;
}

ProtocolHandler::StatResult BuiltinProtocolHandlersLocal::stat(const Uri &uri)
{
    d->m_success = false;
    StatResult statRes;
    statRes.uri = uri;
    statRes.errorCode = NoError;
    {
        struct stat statResult;
        if (!::stat(uri.path().data(), &statResult)) {
            statRes.exists = true;
            statRes.type = ProtocolHandler::UnknownType;
            if (S_ISREG(statResult.st_mode)) {
                statRes.type |= ProtocolHandler::RegularFile;
            } else if (S_ISDIR(statResult.st_mode)) {
                statRes.type |= ProtocolHandler::Directory;
            } else if (S_ISCHR(statResult.st_mode)) {
                statRes.type |= ProtocolHandler::CharacterDevice;
            } else if (S_ISBLK(statResult.st_mode)) {
                statRes.type |= ProtocolHandler::BlockDevice;
            } else if (S_ISLNK(statResult.st_mode)) {
                statRes.type |= ProtocolHandler::SymbolicLink;
            } else if (S_ISSOCK(statResult.st_mode)) {
                statRes.type |= ProtocolHandler::Socket;
            } else if (S_ISFIFO(statResult.st_mode)) {
                statRes.type |= ProtocolHandler::Pipe;
            }
            const struct passwd *const password = getpwuid(statResult.st_uid);
            if (password) {
                statRes.ownerUser = password->pw_name;
            }
            const struct group *const gr = getgrgid(statResult.st_gid);
            if (gr) {
                statRes.ownerGroup = gr->gr_name;
            }
            statRes.permissions = ProtocolHandler::NoPermissions;
            const mode_t mode = statResult.st_mode;
            if (mode & S_IRUSR) {
                statRes.permissions |= ProtocolHandler::OwnerCanRead;
            }
            if (mode & S_IWUSR) {
                statRes.permissions |= ProtocolHandler::OwnerCanWrite;
            }
            if (mode & S_IXUSR) {
                statRes.permissions |= ProtocolHandler::OwnerCanExecute;
            }
            if (mode & S_IRGRP) {
                statRes.permissions |= ProtocolHandler::GroupCanRead;
            }
            if (mode & S_IWGRP) {
                statRes.permissions |= ProtocolHandler::GroupCanWrite;
            }
            if (mode & S_IXGRP) {
                statRes.permissions |= ProtocolHandler::GroupCanExecute;
            }
            if (mode & S_IROTH) {
                statRes.permissions |= ProtocolHandler::OthersCanRead;
            }
            if (mode & S_IWOTH) {
                statRes.permissions |= ProtocolHandler::OthersCanWrite;
            }
            if (mode & S_IXOTH) {
                statRes.permissions |= ProtocolHandler::OthersCanExecute;
            }
            statRes.size = statResult.st_size;
            statRes.lastAccessed = statResult.st_atime;
            statRes.lastModified = statResult.st_mtime;
            d->m_success = true;
        } else {
            switch (errno) {
                case ENOENT:
                case ENOTDIR:
                    break;
                case EACCES:
                    statRes.errorCode = InsufficientPermissions;
                    break;
                default:
                    statRes.errorCode = UnknownError;
                    break;
            }
        }
    }
    return statRes;
}

bool BuiltinProtocolHandlersLocal::canBeReusedWith(const Uri &uri) const
{
    return uri.scheme().empty() || !uri.scheme().compare("file");
}

}

EXTENSION_ENTRY_POINT(builtinProtocolHandlersLocal, IdealCore::BuiltinProtocolHandlersLocal)
