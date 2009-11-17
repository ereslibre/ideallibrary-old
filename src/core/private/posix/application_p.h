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

#ifndef APPLICATION_P_H_POSIX
#define APPLICATION_P_H_POSIX

#include <map>
#include <getopt.h>

#include <core/file.h>

#include <core/private/application_p.h>

namespace IdealCore {

class Application::PrivateImpl
    : public Application::Private
{
public:
    PrivateImpl(Application *q);
    virtual ~PrivateImpl();

    struct OptionItem
    {
        struct option opt;
        Option *option;
    };

    struct InotifyEvent {
        File             *file;
        File::EventNotify eventNotify;
    };

    List<OptionItem>     m_optionList;
#ifdef HAVE_INOTIFY
    bool                 m_inotifyStarted;
    iint32               m_inotify;
    std::map<int, File*> m_inotifyMap;
#endif
};

}

#endif //APPLICATION_P_H_POSIX

