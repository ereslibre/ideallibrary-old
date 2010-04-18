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

#include "widget.h"
#include "private/widget_p.h"

#include <core/event.h>
#include <gui/application.h>

namespace IdealGUI {

Widget::Private::Private(Widget *q)
    : q(q)
{
}

Widget::Private::~Private()
{
}

Widget::Widget(Object *parent)
    : Object(parent)
    , m_styleInfo(new StyleInfo)
    , d(new PrivateImpl(this))
{
    d->m_parentWidget = dynamic_cast<Widget*>(parent);
}

Widget::~Widget()
{
    delete d;
    delete m_styleInfo;
}

const Widget::StyleInfo *Widget::styleInfo() const
{
    return m_styleInfo;
}

void Widget::drawWidget()
{
    GUIApplication()->style()->drawWidget(this);
}

Widget *Widget::parentWidget() const
{
    return d->m_parentWidget;
}

Application *Widget::GUIApplication() const
{
    return dynamic_cast<Application*>(application());
}

bool Widget::event(IdealCore::Event *event)
{
    bool drawPending = true;
    switch (event->type()) {
        case IdealCore::Event::MapNotify:
        case IdealCore::Event::Expose:
            break;
        case IdealCore::Event::EnterNotify:
            m_styleInfo->isHovered = true;
            break;
        case IdealCore::Event::LeaveNotify:
            m_styleInfo->isHovered = false;
            m_styleInfo->isPressed = false;
            break;
        case IdealCore::Event::ButtonPress:
            m_styleInfo->isPressed = true;
            break;
        case IdealCore::Event::ButtonRelease:
            m_styleInfo->isPressed = false;
            break;
        default:
            drawPending = false;
            break;
    }
    if (drawPending) {
        drawWidget();
    }
    return false;
}

}
