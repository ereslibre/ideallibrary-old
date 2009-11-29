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

#include "module.h"
#include "private/module_p.h"

namespace IdealCore {

List<Module::ExtensionInfo> Module::Private::FakeModule::extensionInfoList()
{
    return List<ExtensionInfo>();
}

Module::Private::Private(Module *q)
    : m_unused(true)
    , m_refs(0)
    , q(q)
{
}

Module::Private::~Private()
{
    if (m_unused) {
        FakeModule *fakeModule = new FakeModule;
        fakeModule->d->m_handle = m_handle;
        ContextMutexLocker cml(m_application->d->m_markedForUnloadMutex);
        m_application->d->m_markedForUnload.push_back(fakeModule);
    }
}

void Module::Private::deref()
{
    List<Module*>::iterator it;
    ContextMutexLocker cml(m_application->d->m_markedForUnloadMutex);
    for (it = m_application->d->m_markedForUnload.begin(); it != m_application->d->m_markedForUnload.end(); ++it) {
        if (*it == q) {
            return;
        }
    }
    if (!--m_refs) {
        m_application->d->m_markedForUnload.push_back(q);
    }
}

Module::Module()
    : d(new Private(this))
{
}

Module::~Module()
{
    delete d;
}

String Module::path() const
{
    return d->m_path;
}

bool Module::isUnused() const
{
    return d->m_unused;
}

}
