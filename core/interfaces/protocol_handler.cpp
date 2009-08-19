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

#include "protocol_handler.h"

namespace IdealCore {

ProtocolHandler::ProtocolHandler()
    : IDEAL_SIGNAL_INIT(dataRead, ByteStream)
    , IDEAL_SIGNAL_INIT(dirRead, List<Uri>)
    , IDEAL_SIGNAL_INIT(error, ErrorCode)
    , m_weight(0)
{
}

ProtocolHandler::StatResult::StatResult()
    : isValid(false)
    , exists(false)
    , type(0 /* File::UnknownType */)
    , permissions(0 /* File::UnknownPermissions */)
    , size(-1)
    , lastAccessed(-1)
    , lastModified(-1)
{
}

}
