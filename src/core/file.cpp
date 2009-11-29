/*
 * This file is part of the Ideal Library
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
#define BUFFER_SIZE (1024 * 32)

namespace IdealCore {

File::Private::Private(File *q)
    : m_events(NoEvent)
    , q(q)
{
}

File::Private::~Private()
{
}

class File::Private::Job
    : public Thread
{
public:
    enum Operation {
        Stat = 0,
        Get,
        Mkdir
    };

    Job(File *file, Type type);

    ProtocolHandler *findProtocolHandler();
    void cacheOrDiscard(ProtocolHandler *protocolHandler);

    ProtocolHandler::StatResult statPrivate();

    void stat();
    void get();
    void mkdir();

    void readDir();
    void readFile();

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

    File                        *m_file;
    Operation                    m_operation;
    size_t                       m_maxBytes;
    ProtocolHandler::Permissions m_permissions;
    ProtocolHandler             *m_protocolHandler;

protected:
    virtual void run();
};

File::Private::Job::Job(File *file, Type type)
    : Thread(type)
    , m_file(file)
    , m_maxBytes(NoMaxBytes)
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
    ProtocolHandler *res = ExtensionLoader::findFirstExtension<ProtocolHandler>(new ExtensionLoadDecider(m_file), m_file);
    if (!res) {
        IDEAL_DEBUG_WARNING("currently there are no installed extensions capable of handling \"" << m_file->d->m_uri.scheme() << "\" protocol");
    }
    return res;
}

void File::Private::Job::cacheOrDiscard(ProtocolHandler *protocolHandler)
{
    if (!protocolHandler) {
        return;
    }
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

ProtocolHandler::StatResult File::Private::Job::statPrivate()
{
    ProtocolHandler::StatResult res;
    m_protocolHandler = findProtocolHandler();
    if (m_protocolHandler) {
        res = m_protocolHandler->stat(m_file->d->m_uri);
    }
    return res;
}

void File::Private::Job::stat()
{
    emit(m_file->statResult, statPrivate());
    cacheOrDiscard(m_protocolHandler);
}

void File::Private::Job::get()
{
    const ProtocolHandler::StatResult statResult = statPrivate();
    if (statResult.errorCode != ProtocolHandler::NoError) {
        emit(m_file->error, statResult.errorCode);
        return;
    }
    if (statResult.type == ProtocolHandler::Directory) {
        readDir();
    } else {
        readFile();
    }
    cacheOrDiscard(m_protocolHandler);
}

void File::Private::Job::mkdir()
{
    m_protocolHandler = findProtocolHandler();
    if (m_protocolHandler) {
        m_protocolHandler->mkdir(m_file->d->m_uri, m_permissions);
        cacheOrDiscard(m_protocolHandler);
    }
}

void File::Private::Job::readDir()
{
    emit(m_file->dirRead, m_protocolHandler->listDir(m_file->d->m_uri));
}

void File::Private::Job::readFile()
{
    m_protocolHandler->open(m_file->d->m_uri, ProtocolHandler::Read);
    size_t bytesRead = 0;
    while (m_maxBytes == NoMaxBytes || bytesRead < m_maxBytes) {
        const ByteStream byteStream = m_protocolHandler->read(BUFFER_SIZE);
        if (!byteStream.size()) {
            break;
        }
        bytesRead += byteStream.size();
        emit(m_file->dataRead, byteStream);
    }
    m_protocolHandler->close();
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
    switch (m_operation) {
        case Stat:
            stat();
            break;
        case Get:
            get();
            break;
        case Mkdir:
            mkdir();
            break;
        default:
            IDEAL_DEBUG_WARNING("unknown job operation");
            break;
    }
}

File::File(const Uri &uri, Object *parent)
    : Object(parent)
    , IDEAL_SIGNAL_INIT(statResult, ProtocolHandler::StatResult)
    , IDEAL_SIGNAL_INIT(dataRead, ByteStream)
    , IDEAL_SIGNAL_INIT(dirRead, List<Uri>)
    , IDEAL_SIGNAL_INIT(event, EventNotify)
    , IDEAL_SIGNAL_INIT(error, ProtocolHandler::ErrorCode)
    , d(new PrivateImpl(this))
{
    setDeleteChildrenRecursively(false);
    d->m_uri = uri;
}

File::~File()
{
    delete d;
}

File::Event File::trackEvents() const
{
    return d->m_events;
}

Thread *File::stat(Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::Stat;
    return job;
}

Thread *File::get(size_t maxBytes, Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::Get;
    job->m_maxBytes = maxBytes;
    return job;
}

Thread *File::mkdir(ProtocolHandler::Permissions permissions, Thread::Type type) const
{
    Private::Job *job = new Private::Job(const_cast<File*>(this), type);
    job->m_operation = Private::Job::Mkdir;
    job->m_permissions = permissions;
    return job;
}

Uri File::uri() const
{
    return d->m_uri;
}

}
