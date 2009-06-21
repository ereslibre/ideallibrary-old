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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <ideal_export.h>
#include <core/object.h>
#include <core/option.h>
#include <core/ideal_string.h>

namespace IdealCore {

class Event;

/**
  * @class Application application.h core/application.h
  *
  * The application. All ideal library based applications need an instance of this class.
  * It allows you to parse options passed to the application when it was launched, as well as other
  * miscellaneous operations. Usually, all applications based on the Ideal Library will look like
  * this on the very beginning:
  *
  * @code
  * #include <core/application.h>
  *
  * int main(int argc, char **argv)
  * {
  *     Application app(argc, argv);
  *     // code
  *     return 0;
  * }
  * @endcode
  *
  * This is an example of an extremely simple application. This application will not even have
  * event handling, because a main event loop hasn't been launched. The following example will be
  * able to handle events:
  *
  * @code
  * #include <core/application.h>
  *
  * int main(int argc, char **argv)
  * {
  *     Application app(argc, argv);
  *     // code
  *     return app.exec(); // launch the main event loop, and process events until quit() is called in app
  * }
  * @endcode
  *
  * So, for example, if you have a top-level object in your application, it would look like this:
  *
  * @code
  * #include <core/object.h>
  * #include <core/application.h>
  *
  * int main(int argc, char **argv)
  * {
  *     Application app(argc, argv);
  *     Object *object = new Object(&app);
  *     // code
  *     return app.exec();
  * }
  * @endcode
  *
  * @author Rafael Fernández López <ereslibre@ereslibre.es>
  */
class IDEAL_EXPORT Application
    : public Object
{
    friend class Timer;
    friend class Object;
    friend class Module;
    friend class File;

public:
    enum ParsingStrictness {
        Flexible = 0,  ///< Will allow long options to start with a single '-'
        Strict         ///< Short options start with '-', long options start with '--'.
                       ///< Otherwise the parsing will fail.
    };

    enum FailVerbosity {
        FailSilently = 0,  ///< Will fail silently. No output on screen.
        FailNormally       ///< May generate screen output.
    };

    enum Path {
        Global = 0,  ///< Environment variable $PATH
        Library,     ///< Environment variable $LD_LIBRARY_PATH
        PkgConfig,   ///< Environment variable $PKG_CONFIG_PATH
        Home,        ///< Home path of the current user
        Modules      ///< List of paths separated by colons where modules for this application are located
#ifndef NDEBUG
      , UnitTest     ///< @internal. Path for unit testing with files/folders
#endif
    };

    /**
      * Constructs a new application object with @p argc number of args, and @p argv arguments.
      * If @p name is an empty string, @p argv[0] will be used as the application name.
      *
      * @see name
      */
    Application(int argc, char **argv, const String &name = String());
    virtual ~Application();

    /**
      * @return The prefix of this application. This returns an empty string by default, and should
      *         be reimplemented in order to look in the correct place for modules and extensions.
      */
    virtual String prefix() const;

    /**
      * @return The application name in a system fashion. For example, "myfavouriteapp", but not
      *         "My Favourite App".
      *
      * @note This will be used among other things to look for modules and extensions on the correct
      *       place.
      */
    String name() const;

    /**
      * An option without argument will be added to the list of possible options being passed
      * from the console invocation. This is something of the kind "ls -a".
      *
      * @param option When parsed, this field will contain the information updated, if this
      *               flag was set, if it got arguments... @see Option
      *
      * @param optChar The char that enables this flag. '\0' if you only want to set a long option.
      *
      * @param longOpt The long option synonym (e.g. 'ls -a' == 'ls --all'). An empty string if
      *                you only want to set an optChar.
      *
      * @note optChar and longOpt cannot be at the same time '\0' and 0 respectively.
      */
    void addOptionWithoutArg(Option &option, char optChar, const char *longOpt = 0);

    /**
      * Performs the same action as addOptionWithoutArg, only that this option will expect a
      * mandatory argument.
      *
      * @see addOptionWithoutArg
      */
    void addOptionWithMandatoryArg(Option &option, char optChar, const char *longOpt = 0);

    /**
      * Performs the same action as addOptionWithoutArg, only that this option will expect an
      * optional argument.
      *
      * @see addOptionWithoutArg
      */
    void addOptionWithOptionalArg(Option &option, char optChar, const char *longOpt = 0);

    /**
      * Will try to parse the options given by addOptionWithoutArg, addOptionWithMandatoryArg and
      * addOptionWithOptionalArg. The Option objects passed to each function called will result updated if
      * the flag was found.
      *
      * @param parsingStrictness If Flexible is set, long options are allowed to start with a single '-'.
      *        Otherwise, '-' means a short options while '--' means a long option.
      */
    List<char*> parseOptions(ParsingStrictness parsingStrictness = Flexible, FailVerbosity failVerbosity = FailSilently);

    /**
      * @return The default path specified by @p path.
      */
    String getPath(Path path) const;

    /**
      * Starts the main event loop of the application. This event loop will take care of timer
      * events, as well as sockets. Also, it will work on collecting zombie children that were
      * started through Process::exec(). Usually written as:
      *
      * @code
      * int main(int argc, char **argv)
      * {
      *     Application app(argc, argv);
      *     // something to do here
      *     return app.exec();
      * }
      * @endcode
      *
      * @return the result of the execution.
      */
    virtual int exec();

    /**
      * Quits the running application.
      */
    void quit();

public:
    /**
      * Signal emitted when an invalid option has been given to the arguments of the application.
      */
    IDEAL_SIGNAL(invalidOption);

    /**
      * Signal emitted when an option was expecting a non-existant parameter. This option was added
      * with addOptionWithMandatoryArg.
      */
    IDEAL_SIGNAL(missingParameter);

private:
    class Private;
    class PrivateImpl;
    Private *const d;
};

}

#endif //APPLICATION_H
