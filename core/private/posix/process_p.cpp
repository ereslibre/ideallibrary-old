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

#include <core/process.h>
#include "process_p.h"

namespace IdealCore {

void ProcessCommand::run()
{
    if (command.empty()) {
        return;
    }
    List<String> res = command.split(' ');
    const char **params = new const char*[res.size() + 1];
    List<String>::iterator it;
    int i = 0;
    for (it = res.begin(); it != res.end(); ++it, ++i) {
        params[i] = (*it).data();
    }
    params[i] = 0;
    execvp(params[0], const_cast<char* const*>(params));
}

Process::PrivateImpl::PrivateImpl(Process *q)
    : Private(q)
{
}

void Process::exec()
{
    const pid_t pid = fork();
    switch (pid) {
        case 0:
            D_I->m_pid = getpid();
            run();
            exit(EXIT_SUCCESS);
        default:
            break;
    }
}

void Process::join()
{
    waitpid(D_I->m_pid, NULL, 0);
}

}

