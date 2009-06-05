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

#ifndef PROCESS_P_H_POSIX
#define PROCESS_P_H_POSIX

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <core/private/process_p.h>

namespace IdealCore {

class Process::PrivateImpl
    : public Process::Private
{
public:
    PrivateImpl(Process *q);

    pid_t m_pid;
};

}

#endif //PROCESS_P_H_POSIX

