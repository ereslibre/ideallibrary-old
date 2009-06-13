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

    void addOptionWithoutArg(Option &option, char optChar, const char *longOpt);
    void addOptionWithMandatoryArg(Option &option, char optChar, const char *longOpt);
    void addOptionWithOptionalArg(Option &option, char optChar, const char *longOpt);

    List<char*> parseOptions(ParsingStrictness parsingStrictness, FailVerbosity failVerbosity);

    String getPath(Path path) const;

    void processEvents();
    void processDelayedDeletions();
    void unloadUnneededDynamicLibraries();

    void postEvent(Object *object, Event *event);

    void quit();

    static bool timerSort(const Timer *left, const Timer *right);
    void checkTimers();

    struct EventEntry {
        Object *object;
        Event  *event;
    };

    int                      m_argc;
    char                   **m_argv;
    int                      m_sleepTime;                   // Initialized in base class
    const int                m_defaultSleepTime;            // Initialized in base class
    List<Object*>            m_markedForDeletion;
    Mutex                    m_markedForDeletionMutex;
    List<IdealCore::Module*> m_markedForUnload;
    Mutex                    m_markedForUnloadMutex;
    std::vector<Timer*>      m_runningTimerList;
    Mutex                    m_runningTimerListMutex;
    int                      m_nextTimeout;                 // Initialized in base class
    List<ProtocolHandler*>   m_protocolHandlerCache;
    Mutex                    m_protocolHandlerCacheMutex;
    List<EventEntry>         m_eventEntryList;
    Mutex                    m_eventEntryListMutex;
    Application             *q;                             // Initialized in base class
};

}

#include <core/private/posix/application_p.h>

#endif //APPLICATION_P_H
