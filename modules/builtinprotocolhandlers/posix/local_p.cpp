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
        : q(q)
    {
    }

    void cp(const Uri &source, const Uri &target);

    void getDir(const Uri &uri);
    List<Uri> getDirEntries(const String &path);
    void getFile(const Uri &uri, double maxBytes);

    bool                          m_success;
    BuiltinProtocolHandlersLocal *q;
};

void BuiltinProtocolHandlersLocal::Private::cp(const Uri &source, const Uri &target)
{
    struct stat sourceStat;
    ::stat(source.path().data(), &sourceStat);
    const bool sourceIsDir = S_ISDIR(sourceStat.st_mode);

    IDEAL_SDEBUG("is dir " << sourceIsDir);

    if (sourceIsDir) {
        List<Uri> children = getDirEntries(source.path());
        List<Uri>::const_iterator it;
        const Uri newTarget(target.path(), source.filename());
        //q->mkdir(newTarget);
        IDEAL_SDEBUG("===");
        IDEAL_SDEBUG("creating dir " << newTarget.uri());
        for (it = children.begin(); it != children.end(); ++it) {
            const Uri uri = *it;
            if (source.contains(uri)) {
                continue;
            }
            IDEAL_SDEBUG("copying " << uri.uri() << " to " << newTarget.uri());
            cp(uri, newTarget);
        }
    } else {
    }
}

List<Uri> BuiltinProtocolHandlersLocal::Private::getDirEntries(const String &path)
{
    List<Uri> res;
    DIR *const dir = opendir(path.data());
    struct dirent *dirEntry;
    while ((dirEntry = readdir(dir))) {
        res.push_back(Uri(path, dirEntry->d_name));
    }
    closedir(dir);
    return res;
}

void BuiltinProtocolHandlersLocal::Private::getDir(const Uri &uri)
{
    emit(q->dirRead, getDirEntries(uri.path()));
}

void BuiltinProtocolHandlersLocal::Private::getFile(const Uri &uri, double maxBytes)
{
    double currentSize = 0;
    const int fd = open(uri.path().data(), O_RDONLY);
    ssize_t bytesRead;
    char *buf = new char[BUFSIZ];
    bzero(buf, BUFSIZ);
    while ((bytesRead = read(fd, buf, BUFSIZ)) > 0) {
        ByteStream res(buf);
        currentSize += bytesRead;
        emit(q->dataRead, res);
        if (maxBytes && currentSize >= maxBytes) {
            break;
        }
        bzero(buf, BUFSIZ);
    }
    close(fd);
    delete buf;
}

BuiltinProtocolHandlersLocal::BuiltinProtocolHandlersLocal()
    : d(new Private(this))
{
}

BuiltinProtocolHandlersLocal::~BuiltinProtocolHandlersLocal()
{
    delete d;
}

void BuiltinProtocolHandlersLocal::mkdir(const Uri &uri, Permissions permissions)
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
                emit(error, InsufficientPermissions);
                break;
            default:
                emit(error, Unknown);
                break;
        }
    }
}

void BuiltinProtocolHandlersLocal::cp(const Uri &source, const Uri &target, Behavior behavior)
{
    struct stat statResultSource;
    if (!::stat(source.path().data(), &statResultSource)) {
        const bool sourceIsDir = S_ISDIR(statResultSource.st_mode);
        //BEGIN: check problematic cases
        struct stat statResultTarget;
        const bool targetExists = !::stat(target.path().data(), &statResultTarget);
        if (targetExists && !S_ISDIR(statResultTarget.st_mode) && behavior == DoNotOverwriteTarget) {
            emit(error, FileAlreadyExists);
            return;
        } else if (!targetExists && sourceIsDir) {
            emit(error, FileNotFound);
            return;
        }
        //END: check problematic cases
        d->cp(source, target);
    } else {
        switch (errno) {
            case ENOENT:
            case ENOTDIR:
                emit(error, FileNotFound);
                break;
            case EACCES:
                emit(error, InsufficientPermissions);
                break;
            default:
                emit(error, Unknown);
                break;
        }
    }
}

void BuiltinProtocolHandlersLocal::mv(const Uri &source, const Uri &target)
{
    link(source.path().data(), target.path().data());
    unlink(source.path().data());
}

void BuiltinProtocolHandlersLocal::rm(const Uri &uri)
{
    unlink(uri.path().data());
}

ProtocolHandler::StatResult BuiltinProtocolHandlersLocal::stat(const Uri &uri)
{
    d->m_success = false;
    StatResult statRes;
    statRes.uri = uri;
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
                    d->m_success = true;
                    break;
                case EACCES:
                    emit(error, InsufficientPermissions);
                    break;
                default:
                    emit(error, Unknown);
                    break;
            }
        }
    }
    statRes.isValid = d->m_success;
    return statRes;
}

void BuiltinProtocolHandlersLocal::get(const Uri &uri, double maxBytes)
{
    struct stat statResult;
    if (!::stat(uri.path().data(), &statResult)) {
        if (S_ISDIR(statResult.st_mode)) {
            d->getDir(uri);
        } else {
            d->getFile(uri, maxBytes);
        }
    } else {
        switch (errno) {
            case ENOENT:
            case ENOTDIR:
                emit(error, FileNotFound);
                break;
            case EACCES:
                emit(error, InsufficientPermissions);
                break;
            default:
                emit(error, Unknown);
                break;
        }
    }
}

bool BuiltinProtocolHandlersLocal::canBeReusedWith(const Uri &uri) const
{
    return uri.scheme().empty() || !uri.scheme().compare("file");
}

}

EXTENSION_ENTRY_POINT(builtinProtocolHandlersLocal, IdealCore::BuiltinProtocolHandlersLocal)
