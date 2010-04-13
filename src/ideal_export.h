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

#ifndef IDEAL_EXPORT_H
#define IDEAL_EXPORT_H

#include <ideal_conf.h>
#include <ideal_globals.h>
#include <iostream>

#define IDEAL_SIGNAL(name, ...) const IdealCore::Signal<__VA_ARGS__> name
#define IDEAL_SIGNAL_INIT(name, ...) name(IdealCore::Signal<__VA_ARGS__>(this, #name, #__VA_ARGS__))

#ifndef __GNUC__
#define __attribute__(x)
#define __builtin_expect((x), y) x
#endif

#define IDEAL_NO_EXPORT __attribute__ ((visibility("hidden")))
#define IDEAL_EXPORT __attribute__ ((visibility("default")))
#define IDEAL_EXPORT_DEPRECATED IDEAL_EXPORT __attribute__ ((deprecated))
#define IDEAL_UNUSED(expr) do { (void)(expr); } while (0)
#define IDEAL_POSSIBLY_UNUSED __attribute__ ((unused))
#define IDEAL_LIKELY(x)   __builtin_expect((x), 1)
#define IDEAL_UNLIKELY(x) __builtin_expect((x), 0)

#ifndef MUTEX_H

#include <core/mutex.h>
#include <core/context_mutex_locker.h>

namespace IdealCore {

static Mutex outputMutex;

}

#endif //MUTEX_H

#define IDEAL_DEBUG_WARNING(message) do {                                                                                                          \
                                         IdealCore::ContextMutexLocker cml(IdealCore::outputMutex);                                                \
                                         std::cerr << __FILE__ << ": " << __LINE__ << " at " << __func__ << ": WARNING: " << message << std::endl; \
                                     } while (0)

#ifndef NDEBUG
#define IDEAL_DEBUG(message) do {                                                                                                 \
                                 IdealCore::ContextMutexLocker cml(IdealCore::outputMutex);                                       \
                                 std::cout << __FILE__ << ": " << __LINE__ << " at " << __func__ << ": " << message << std::endl; \
                             } while (0)
#define IDEAL_SDEBUG(message) do {                                                           \
                                  IdealCore::ContextMutexLocker cml(IdealCore::outputMutex); \
                                  std::cout << message << std::endl;                         \
                              } while (0)
#else
#define IDEAL_DEBUG(message) do {        \
                             } while (0)
#define IDEAL_SDEBUG(message) do {        \
                              } while (0)
#endif

#define D_I static_cast<PrivateImpl*>(d)

#define IDEAL_FOREVER for(;;)

#endif //IDEAL_EXPORT_H
