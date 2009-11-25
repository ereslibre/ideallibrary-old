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

#ifndef APPLICATION_P_H
#define APPLICATION_P_H

#include <vector>
#include <core/option.h>

namespace IdealCore {

class Timer;
class Module;
class ProtocolHandler;

class Application::Private
{
public:
    Private(Application *q);
    virtual ~Private();

    void processEvents();
    void processDelayedDeletions();
    void checkFileWatches();
    void unloadUnneededDynamicLibraries();

    static bool timerSort(const Timer *left, const Timer *right);
    void checkTimers();

    iint32                   m_argc;
    ichar                  **m_argv;
    String                   m_name;
    bool                     m_prefixSet;
    iint32                   m_sleepTime;
    const iint32             m_defaultSleepTime;
    List<Object*>            m_markedForDeletion;
    Mutex                    m_markedForDeletionMutex;
    List<IdealCore::Module*> m_markedForUnload;
    Mutex                    m_markedForUnloadMutex;
    std::vector<Timer*>      m_runningTimerList;
    Mutex                    m_runningTimerListMutex;
    iint32                   m_nextTimeout;
    List<ProtocolHandler*>   m_protocolHandlerCache;
    Mutex                    m_protocolHandlerCacheMutex;
    Application             *q;
};

}

#ifdef IDEAL_OS_POSIX
#include <core/private/posix/application_p.h>
#endif //IDEAL_OS_POSIX

#endif //APPLICATION_P_H
