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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include <core/file.h>

namespace IdealCore {

class BuiltinProtocolHandlersLocal::Private
{
public:
    StatResult stat(const Uri &uri)
    {
        StatResult statRes;
        {
            struct stat statResult;
            statRes.errorCode = ProtocolHandler::Unknown;
            statRes.exists = false;
            statRes.type = File::UnknownType;
            statRes.ownerUser = String();
            statRes.ownerGroup = String();
            statRes.permissions = File::UnknownPermissions;
            statRes.size = 0;
            statRes.lastAccessed = -1;
            statRes.lastModified = -1;
            statRes.contentType = String();
            statRes.uri = uri;
            if (!::stat(uri.path().data(), &statResult)) {
                statRes.errorCode = ProtocolHandler::NoError;
                statRes.exists = true;
                statRes.type = File::UnknownType;
                if (S_ISREG(statResult.st_mode)) {
                    statRes.type |= File::RegularFile;
                } else if (S_ISDIR(statResult.st_mode)) {
                    statRes.type |= File::Directory;
                } else if (S_ISCHR(statResult.st_mode)) {
                    statRes.type |= File::CharacterDevice;
                } else if (S_ISBLK(statResult.st_mode)) {
                    statRes.type |= File::BlockDevice;
                } else if (S_ISLNK(statResult.st_mode)) {
                    statRes.type |= File::SymbolicLink;
                } else if (S_ISSOCK(statResult.st_mode)) {
                    statRes.type |= File::Socket;
                } else if (S_ISFIFO(statResult.st_mode)) {
                    statRes.type |= File::Pipe;
                }
                const struct passwd *const password = getpwuid(statResult.st_uid);
                if (password) {
                    statRes.ownerUser = password->pw_name;
                }
                const struct group *const gr = getgrgid(statResult.st_gid);
                if (gr) {
                    statRes.ownerGroup = gr->gr_name;
                }
                statRes.permissions = File::NoPermissions;
                const mode_t mode = statResult.st_mode;
                if (mode & S_IRUSR) {
                    statRes.permissions |= File::OwnerCanRead;
                }
                if (mode & S_IWUSR) {
                    statRes.permissions |= File::OwnerCanWrite;
                }
                if (mode & S_IXUSR) {
                    statRes.permissions |= File::OwnerCanExecute;
                }
                if (mode & S_IRGRP) {
                    statRes.permissions |= File::GroupCanRead;
                }
                if (mode & S_IWGRP) {
                    statRes.permissions |= File::GroupCanWrite;
                }
                if (mode & S_IXGRP) {
                    statRes.permissions |= File::GroupCanExecute;
                }
                if (mode & S_IROTH) {
                    statRes.permissions |= File::OthersCanRead;
                }
                if (mode & S_IWOTH) {
                    statRes.permissions |= File::OthersCanWrite;
                }
                if (mode & S_IXOTH) {
                    statRes.permissions |= File::OthersCanExecute;
                }
                statRes.size = statResult.st_size;
                statRes.lastAccessed = statResult.st_atime;
                statRes.lastModified = statResult.st_mtime;
            } else {
                switch (errno) {
                    case ENOENT:
                    case ENOTDIR:
                        statRes.errorCode = ProtocolHandler::FileNotFound;
                        break;
                    case EACCES:
                        statRes.errorCode = ProtocolHandler::InsufficientPermissions;
                        break;
                    default:
                        break;
                }
            }
        }
        return statRes;
    }
};

BuiltinProtocolHandlersLocal::BuiltinProtocolHandlersLocal()
    : d(new Private)
{
}

BuiltinProtocolHandlersLocal::~BuiltinProtocolHandlersLocal()
{
    delete d;
}

void BuiltinProtocolHandlersLocal::cd(const Uri &uri)
{
    if (chdir(uri.path().data())) {}
}

void BuiltinProtocolHandlersLocal::mkdir(const Uri &uri)
{
    ::mkdir(uri.path().data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void BuiltinProtocolHandlersLocal::rm(const Uri &uri)
{
    unlink(uri.path().data());
}

void BuiltinProtocolHandlersLocal::stat(const Uri &uri)
{
    emit(statResult, d->stat(uri));
}

bool BuiltinProtocolHandlersLocal::canBeReusedWith(const Uri &uri) const
{
    return uri.scheme().empty() || !uri.scheme().compare("file");
}

}

EXTENSION_ENTRY_POINT(builtinProtocolHandlersLocal, IdealCore::BuiltinProtocolHandlersLocal)
