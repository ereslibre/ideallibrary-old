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

#ifndef OPTION_H
#define OPTION_H

#include <ideal_export.h>

namespace IdealCore {

/**
  * @class Option option.h core/option.h
  *
  * Class used to easily parse options and parameters passed to the application from the command
  * line.
  *
  * @note Please note that for options being actually parsed after having used
  *       Application::addOptionWithoutArg, Application::addOptionWithMandatoryArg or
  *       Application::addOptionWithOptionalArg you have to call to Application::parseOptions.
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Option
{
    friend class Application;

public:
    Option();
    virtual ~Option();

    /**
      * @return whether this option was added to the options of the application using
      *         Application::addOptionWithoutArg, Application::addOptionWithMandatoryArg or
      *         Application::addOptionWithOptionalArg.
      */
    bool isValid() const;

    /**
      * @return whether this option was set by the user from the command line or not.
      */
    bool isOptSet() const;

    /**
      * @note only options added with Application::addOptionWithMandatoryArg or with
      *       Application::addOptionWithOptionalArg can have this attribute set.
      *
      * @return the argument of this option in case that an argument was given on the command line
      *         to this option.
      */
    char *optArg() const;

private:
    class Private;
    Private *const d;
};

}

#endif //OPTION_H
