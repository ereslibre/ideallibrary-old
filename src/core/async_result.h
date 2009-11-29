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

#ifndef ASYNC_RESULT_H
#define ASYNC_RESULT_H

#include <ideal_export.h>
#include <core/object.h>
#include <core/any.h>

namespace IdealCore {

/**
  * @class AsyncResult async_result.h core/async_result.h
  *
  * Capable of saving the response of an asynchronous event for being accessed later.
  *
  * @code
  * File f("ftp://server/folder/folder/file", &app);
  * AsyncResult result(&app);
  * connect(f.statResult, &result, &AsyncResult::set<ProtocolHandler::StatResult>);
  * f.stat(Thread::Joinable)->execAndJoin();
  * if (result.resultReceived()) {
  *     ProtocolHandler::StatResult statResult = result.get<ProtocolHandler::StatResult>(0);
  *     // work with statResult fields normally
  * }
  * @endcode
  *
  * If the signal provides more than one parameter, the mechanism is the same:
  *
  * @code
  * File f("ftp://server/folder/folder/file", &app);
  * AsyncResult result(&app);
  * connectMulti(f.statResult, &result, &AsyncResult::set<Object*, ProtocolHandler::StatResult>);
  * f.stat(Thread::Joinable)->execAndJoin();
  * if (result.resultReceived()) {
  *     Object *sender = result.get<Object*>(0);
  *     ProtocolHandler::StatResult statResult = result.get<ProtocolHandler::StatResult>(1);
  *     // work with sender and statResult fields normally
  * }
  * @endcode
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT AsyncResult
    : public Object
{
public:
    AsyncResult(Object *parent);
    virtual ~AsyncResult();

    template <typename... Values>
    void set(const Values&... values);

    template <typename T>
    T get(iint32 i) const;

    /**
      * @return How many parameters the signal returned.
      *
      * @note It is possible that size() returns 0 but the signal was received (but it had no
      *       arguments). If you want to know if the signal was received, you can use resultReceived()
      *       method.
      */
    iint32 size() const;

    /**
      * @return Whether this object set() method was called.
      */
    bool resultReceived() const;

    /**
      * Clears the current AsyncResult object state.
      */
    void clear();

public:
    IDEAL_SIGNAL(resultSet);

private:
    Any   *m_values;
    iint32 m_size;
    bool   m_resultReceived;
};

template <typename... Values>
void AsyncResult::set(const Values&... values)
{
    delete[] m_values;
    m_size = sizeof...(Values);
    if (m_size) {
        m_values = new Any[m_size];
        Any val[sizeof...(Values)] = { values... };
        for (iint32 i = 0; i < m_size; ++i) {
            m_values[i] = val[i];
        }
    } else {
        m_values = 0;
    }
    m_resultReceived = true;
    emit(resultSet);
}

template <typename T>
T AsyncResult::get(iint32 i) const
{
    if (i >= 0 && i < m_size) {
        return m_values[i].get<T>();
    }
    IDEAL_DEBUG_WARNING("the requested parameter is out of range");
    return T();
}

}

#endif //ASYNC_RESULT_H
