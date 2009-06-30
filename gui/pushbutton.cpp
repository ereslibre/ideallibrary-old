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

namespace IdealGUI {

class PushButton::Private
{
public:
    Private(PushButton *q);

    void drawButton() const;

    IdealCore::String m_text;
    bool              m_mouseOver;
    bool              m_buttonPress;
    PushButton       *q;
};

PushButton::Private::Private(PushButton *q)
    : m_mouseOver(false)
    , m_buttonPress(false)
    , q(q)
{
}

void PushButton::Private::drawButton() const
{
    Painter p(q);
    if (m_buttonPress && m_mouseOver) {
        p.setPenColor(0, 0, 1.0);
    } else if (m_mouseOver) {
        p.setPenColor(1.0, 0, 0);
    }
    p.drawRectangle(1, 1, 99, 39);
    p.drawText(10, 25, m_text);
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
    return Size(100, 100);
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
    switch (event->type()) {
        case IdealCore::Event::Expose:
        case IdealCore::Event::MapNotify:
            d->drawButton();
            break;
        case IdealCore::Event::EnterNotify:
            d->m_mouseOver = true;
            d->drawButton();
            break;
        case IdealCore::Event::LeaveNotify:
            d->m_mouseOver = false;
            d->m_buttonPress = false;
            d->drawButton();
            break;
        case IdealCore::Event::ButtonPress:
            d->m_buttonPress = true;
            d->drawButton();
            break;
        case IdealCore::Event::ButtonRelease:
            if (d->m_buttonPress) {
                emit(clicked);
            }
            d->m_buttonPress = false;
            d->drawButton();
            break;
        default:
            break;
    }
    return false;
}

}
