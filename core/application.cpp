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

#include "application.h"
#include "private/application_p.h"
#include "interfaces/protocol_handler.h"
#include "private/object_p.h"

namespace IdealCore {

Application::Application(int argc, char **argv)
    : IDEAL_SIGNAL_INIT(invalidOption)
    , IDEAL_SIGNAL_INIT(missingParameter)
    , d(new PrivateImpl(this))
{
    d->m_argc = argc;
    d->m_argv = argv;
    static_cast<Object*>(this)->d->m_application = this;
}

Application::~Application()
{
    List<ProtocolHandler*>::iterator it;
    ContextMutexLocker cml(d->m_protocolHandlerCacheMutex);
    for (it = d->m_protocolHandlerCache.begin(); it != d->m_protocolHandlerCache.end(); ++it) {
        delete *it;
    }
    d->m_protocolHandlerCache.clear();
    delete d;
}

void Application::addOptionWithoutArg(Option &option, char optChar, const char *longOpt)
{
    d->addOptionWithoutArg(option, optChar, longOpt);
}

void Application::addOptionWithMandatoryArg(Option &option, char optChar, const char *longOpt)
{
    d->addOptionWithMandatoryArg(option, optChar, longOpt);
}

void Application::addOptionWithOptionalArg(Option &option, char optChar, const char *longOpt)
{
    d->addOptionWithOptionalArg(option, optChar, longOpt);
}

List<char*> Application::parseOptions(ParsingStrictness parsingStrictness, FailVerbosity failVerbosity)
{
    return d->parseOptions(parsingStrictness, failVerbosity);
}

String Application::getPath(Path path) const
{
    return d->getPath(path);
}

int Application::exec()
{
    while (true) {
        d->processEvents();
        d->processDelayedDeletions();
        d->unloadUnneededDynamicLibraries();
    }
    return 0;
}

void Application::quit()
{
    d->quit();
}

}
