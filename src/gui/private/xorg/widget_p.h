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

#ifndef WIDGET_P_H_XORG
#define WIDGET_P_H_XORG

#include <cairo.h>

#include <X11/Xlib.h>
#include "fixincludes.h"

#include <gui/widget.h>
#include <gui/private/widget_p.h>

namespace IdealGUI {

class Widget::PrivateImpl
    : public Widget::Private
{
public:
    PrivateImpl(Widget *q);
    virtual ~PrivateImpl();

    Window           m_window;
    cairo_surface_t *m_cs;
};

}

#endif //WIDGET_P_H_XORG
