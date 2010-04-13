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

#ifndef WIDGET_P_H
#define WIDGET_P_H

#include <core/mutex.h>

#include <gui/widget.h>

namespace IdealGUI {

class Widget::Private
{
public:
    Private(Widget *q);
    virtual ~Private();

    Widget          *m_parentWidget;
    IdealCore::Mutex m_mutex;
    Widget          *q;
};

}

#include <gui/private/xorg/widget_p.h>

#endif //WIDGET_P_H
