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
#include <gui/point.h>
#include <gui/size.h>

namespace IdealCore {
    class Event;
}

namespace IdealGUI {

class Application;

/**
  * @class Widget widget.h gui/widget.h
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Widget
    : public IdealCore::Object
{
    friend class Application;
    friend class Painter;

public:
    struct StyleInfo {
        StyleInfo()
            : isFocused(false)
            , isHovered(false)
            , isPressed(false) { }

        bool isFocused;
        bool isHovered;
        bool isPressed;
    };

    Widget(Object *parent);
    virtual ~Widget();

    // temporary parameters until we have layouting code
    void show(const Point &point);
    void hide();

    virtual Size minimumSize() const = 0;

    const StyleInfo *styleInfo() const;

    virtual void drawWidget();

    Widget *parentWidget() const;

    Application *GUIApplication() const;

protected:
    virtual bool event(IdealCore::Event *event);

protected:
    StyleInfo *m_styleInfo;

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //WIDGET_H
