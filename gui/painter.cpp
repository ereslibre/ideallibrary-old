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

#include "painter.h"
#include "xorg/painter_p.h"

namespace IdealGUI {

Painter::Painter(Widget *widget)
    : d(new Private(widget))
{
}

Painter::~Painter()
{
    delete d;
}

void Painter::drawPoint(int x, int y)
{
    d->drawPoint(x, y);
}

void Painter::drawLine(int x1, int y1, int x2, int y2)
{
    d->drawLine(x1, y1, x2, y2);
}

void Painter::drawRectangle(int x, int y, int width, int height)
{
    d->drawRectangle(x, y, width, height);
}

void Painter::drawText(int x, int y, const IdealCore::String &text)
{
    d->drawText(x, y, text);
}

void Painter::fillRectangle(int x, int y, int width, int height)
{
    d->fillRectangle(x, y, width, height);
}

}
