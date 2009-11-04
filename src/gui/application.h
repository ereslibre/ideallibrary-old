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

#ifndef APPLICATION_GUI_H
#define APPLICATION_GUI_H

#include <ideal_export.h>
#include <core/application.h>
#include <gui/interfaces/style.h>

/**
  * The IdealGUI namespace.
  *
  * It provides a way of working with GUIs.
  */
namespace IdealGUI {

/**
  * @class Application application.h gui/application.h
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Application
    : public IdealCore::Application
{
    friend class Widget;
    friend class Painter;

public:
    Application(iint32 argc, char **argv);
    virtual ~Application();

    virtual iint32 exec();

    Style *style() const;
    
private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //APPLICATION_GUI_H
