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

#include <core/event.h>
#include <gui/widget.h>
#include <gui/painter.h>
#include <gui/application.h>

using namespace IdealGUI;

class MyWidget
    : public Widget
{
public:
    MyWidget(Object *parent);

    virtual bool event(IdealCore::Event *event);
};

bool MyWidget::event(IdealCore::Event *event)
{
    if (event->type() == IdealCore::Event::MapNotify ||
        event->type() == IdealCore::Event::Expose) {
        Painter p(this);
        p.drawLine(10, 10, 100, 100);
        p.drawLine(200, 100, 300, 300);
        p.drawLine(200, 400, 600, 200);
        p.drawLine(280, 160, 600, 400);
    }
    return false;
}

class OtherWidget
    : public Widget
{
public:
    OtherWidget(Object *parent);

protected:
    virtual bool event(IdealCore::Event *event);
};

OtherWidget::OtherWidget(Object *parent)
    : Widget(parent)
{
}

bool OtherWidget::event(IdealCore::Event *event)
{
    if (event->type() == IdealCore::Event::MapNotify ||
        event->type() == IdealCore::Event::Expose) {
        Painter p(this);
        p.drawRectangle(0, 0, 100, 100);
    } else if (event->type() == IdealCore::Event::ButtonPress) {
        Painter p(this);
        p.drawRectangle(0, 0, 100, 100);
        p.drawLine(0, 0, 100, 100);
        p.drawLine(100, 0, 0, 100);
    } else if (event->type() == IdealCore::Event::ButtonRelease) {
        application()->quit();
    }
    return false;
}

MyWidget::MyWidget(Object *parent)
    : Widget(parent)
{
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    MyWidget *myWidget = new MyWidget(&app);
    OtherWidget *otherWidget = new OtherWidget(myWidget);
    myWidget->show(0, 0, 640, 480);
    otherWidget->show(100, 200, 100, 100);

    return app.exec();
}
