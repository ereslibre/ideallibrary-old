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

#include "widget.h"
#include "xorg/widget_p.h"

namespace IdealGUI {

Widget::Widget(Object *parent)
    : Object(parent)
    , d(new Private(this))
{
    d->m_parentWidget = dynamic_cast<Widget*>(parent);
}

Widget::~Widget()
{
    delete d;
}

bool Widget::event(IdealCore::Event *event)
{
    return false;
}

void Widget::show(int x, int y, int width, int height)
{
    d->show(x, y, width, height);
}

void Widget::hide()
{
    d->hide();
}

Widget *Widget::parentWidget() const
{
    return d->m_parentWidget;
}

}
