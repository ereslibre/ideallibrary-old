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

#include <X11/extensions/Xrender.h>
#include "fixincludes.h"

#include <gui/widget.h>

namespace IdealGUI {

class Widget::Private
{
public:
    Private(Widget *q);
    ~Private();

    void show(int x, int y, int width, int height);
    void hide();

    Widget            *m_parentWidget;
    Window             m_window;
    Picture            m_fillPicture;
    Picture            m_picture;
    XRenderPictFormat *m_maskFormat;
    Widget            *q;
};

}
