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

#include <core/event.h>
#include <core/file.h>
#include <core/thread.h>
#include <gui/widget.h>
#include <gui/painter.h>
#include <gui/application.h>

using namespace IdealGUI;

static IdealCore::File *myFile = 0;

class MyWidget
    : public Widget
{
public:
    MyWidget(Object *parent);

    virtual Size minimumSize() const;

protected:
    virtual bool event(IdealCore::Event *event);
};

MyWidget::MyWidget(Object *parent)
    : Widget(parent)
{
}

Size MyWidget::minimumSize() const
{
    return Size(220, 220);
}

bool MyWidget::event(IdealCore::Event *event)
{
    IDEAL_SDEBUG("*** Got graphical event of type " << event->type());
    if (event->type() == IdealCore::Event::MapNotify ||
        event->type() == IdealCore::Event::Expose) {
        Painter p(this);
        p.drawLine(Point(10, 10), Point(210, 210));
        p.drawLine(Point(210, 10), Point(10, 210));
        p.drawRectangle(Point(10, 10), Size(200, 200));
    } else if (event->type() == IdealCore::Event::KeyPress) {
       IDEAL_SDEBUG("*** Retrieving size of file...");
       IdealCore::Thread *stat = myFile->stat();
       stat->exec();
    }
    return false;
}

static void statRes(const IdealCore::ProtocolHandler::StatResult &statResult)
{
    IDEAL_SDEBUG("*** The size of the file \"ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz\" is:");
    IDEAL_SDEBUG("\t" << (statResult.size / (1024 * 1024)) << " MiB");
}

int main(int argc, char **argv)
{
    Application app(argc, argv);

    myFile = new IdealCore::File("ftp://ftp.kernel.org/pub/linux/kernel/v2.6/linux-2.6.22.1.tar.gz", &app);
    IdealCore::Object::connectStatic(myFile->statResult, statRes);

    MyWidget *myWidget = new MyWidget(&app);
    myWidget->show(Point(0, 0));

    IDEAL_SDEBUG("*** Press any key to get the size of a remote file");

    return app.exec();
}
