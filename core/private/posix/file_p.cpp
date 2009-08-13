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

#include <sys/inotify.h>

#include <core/file.h>
#include "file_p.h"
#include <core/application.h>
#include "application_p.h"

namespace IdealCore {

File::PrivateImpl::PrivateImpl(File *q)
    : Private(q)
{
}

File::PrivateImpl::~PrivateImpl()
{
}

void File::trackEvents(Event events)
{
    if (events == d->m_events) {
        return;
    }
    Application::PrivateImpl *app_d = static_cast<Application::PrivateImpl*>(application()->d);
    if (!app_d->m_inotifyStarted) {
        if ((app_d->m_inotify = inotify_init()) > -1) {
            app_d->m_inotifyStarted = true;
        }
    }
}

}
