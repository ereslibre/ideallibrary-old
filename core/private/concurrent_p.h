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

#ifndef CONCURRENT_P_H
#define CONCURRENT_P_H

#include <core/concurrent.h>

namespace IdealCore {

class Concurrent::Private
{
public:
    Private(Concurrent *q, Type type);
    virtual ~Private();

    void exec();
    void join();
    State state() const;

    State        m_state;   // Initialized in base class
    Type         m_type;    // Initialized in base class
    Concurrent  *q;         // Initialized in base class
};

}

#include <core/private/posix/concurrent_p.h>

#endif //CONCURRENT_P_H

