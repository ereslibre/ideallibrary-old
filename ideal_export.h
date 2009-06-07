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

#ifndef IDEAL_EXPORT_H
#define IDEAL_EXPORT_H

#include <ideal_conf.h>
#include <iostream>

#define IDEAL_SIGNAL(name, ...) const IdealCore::Signal<__VA_ARGS__> name
#define IDEAL_SIGNAL_INIT(name, ...) name(IdealCore::Signal<__VA_ARGS__>(this, #name, #__VA_ARGS__))

#ifndef __GNUC__
#define __attribute__(x)
#endif

#define IDEAL_NO_EXPORT __attribute__ ((visibility("hidden")))
#define IDEAL_EXPORT __attribute__ ((visibility("default")))
#define IDEAL_EXPORT_DEPRECATED IDEAL_EXPORT __attribute__ ((deprecated))
#define IDEAL_UNUSED(expr) do { (void)(expr); } while (0)
#define IDEAL_POSSIBLY_UNUSED __attribute__ ((unused))

//### WARNING: system-dependant code in public header

namespace IdealCore {

static pthread_mutex_t outputMutex        IDEAL_POSSIBLY_UNUSED = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t warningOutputMutex IDEAL_POSSIBLY_UNUSED = PTHREAD_MUTEX_INITIALIZER;

}

#define IDEAL_DEBUG_WARNING(message) do {                                                                                                          \
                                         pthread_mutex_lock(&IdealCore::warningOutputMutex);                                                       \
                                         std::cerr << __FILE__ << ": " << __LINE__ << " at " << __func__ << ": WARNING: " << message << std::endl; \
                                         pthread_mutex_unlock(&IdealCore::warningOutputMutex);                                                     \
                                     } while(0)

#ifndef NDEBUG
#define IDEAL_DEBUG(message) do {                                                                                                 \
                                 pthread_mutex_lock(&IdealCore::outputMutex);                                                     \
                                 std::cout << __FILE__ << ": " << __LINE__ << " at " << __func__ << ": " << message << std::endl; \
                                 pthread_mutex_unlock(&IdealCore::outputMutex);                                                   \
                             } while(0)
#define IDEAL_SDEBUG(message) do {                                               \
                                  pthread_mutex_lock(&IdealCore::outputMutex);   \
                                  std::cout << message << std::endl;             \
                                  pthread_mutex_unlock(&IdealCore::outputMutex); \
                              } while(0)
#else
#define IDEAL_DEBUG(message)
#define IDEAL_SDEBUG(message)
#endif

#define D_I static_cast<PrivateImpl*>(this)

#endif //IDEAL_EXPORT_H
