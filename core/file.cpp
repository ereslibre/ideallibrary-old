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
    : m_stated(false)
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
        FileContentType,
        FileGet
    };

    Job(File *file, Type type);

    ProtocolHandler *findProtocolHandler();
    void cacheOrDiscard(ProtocolHandler *protocolHandler);

    void fetchInfo();
    void get();

    struct LessThanProtocolHandler
    {
        bool operator()(ProtocolHandler *&left, ProtocolHandler *&right);
    };

    class ExtensionLoadDecider
        : public ExtensionLoader::ExtensionLoadDecider
    {
    public:
        ExtensionLoadDecider(File *file);

        virtual bool loadExtension(const Module::ExtensionInfo &extensionInfo) const;

    private:
        File *m_file;
    };

    File            *m_file;
    Operation        m_operation;
    double           m_maxBytes;
    ProtocolHandler *m_protocolHandler;

protected:
    virtual void run();
};

File::Private::Job::Job(File *file, Type type)
    : Thread(type)
    , m_file(file)
    , m_maxBytes(ProtocolHandler::NoMaxBytes)
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
    return ExtensionLoader::findFirstExtension<ProtocolHandler>(new ExtensionLoadDecider(m_file), m_file);
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
        if (m_operation == FileExists) {
            connect(m_protocolHandler->error, m_file, &File::errorSlotForExists);
        } else {
            connect(m_protocolHandler->error, m_file, &File::errorSlot);
        }
        m_protocolHandler->stat(m_file->d->m_uri);
        disconnect(m_protocolHandler->statResult, m_file, &File::statResultSlot);
        if (m_operation == FileExists) {
            disconnect(m_protocolHandler->error, m_file, &File::errorSlotForExists);
        } else {
            disconnect(m_protocolHandler->error, m_file, &File::errorSlot);
        }
    } else {
        IDEAL_DEBUG_WARNING("currently there are no installed extensions capable of handling \"" << m_file->d->m_uri.scheme() << "\" protocol");
    }
}

void File::Private::Job::get()
{
    m_protocolHandler = findProtocolHandler();
    if (m_protocolHandler) {
        connect(m_protocolHandler->dataRead, m_file->dataRead);
        connect(m_protocolHandler->error, m_file->error);
        m_protocolHandler->get(m_file->d->m_uri, m_maxBytes);
        disconnect(m_protocolHandler->dataRead, m_file->dataRead);
        disconnect(m_protocolHandler->error, m_file->error);
    } else {
        IDEAL_DEBUG_WARNING("currently there are no installed extensions capable of handling \"" << m_file->d->m_uri.scheme() << "\" protocol");
    }
}

bool File::Private::Job::LessThanProtocolHandler::operator()(ProtocolHandler *&left, ProtocolHandler *&right)
{
    return left->m_weight < right->m_weight;
}

File::Private::Job::ExtensionLoadDecider::ExtensionLoadDecider(File *file)
    : m_file(file)
{
}

bool File::Private::Job::ExtensionLoadDecider::loadExtension(const Module::ExtensionInfo &extensionInfo) const
{
    if (extensionInfo.componentOwner.compare("ideallibrary") ||
        extensionInfo.extensionType != Module::ProtocolHandler) {
        return false;
    }
    ProtocolHandler::AdditionalInfo *additionalInfo = static_cast<ProtocolHandler::AdditionalInfo*>(extensionInfo.additionalInfo);
    return additionalInfo->handlesProtocols.contains(m_file->d->m_uri.scheme());
}

void File::Private::Job::run()
{
    if (m_operation == FileGet) {
        get();
        return;
    }
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
    , IDEAL_SIGNAL_INIT(dataRead, ByteStream)
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

Thread *File::get(double maxBytes, Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::FileGet;
    job->m_maxBytes = maxBytes;
    return job;
}

Uri File::uri() const
{
    return d->m_uri;
}

void File::statResultSlot(ProtocolHandler::StatResult statResult)
{
    d->m_errorCode = ProtocolHandler::NoError;
    d->m_exists = statResult.exists;
    d->m_type = (Type) statResult.type;
    d->m_ownerUser = statResult.ownerUser;
    d->m_ownerGroup = statResult.ownerGroup;
    d->m_permissions = (Permissions) statResult.permissions;
    d->m_size = statResult.size;
    d->m_contentType = statResult.contentType;
}

void File::errorSlot(ProtocolHandler::ErrorCode errorCode)
{
    d->m_errorCode = errorCode;
    emit(error, errorCode);
}

void File::errorSlotForExists(ProtocolHandler::ErrorCode errorCode)
{
    d->m_errorCode = errorCode;
    if (errorCode == ProtocolHandler::FileNotFound) {
        emit(existsResult, false);
    } else {
        emit(error, errorCode);
    }
}

}
