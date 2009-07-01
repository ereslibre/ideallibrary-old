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

#include "pushbutton.h"

#include <core/event.h>
#include <gui/painter.h>
#include <gui/application.h>

namespace IdealGUI {

class PushButton::Private
{
public:
    Private(PushButton *q);
    ~Private();

    StyleInfo  *m_styleInfo;
    PushButton *q;
};

PushButton::Private::Private(PushButton *q)
    : m_styleInfo(new StyleInfo)
    , q(q)
{
}

PushButton::Private::~Private()
{
    delete m_styleInfo;
}

PushButton::PushButton(Object *parent)
    : Widget(parent)
    , IDEAL_SIGNAL_INIT(clicked)
    , d(new Private(this))
{
}

PushButton::~PushButton()
{
    delete d;
}

Size PushButton::minimumSize() const
{
    // FIXME
    return Size(102, 41);
}

Widget::StyleInfo *PushButton::styleInfo() const
{
    // FIXME: do something automatic and less memory consuming
    Widget::StyleInfo *wstyleInfo = Widget::styleInfo();
    d->m_styleInfo->isFocused = wstyleInfo->isFocused;
    d->m_styleInfo->isHovered = wstyleInfo->isHovered;
    d->m_styleInfo->isPressed = wstyleInfo->isPressed;
    return d->m_styleInfo;
}

IdealCore::String PushButton::text() const
{
    return d->m_styleInfo->text;
}

void PushButton::setText(const IdealCore::String &text)
{
    d->m_styleInfo->text = text;
}

bool PushButton::event(IdealCore::Event *event)
{
    const Widget::StyleInfo *sInfo = styleInfo();
    switch (event->type()) {
        case IdealCore::Event::ButtonRelease:
            if (sInfo->isPressed) {
                emit(clicked);
            }
            break;
        default:
            break;
    }
    return Widget::event(event);
}

}
