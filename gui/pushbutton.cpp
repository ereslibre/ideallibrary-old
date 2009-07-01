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

    IdealCore::String m_text;
    PushButton       *q;
};

PushButton::Private::Private(PushButton *q)
    : q(q)
{
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

IdealCore::String PushButton::text() const
{
    return d->m_text;
}

void PushButton::setText(const IdealCore::String &text)
{
    d->m_text = text;
}

bool PushButton::event(IdealCore::Event *event)
{
    const Widget::StyleInfo sInfo = styleInfo();
    switch (event->type()) {
        case IdealCore::Event::ButtonRelease:
            if (sInfo.isPressed) {
                emit(clicked);
            }
            break;
        default:
            break;
    }
    return Widget::event(event);
}

}
