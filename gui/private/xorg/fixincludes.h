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

namespace IdealGUI {

static const unsigned int CreateNotify_    = CreateNotify;
static const unsigned int MapNotify_       = MapNotify;
static const unsigned int UnmapNotify_     = UnmapNotify;
static const unsigned int MotionNotify_    = MotionNotify;
static const unsigned int EnterNotify_     = EnterNotify;
static const unsigned int LeaveNotify_     = LeaveNotify;
static const unsigned int ButtonPress_     = ButtonPress;
static const unsigned int ButtonRelease_   = ButtonRelease;
static const unsigned int KeyPress_        = KeyPress;
static const unsigned int KeyRelease_      = KeyRelease;
static const unsigned int ConfigureNotify_ = ConfigureNotify;
static const unsigned int Expose_          = Expose;
static const unsigned int FocusIn_         = FocusIn;
static const unsigned int FocusOut_        = FocusOut;

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
