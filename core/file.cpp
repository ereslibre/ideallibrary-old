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

#include "file.h"
#include "private/file_p.h"

#include <core/module.h>
#include <core/extension_loader.h>

#include <core/application.h>
#include <core/private/application_p.h>

#define PH_CACHE_SIZE 10

namespace IdealCore {

File::Private::Private(File *q)
    : m_errorCode(ProtocolHandler::Unknown)
    , m_stated(false)
    , m_exists(false)
    , m_type(UnknownType)
    , m_permissions(UnknownPermissions)
    , q(q)
{
}

class File::Private::Job
    : public Thread
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

File::Private::Job::Job(File *file, Type type)
    : Thread(type)
    , m_file(file)
    , m_protocolHandler(0)
{
}

ProtocolHandler *File::Private::Job::findProtocolHandler()
{
    Application::Private *const app_d = m_file->application()->d;
    List<ProtocolHandler*>::iterator it;
    {
        ContextMutexLocker cml(app_d->m_protocolHandlerCacheMutex);
        for (it = app_d->m_protocolHandlerCache.begin(); it != app_d->m_protocolHandlerCache.end(); ++it) {
            ProtocolHandler *protocolHandler = *it;
            if (protocolHandler->canBeReusedWith(m_file->d->m_uri)) {
                app_d->m_protocolHandlerCache.erase(it);
                ++protocolHandler->m_weight;
                return protocolHandler;
            }
        }
    }
    ProtocolHandler *res = 0;
    Module *module = ExtensionLoader::loadModule("libbuiltinprotocolhandlers.so", m_file);
    List<Module::ExtensionInfo> extensionList = module->extensionInfoList();
    List<Module::ExtensionInfo>::iterator extIt;
    for (extIt = extensionList.begin(); extIt != extensionList.end(); ++extIt) {
        Module::ExtensionInfo extensionInfo = *extIt;
        if (!extensionInfo.componentOwner.compare("ideallibrary")) {
            ProtocolHandler::AdditionalInfo *additionalInfo = static_cast<ProtocolHandler::AdditionalInfo*>(extensionInfo.additionalInfo);
            if (additionalInfo->handlesProtocols.contains(m_file->d->m_uri.scheme())) {
                res = ExtensionLoader::loadExtension<ProtocolHandler>(module, extensionInfo.entryPoint, m_file);
                break;
            }
        }
    }
    return res;
}

void File::Private::Job::cacheOrDiscard(ProtocolHandler *protocolHandler)
{
    Application::Private *const app_d = m_file->application()->d;
    ContextMutexLocker cml(app_d->m_protocolHandlerCacheMutex);
    if (app_d->m_protocolHandlerCache.size() < PH_CACHE_SIZE) {
        app_d->m_protocolHandlerCache.push_back(protocolHandler);
    } else {
        app_d->m_protocolHandlerCache.sort(LessThanProtocolHandler());
        ProtocolHandler *const protocolHandlerCachedWithLessWeight = app_d->m_protocolHandlerCache.front();
        app_d->m_protocolHandlerCache.erase(app_d->m_protocolHandlerCache.begin());
        delete protocolHandlerCachedWithLessWeight;
        ++protocolHandler->m_weight;
        app_d->m_protocolHandlerCache.push_front(protocolHandler);
    }
}

void File::Private::Job::fetchInfo()
{
    m_protocolHandler = findProtocolHandler();
    if (m_protocolHandler) {
        connect(m_protocolHandler->statResult, m_file, &File::statResultSlot);
        m_protocolHandler->stat(m_file->d->m_uri);
        disconnect(m_protocolHandler->statResult, m_file, &File::statResultSlot);
    } else {
        IDEAL_DEBUG_WARNING("currently there are no installed extensions capable of handling \"" << m_file->d->m_uri.scheme() << "\" protocol");
    }
}

bool File::Private::Job::LessThanProtocolHandler::operator()(ProtocolHandler *&left, ProtocolHandler *&right)
{
    return left->m_weight < right->m_weight;
}

void File::Private::Job::run()
{
    bool wasStated = true;
    if (!m_file->d->m_stated) {
        fetchInfo();
        if (!m_protocolHandler) {
            return;
        }
        wasStated = false;
        m_file->d->m_stated = true;
    }
    if (m_file->d->m_errorCode != ProtocolHandler::NoError) {
        if (m_operation == FileExists && m_file->d->m_errorCode == ProtocolHandler::FileNotFound) {
            m_file->emit(m_file->existsResult, false);
        } else {
            m_file->emit(m_file->error, m_file->d->m_errorCode);
        }
        delete m_protocolHandler;
        return;
    }
    if (!wasStated) {
        cacheOrDiscard(m_protocolHandler);
        m_protocolHandler = 0;
    }
    switch (m_operation) {
        case FileExists:
            m_file->emit(m_file->existsResult, m_file->d->m_exists);
            break;
        case FileType:
            m_file->emit(m_file->typeResult, m_file->d->m_type);
            break;
        case FileOwnerUser:
            m_file->emit(m_file->ownerUserResult, m_file->d->m_ownerUser);
            break;
        case FileOwnerGroup:
            m_file->emit(m_file->ownerGroupResult, m_file->d->m_ownerGroup);
            break;
        case FilePermissions:
            m_file->emit(m_file->permissionsResult, m_file->d->m_permissions);
            break;
        case FileSize:
            m_file->emit(m_file->sizeResult, m_file->d->m_size);
            break;
        case FileContentType:
            m_file->emit(m_file->contentTypeResult, m_file->d->m_contentType);
            break;
        default:
            IDEAL_DEBUG_WARNING("invalid operation");
            break;
    }
}

File::File(const Uri &uri, Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(existsResult, bool)
    , IDEAL_SIGNAL_INIT(typeResult, Type)
    , IDEAL_SIGNAL_INIT(ownerUserResult, String)
    , IDEAL_SIGNAL_INIT(ownerGroupResult, String)
    , IDEAL_SIGNAL_INIT(permissionsResult, Permissions)
    , IDEAL_SIGNAL_INIT(sizeResult, double)
    , IDEAL_SIGNAL_INIT(contentTypeResult, String)
    , IDEAL_SIGNAL_INIT(error, ProtocolHandler::ErrorCode)
    , d(new Private(this))
{
    setDeleteChildrenRecursively(false);
    d->m_uri = uri;
}

File::~File()
{
    delete d;
}

Thread *File::exists(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FileExists;
    return job;
}

Thread *File::type(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FileType;
    return job;
}

Thread *File::ownerUser(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FileOwnerUser;
    return job;
}

Thread *File::ownerGroup(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FileOwnerGroup;
    return job;
}

Thread *File::permissions(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FilePermissions;
    return job;
}

Thread *File::size(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FileSize;
    return job;
}

Thread *File::contentType(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FileContentType;
    return job;
}

Uri File::uri() const
{
    return d->m_uri;
}

void File::statResultSlot(ProtocolHandler::StatResult statResult)
{
    d->m_errorCode = statResult.errorCode;
    d->m_exists = statResult.exists;
    d->m_type = (Type) statResult.type;
    d->m_ownerUser = statResult.ownerUser;
    d->m_ownerGroup = statResult.ownerGroup;
    d->m_permissions = (Permissions) statResult.permissions;
    d->m_size = statResult.size;
    d->m_contentType = statResult.contentType;
}

}
