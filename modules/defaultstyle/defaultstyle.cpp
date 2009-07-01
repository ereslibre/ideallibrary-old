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

#include "defaultstyle.h"

#include <gui/painter.h>
#include <gui/widget.h>
#include <gui/pushbutton.h>

namespace IdealGUI {

void DefaultStyle::drawWidget(Widget *widget) const
{
    const Widget::StyleInfo sInfo = widget->styleInfo();
    Painter p(widget);
    if (sInfo.isPressed && sInfo.isHovered) {
        p.setPenColor(0, 0, 1.0);
    } else if (sInfo.isHovered) {
        p.setPenColor(1.0, 0, 0);
    }
    p.drawRectangle(1, 1, 99, 39);
    p.drawText(10, 25, "Test");
}

IdealCore::String DefaultStyle::name() const
{
    return "Style by default";
}

IdealCore::String DefaultStyle::description() const
{
    return "The Ideal Library's style by default";
}

IdealCore::String DefaultStyle::author() const
{
    return "Rafael Fernández López";
}

IdealCore::String DefaultStyle::version() const
{
    return IDEALLIBRARY_VERSION;
}

}

EXTENSION_ENTRY_POINT(defaultStyle, IdealGUI::DefaultStyle)
