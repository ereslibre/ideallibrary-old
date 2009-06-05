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

#ifndef WIDGET_H
#define WIDGET_H

#include <ideal_export.h>
#include <core/object.h>

namespace IdealCore {
    class Event;
}

namespace IdealGUI {

class IDEAL_EXPORT Widget
    : public IdealCore::Object
{
    friend class Application;
    friend class Painter;

public:
    Widget(Object *parent);
    virtual ~Widget();

    // temporary parameters until we have layouting code
    void show(int x = 0, int y = 0, int width = 300, int height = 300);
    void hide();

    Widget *parentWidget() const;

protected:
    virtual bool event(IdealCore::Event *event);

private:
    class Private;
    Private *const d;
};

}

#endif //WIDGET_H
