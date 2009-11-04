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

#ifndef FIXINCLUDES_H
#define FIXINCLUDES_H

#include <ideal_export.h>

namespace IdealGUI {

static const iuint32 CreateNotify_    = CreateNotify;
static const iuint32 MapNotify_       = MapNotify;
static const iuint32 UnmapNotify_     = UnmapNotify;
static const iuint32 MotionNotify_    = MotionNotify;
static const iuint32 EnterNotify_     = EnterNotify;
static const iuint32 LeaveNotify_     = LeaveNotify;
static const iuint32 ButtonPress_     = ButtonPress;
static const iuint32 ButtonRelease_   = ButtonRelease;
static const iuint32 KeyPress_        = KeyPress;
static const iuint32 KeyRelease_      = KeyRelease;
static const iuint32 ConfigureNotify_ = ConfigureNotify;
static const iuint32 Expose_          = Expose;
static const iuint32 FocusIn_         = FocusIn;
static const iuint32 FocusOut_        = FocusOut;

}

#undef CreateNotify
#undef MapNotify
#undef UnmapNotify
#undef MotionNotify
#undef EnterNotify
#undef LeaveNotify
#undef ButtonPress
#undef ButtonRelease
#undef KeyPress
#undef KeyRelease
#undef ConfigureNotify
#undef Expose
#undef FocusIn
#undef FocusOut

#endif
